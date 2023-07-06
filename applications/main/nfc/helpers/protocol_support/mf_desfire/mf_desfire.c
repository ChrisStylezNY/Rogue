#include "mf_desfire.h"
#include "mf_desfire_render.h"

#include <nfc/protocols/mf_desfire/mf_desfire_poller.h>

#include "../nfc_protocol_support_gui_common.h"
#include "../../../nfc_app_i.h"

static void nfc_scene_info_on_enter_mf_desfire(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfDesfireData* data = nfc_device_get_data(device, NfcProtocolMfDesfire);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_mf_desfire_info(data, NfcProtocolFormatTypeFull, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    widget_add_button_element(
        instance->widget,
        GuiButtonTypeRight,
        "More",
        nfc_protocol_support_common_widget_callback,
        instance);

    furi_string_free(temp_str);
}

static NfcCommand nfc_scene_read_poller_callback_mf_desfire(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolMfDesfire);

    NfcApp* instance = context;
    const MfDesfirePollerEvent* mf_desfire_event = event.data;

    if(mf_desfire_event->type == MfDesfirePollerEventTypeReadSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfDesfire, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventReadHandlerSuccess);
        return NfcCommandStop;
    }

    return NfcCommandContinue;
}

static void nfc_scene_read_on_enter_mf_desfire(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_mf_desfire, instance);
}

static void nfc_scene_read_menu_on_enter_mf_desfire(NfcApp* instance) {
    UNUSED(instance);
}

static void nfc_scene_read_success_on_enter_mf_desfire(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfDesfireData* data = nfc_device_get_data(device, NfcProtocolMfDesfire);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_mf_desfire_info(data, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static void nfc_scene_saved_menu_on_enter_mf_desfire(NfcApp* instance) {
    UNUSED(instance);
}

static bool nfc_scene_info_on_event_mf_desfire(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfDesfireData);
        return true;
    }

    return false;
}

static bool nfc_scene_read_menu_on_event_mf_desfire(NfcApp* instance, uint32_t event) {
    if(event == SubmenuIndexCommonEmulate) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaEmulate);
        return true;
    }

    return false;
}

static bool nfc_scene_saved_menu_on_event_mf_desfire(NfcApp* instance, uint32_t event) {
    if(event == SubmenuIndexCommonEmulate) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaEmulate);
        return true;
    }

    return false;
}

const NfcProtocolSupportBase nfc_protocol_support_mf_desfire = {
    .features = NfcProtocolFeatureEmulateUid,

    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_mf_desfire,
            .on_event = nfc_scene_info_on_event_mf_desfire,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_mf_desfire,
            .on_event = NULL,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_scene_read_menu_on_enter_mf_desfire,
            .on_event = nfc_scene_read_menu_on_event_mf_desfire,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_mf_desfire,
            .on_event = NULL,
        },
    .scene_saved_menu =
        {
            .on_enter = nfc_scene_saved_menu_on_enter_mf_desfire,
            .on_event = nfc_scene_saved_menu_on_event_mf_desfire,
        },
};
