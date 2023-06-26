#include "mf_ultralight_poller_i.h"

#include <furi.h>

#define TAG "MfUltralightPoller"

MfUltralightError mf_ultralight_process_error(NfcaError error) {
    MfUltralightError ret = MfUltralightErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfUltralightErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfUltralightErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfUltralightErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfUltralightErrorTimeout;
        break;
    default:
        ret = MfUltralightErrorProtocol;
        break;
    }

    return ret;
}

MfUltralightError mf_ultralight_poller_async_auth(
    MfUltralightPoller* instance,
    MfUltralightPollerAuthContext* data) {
    uint8_t auth_cmd[5] = {MF_ULTRALIGHT_CMD_AUTH};
    nfc_util_num2bytes(data->password.pass, MF_ULTRALIGHT_AUTH_PASSWORD_SIZE, &auth_cmd[1]);
    bit_buffer_copy_bytes(instance->tx_buffer, auth_cmd, sizeof(auth_cmd));

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != MF_ULTRALIGHT_AUTH_PACK_SIZE) {
            ret = MfUltralightErrorAuth;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data->pack.data, MF_ULTRALIGHT_AUTH_PACK_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_page_from_sector(
    MfUltralightPoller* instance,
    uint8_t sector,
    uint8_t tag,
    MfUltralightPageReadCommandData* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        const uint8_t select_sector_cmd[2] = {MF_ULTRALIGHT_CMD_SECTOR_SELECT, 0xff};
        bit_buffer_copy_bytes(instance->tx_buffer, select_sector_cmd, sizeof(select_sector_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorWrongCrc) {
            FURI_LOG_D(TAG, "Failed to issue sector select command");
            ret = mf_ultralight_process_error(error);
            break;
        }

        const uint8_t read_sector_cmd[4] = {sector, 0x00, 0x00, 0x00};
        bit_buffer_copy_bytes(instance->tx_buffer, read_sector_cmd, sizeof(read_sector_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorTimeout) {
            // This is NOT a typo! The tag ACKs by not sending a response within 1ms.
            FURI_LOG_D(TAG, "Sector %u select NAK'd", sector);
            ret = MfUltralightErrorProtocol;
            break;
        }

        ret = mf_ultralight_poller_async_read_page(instance, tag, data);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_page(
    MfUltralightPoller* instance,
    uint8_t start_page,
    MfUltralightPageReadCommandData* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t read_page_cmd[2] = {MF_ULTRALIGHT_CMD_READ_PAGE, start_page};
        bit_buffer_copy_bytes(instance->tx_buffer, read_page_cmd, sizeof(read_page_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) !=
           sizeof(MfUltralightPageReadCommandData)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightPageReadCommandData));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_write_page(
    MfUltralightPoller* instance,
    uint8_t page,
    MfUltralightPage* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t write_page_cmd[MF_ULTRALIGHT_PAGE_SIZE + 2] = {MF_ULTRALIGHT_CMD_WRITE_PAGE, page};
        memcpy(&write_page_cmd[2], data->data, MF_ULTRALIGHT_PAGE_SIZE);
        bit_buffer_copy_bytes(instance->tx_buffer, write_page_cmd, sizeof(write_page_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorWrongCrc) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_buffer) != 4) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        if(!bit_buffer_starts_with_byte(instance->rx_buffer, MF_ULTRALIGHT_CMD_ACK)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_version(
    MfUltralightPoller* instance,
    MfUltralightVersion* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        const uint8_t get_version_cmd = MF_ULTRALIGHT_CMD_GET_VERSION;
        bit_buffer_copy_bytes(instance->tx_buffer, &get_version_cmd, sizeof(get_version_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(MfUltralightVersion)) {
            FURI_LOG_I(
                TAG, "Read Version failed: %d", bit_buffer_get_size_bytes(instance->rx_buffer));
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_signature(
    MfUltralightPoller* instance,
    MfUltralightSignature* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        const uint8_t read_signature_cmd[2] = {MF_ULTRALIGTH_CMD_READ_SIG, 0x00};
        bit_buffer_copy_bytes(instance->tx_buffer, read_signature_cmd, sizeof(read_signature_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(MfUltralightSignature)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t read_counter_cmd[2] = {MF_ULTRALIGHT_CMD_READ_CNT, counter_num};
        bit_buffer_copy_bytes(instance->tx_buffer, read_counter_cmd, sizeof(read_counter_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != MF_ULTRALIGHT_COUNTER_SIZE) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data->data, MF_ULTRALIGHT_COUNTER_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t tearing_falg_num,
    MfUltralightTearingFlag* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t check_tearing_cmd[2] = {MF_ULTRALIGHT_CMD_CHECK_TEARING, tearing_falg_num};
        bit_buffer_copy_bytes(instance->tx_buffer, check_tearing_cmd, sizeof(check_tearing_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(MfUltralightTearingFlag)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightTearingFlag));
    } while(false);

    return ret;
}
