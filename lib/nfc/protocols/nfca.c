#include "nfca.h"
#include <string.h>
#include <stdio.h>
#include <furi.h>

#define NFCA_CMD_RATS (0xE0U)

#define NFCA_CRC_INIT (0x6363)

#define NFCA_F_SIG  (13560000.0)                    /* [Hz] NFC frequency */
#define NFCA_F_SUB  (NFCA_F_SIG/16)                 /* [Hz] NFC subcarrier frequency fs/16 */
#define T_SUB       (1000000000000.0f / NFCA_F_SUB) /* [ps] subcarrier period = 1/NFCA_F_SUB */
#define T_SUB_PHASE (T_SUB/2)                       /* [ps] a single subcarrier phase */

#define NFCA_SIGNAL_MAX_EDGES (1350)

typedef struct {
    uint8_t cmd;
    uint8_t param;
} nfca_cmd_rats;

static uint8_t nfca_default_ats[] = {0x05, 0x78, 0x80, 0x80, 0x00};

static uint8_t nfca_sleep_req[] = {0x50, 0x00};

uint16_t nfca_get_crc16(uint8_t* buff, uint16_t len) {
    uint16_t crc = NFCA_CRC_INIT;
    uint8_t byte = 0;

    for(uint8_t i = 0; i < len; i++) {
        byte = buff[i];
        byte ^= (uint8_t)(crc & 0xff);
        byte ^= byte << 4;
        crc = (crc >> 8) ^ (((uint16_t)byte) << 8) ^ (((uint16_t)byte) << 3) ^
              (((uint16_t)byte) >> 4);
    }

    return crc;
}

void nfca_append_crc16(uint8_t* buff, uint16_t len) {
    uint16_t crc = nfca_get_crc16(buff, len);
    buff[len] = (uint8_t)crc;
    buff[len + 1] = (uint8_t)(crc >> 8);
}

bool nfca_emulation_handler(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len) {
    bool sleep = false;
    uint8_t rx_bytes = buff_rx_len / 8;

    if(rx_bytes == sizeof(nfca_sleep_req) && !memcmp(buff_rx, nfca_sleep_req, rx_bytes)) {
        sleep = true;
    } else if(rx_bytes == sizeof(nfca_cmd_rats) && buff_rx[0] == NFCA_CMD_RATS) {
        memcpy(buff_tx, nfca_default_ats, sizeof(nfca_default_ats));
        *buff_tx_len = sizeof(nfca_default_ats) * 8;
    }

    return sleep;
}

static void nfca_add_bit(DigitalSignal* signal, bool bit) {

    signal->start_level = bit;
    for(size_t i = 0; i < 16; i++) {
        signal->edge_timings[signal->edge_cnt++] = T_SUB_PHASE;
    }
}

static void nfca_add_byte(NfcaSignal* nfca_signal, uint8_t byte, bool parity) {
    for(uint8_t i = 0; i < 8; i++) {
        if(byte & (1 << i)) {
            digital_sequence_add(nfca_signal->tx_signal, 1);
        } else {
            digital_sequence_add(nfca_signal->tx_signal, 0);
        }
    }
    if(parity) {
        digital_sequence_add(nfca_signal->tx_signal, 1);
    } else {
        digital_sequence_add(nfca_signal->tx_signal, 0);
    }
}

NfcaSignal* nfca_signal_alloc() {
    NfcaSignal* nfca_signal = malloc(sizeof(NfcaSignal));
    nfca_signal->one = digital_signal_alloc(10);
    nfca_signal->zero = digital_signal_alloc(10);
    nfca_add_bit(nfca_signal->one, true);
    nfca_add_bit(nfca_signal->zero, false);
    nfca_signal->tx_signal = digital_sequence_alloc(NFCA_SIGNAL_MAX_EDGES);

    return nfca_signal;
}

void nfca_signal_free(NfcaSignal* nfca_signal) {
    furi_assert(nfca_signal);

    digital_signal_free(nfca_signal->one);
    digital_signal_free(nfca_signal->zero);
    digital_sequence_free(nfca_signal->tx_signal);
    free(nfca_signal);
}

void nfca_signal_encode(NfcaSignal* nfca_signal, uint8_t* data, uint16_t bits, uint8_t* parity) {
    furi_assert(nfca_signal);
    furi_assert(data);
    furi_assert(parity);

    digital_sequence_clear(nfca_signal->tx_signal);

    /* add a >80/fs phase with no transition, which is >10 1-bits */
    for(int tr1_bit = 0; tr1_bit < 11; tr1_bit++) {
        digital_sequence_add(nfca_signal->tx_signal, 1);
    }

    if(bits < 8) {
        for(size_t i = 0; i < bits; i++) {
            if(FURI_BIT(data[0], i)) {
                digital_sequence_add(nfca_signal->tx_signal, 1);
            } else {
                digital_sequence_add(nfca_signal->tx_signal, 0);
            }
        }
    } else {
        for(size_t i = 0; i < bits / 8; i++) {
            nfca_add_byte(nfca_signal, data[i], parity[i / 8] & (1 << (7 - (i & 0x07))));
        }
    }
}
