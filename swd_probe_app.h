#ifndef __ARHA_FLIPPERAPP_DEMO
#define __ARHA_FLIPPERAPP_DEMO

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_speaker.h>
#include <gui/gui.h>
#include <input/input.h>

#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/subghz_file_encoder_worker.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define TAG "SWD"

#define SWD_DELAY_US 1
#define TIMER_HZ 50
#define TIMEOUT 3
#define MODE_PAGES 4

#define CDBGPWRUPREQ (1 << 28)
#define CDBGPWRUPACK (1 << 29)
#define CSYSPWRUPREQ (1 << 30)
#define CSYSPWRUPACK (1 << 31)
#define WDATAERR (1 << 7)
#define STICKYERR (1 << 5)
#define STAT_ERROR_FLAGS (WDATAERR | STICKYERR)

#define MEMAP_CSW 0x00
#define MEMAP_TAR 0x04
#define MEMAP_DRW 0x0C
#define AP_IDR 0xFC

typedef enum { KeyNone, KeyUp, KeyRight, KeyDown, KeyLeft, KeyOK } KeyCode;

typedef enum {
    EventTimerTick,
    EventKeyPress,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef struct {
    uint32_t ctrlstat;
    bool ctrlstat_ok;
    uint32_t dlcr;
    bool dlcr_ok;
    uint32_t dlpidr;
    bool dlpidr_ok;
    uint32_t dpidr;
    bool dpidr_ok;
    uint32_t eventstat;
    bool eventstat_ok;
    uint32_t select;
    bool select_ok;
    uint32_t targetid;
    bool targetid_ok;
} swd_dpreg_t;

typedef struct {
    bool ok;
    bool tested;
    uint8_t revision;
    uint16_t designer;
    uint8_t class;
    uint8_t variant;
    uint8_t type;
} swd_apidr_info_t;

typedef struct {
    uint8_t revision;
    uint8_t partno;
    uint8_t version;
    uint16_t designer;
} swd_dpidr_info_t;

typedef struct {
    uint8_t revision;
    uint16_t partno;
    uint16_t designer;
} swd_targetid_info_t;

typedef struct {
    KeyCode last_key;

    FuriTimer* _timer;
    FuriMessageQueue* _event_queue;
    NotificationApp* notification;

    swd_targetid_info_t targetid_info;
    swd_dpidr_info_t dpidr_info;
    swd_dpreg_t dp_regs;
    swd_apidr_info_t apidr_info[256];

    uint8_t current_mask_id;
    uint32_t current_mask;
    uint8_t io_swc;
    uint8_t io_swd;
    uint8_t io_num_swc;
    uint8_t io_num_swd;
    uint32_t detected_timeout;
    bool detected;
    bool detected_device;
    bool detected_notified;
    uint8_t mode_page;
    uint8_t ap_pos;

    uint32_t hex_addr;
    uint8_t hex_select;
    uint8_t hex_buffer[32];
    uint8_t hex_buffer_valid[8];
    uint8_t hex_read_delay;

    char state_string[32];
} AppFSM;

const NotificationSequence seq_c_minor = {
    &message_note_c4,
    &message_delay_100,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_100,
    &message_sound_off,
    &message_delay_10,

    &message_note_g4,
    &message_delay_100,
    &message_sound_off,
    &message_delay_10,

    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

#endif