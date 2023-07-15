#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher_i.h>
#include <gui/view_port_i.h>
#include <gui/scene_manager.h>
#include <toolbox/sha256.h>
#include <notification/notification_messages.h>

#include "esubghz_chat_icons.h"

#include "crypto/gcm.h"

#define APPLICATION_NAME "ESubGhzChat"

#define DEFAULT_FREQ 433920000

#define KEY_BITS 256
#define IV_BYTES 12
#define TAG_BYTES 16

#define RX_TX_BUFFER_SIZE 1024

#define CHAT_BOX_STORE_SIZE 4096
#define TEXT_INPUT_STORE_SIZE 512

#define TICK_INTERVAL 50
#define MESSAGE_COMPLETION_TIMEOUT 200
#define TIMEOUT_BETWEEN_MESSAGES 500

#define KBD_UNLOCK_CNT 3
#define KBD_UNLOCK_TIMEOUT 1000

typedef struct {
	SceneManager *scene_manager;
	ViewDispatcher *view_dispatcher;
	NotificationApp *notification;
	TextBox *chat_box;
	FuriString *chat_box_store;
	TextInput *text_input;
	char text_input_store[TEXT_INPUT_STORE_SIZE + 1];
	FuriString *name_prefix;
	FuriString *msg_input;
	bool encrypted;
	uint32_t frequency;
	gcm_context gcm_ctx;
	uint8_t rx_buffer[RX_TX_BUFFER_SIZE];
	uint8_t tx_buffer[RX_TX_BUFFER_SIZE];
	char rx_str_buffer[RX_TX_BUFFER_SIZE + 1];
	FuriStreamBuffer *rx_collection_buffer;
	uint32_t last_time_rx_data;

	// for locking
	ViewPortDrawCallback orig_draw_cb;
	ViewPortInputCallback orig_input_cb;
	bool kbd_locked;
	uint32_t kbd_lock_msg_ticks;
	uint8_t kbd_lock_count;
} ESubGhzChatState;

typedef enum {
	ESubGhzChatScene_FreqInput,
	ESubGhzChatScene_PassInput,
	ESubGhzChatScene_ChatInput,
	ESubGhzChatScene_ChatBox,
	ESubGhzChatScene_MAX
} ESubGhzChatScene;

typedef enum {
	ESubGhzChatView_Input,
	ESubGhzChatView_ChatBox,
} ESubGhzChatView;

typedef enum {
	ESubGhzChatEvent_FreqEntered,
	ESubGhzChatEvent_PassEntered,
	ESubGhzChatEvent_MsgEntered
} ESubGhzChatEvent;

static void esubghz_chat_explicit_bzero(void *s, size_t len)
{
	memset(s, 0, len);
	asm volatile("" ::: "memory");
}

static void post_rx(ESubGhzChatState *state, size_t rx_size)
{
	furi_assert(state);

	if (rx_size == 0) {
		return;
	}

	furi_check(rx_size <= RX_TX_BUFFER_SIZE);

	if (!state->encrypted) {
		memcpy(state->rx_str_buffer, state->rx_buffer, rx_size);
		state->rx_str_buffer[rx_size] = 0;
	} else {
		if (rx_size < IV_BYTES + TAG_BYTES + 1) {
			return;
		}

		int ret = gcm_auth_decrypt(&(state->gcm_ctx),
				state->rx_buffer, IV_BYTES,
				NULL, 0,
				state->rx_buffer + IV_BYTES,
				(uint8_t *) state->rx_str_buffer,
				rx_size - (IV_BYTES + TAG_BYTES),
				state->rx_buffer + rx_size - TAG_BYTES,
				TAG_BYTES);
		state->rx_str_buffer[rx_size - (IV_BYTES + TAG_BYTES)] = 0;

		if (ret != 0) {
			strcpy(state->rx_str_buffer, "ERR: Decryption failed!");
		}
	}

	furi_string_cat_printf(state->chat_box_store, "\n%s",
			state->rx_str_buffer);

	notification_message(state->notification, &sequence_single_vibro);

	// Reset Text Box contents and focus
	text_box_set_text(state->chat_box,
			furi_string_get_cstr(state->chat_box_store));
	text_box_set_focus(state->chat_box, TextBoxFocusEnd);
}

static void freq_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	furi_string_cat_printf(state->chat_box_store, "Frequency: %lu",
			state->frequency);

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_FreqEntered);
}

static bool freq_input_validator(const char *text, FuriString *error,
		void *context)
{
	furi_assert(text);
	furi_assert(error);

	furi_assert(context);
	ESubGhzChatState* state = context;

        int ret = sscanf(text, "%lu", &(state->frequency));
	if (ret != 1) {
		furi_string_printf(error, "Please enter\nfrequency\nin Hz!");
		return false;
	}

	if (!furi_hal_subghz_is_frequency_valid(state->frequency)) {
		furi_string_printf(error, "Frequency\n%lu\n is invalid!",
				state->frequency);
		return false;
	}

	if (!furi_hal_subghz_is_tx_allowed(state->frequency)) {
		furi_string_printf(error, "TX forbidden\non frequency\n%lu!",
				state->frequency);
		return false;
	}

	return true;
}

static void pass_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	furi_string_cat_printf(state->chat_box_store, "\nEncrypted: %s",
			(state->encrypted ? "true" : "false"));

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_PassEntered);
}

static bool pass_input_validator(const char *text, FuriString *error,
		void *context)
{
	furi_assert(text);
	furi_assert(error);

	furi_assert(context);
	ESubGhzChatState* state = context;

	if (strlen(text) == 0) {
		state->encrypted = false;
		return true;
	}

	unsigned char key[KEY_BITS / 8];

	state->encrypted = true;
	sha256((unsigned char *) text, strlen(text), key);

	// TODO: remove this
	furi_string_cat_printf(state->chat_box_store, "\nKey:");
	int i;
	for (i = 0; i < KEY_BITS / 8; i++) {
		furi_string_cat_printf(state->chat_box_store, " %02x", key[i]);
	}

	int ret = gcm_setkey(&(state->gcm_ctx), key, KEY_BITS / 8);

	esubghz_chat_explicit_bzero(key, sizeof(key));

	if (ret != 0) {
		gcm_zero_ctx(&(state->gcm_ctx));
		furi_string_printf(error, "Failed to\nset key!");
		return false;
	}

	return true;
}

static void chat_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	if (strlen(state->text_input_store) == 0) {
		scene_manager_handle_custom_event(state->scene_manager,
				ESubGhzChatEvent_MsgEntered);
		return;
	}

	furi_string_set(state->msg_input, state->name_prefix);
	furi_string_cat_str(state->msg_input, state->text_input_store);

	furi_string_cat_printf(state->chat_box_store, "\n%s",
		furi_string_get_cstr(state->msg_input));

	size_t msg_len = strlen(furi_string_get_cstr(state->msg_input));
	size_t tx_size = msg_len;
	if (state->encrypted) {
		tx_size += IV_BYTES + TAG_BYTES;
		furi_check(tx_size <= sizeof(state->tx_buffer));

		furi_hal_random_fill_buf(state->tx_buffer, IV_BYTES);
		gcm_crypt_and_tag(&(state->gcm_ctx), ENCRYPT,
				state->tx_buffer, IV_BYTES,
				NULL, 0,
				(unsigned char *)
				furi_string_get_cstr(state->msg_input),
				state->tx_buffer + IV_BYTES,
				msg_len,
				state->tx_buffer + IV_BYTES + msg_len,
				TAG_BYTES);
	} else {
		furi_check(tx_size <= sizeof(state->tx_buffer));
		memcpy(state->tx_buffer,
				furi_string_get_cstr(state->msg_input),
				tx_size);
	}

	furi_string_set_char(state->msg_input, 0, 0);

	// TODO: remove this
	furi_string_cat_printf(state->chat_box_store, "\nTXed (HEX):");
	size_t i;
	for (i = 0; i < tx_size; i++) {
		furi_string_cat_printf(state->chat_box_store, " %02x",
				state->tx_buffer[i]);
	}

	// TODO: remove this
	state->last_time_rx_data = furi_get_tick();
	furi_stream_buffer_send(state->rx_collection_buffer,
			state->tx_buffer, tx_size, 0);

	// TODO: actually transmit

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_MsgEntered);
}

static void scene_on_enter_freq_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_freq_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	snprintf(state->text_input_store, TEXT_INPUT_STORE_SIZE, "%lu",
			(uint32_t) DEFAULT_FREQ);
	text_input_reset(state->text_input);
	text_input_set_result_callback(
			state->text_input,
			freq_input_cb,
			state,
			state->text_input_store,
			sizeof(state->text_input_store),
			true);
	text_input_set_validator(
			state->text_input,
			freq_input_validator,
			state);
	text_input_set_header_text(
			state->text_input,
			"Frequency");

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

static bool scene_on_event_freq_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_freq_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		case ESubGhzChatEvent_FreqEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_PassInput);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

static void scene_on_exit_freq_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_freq_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}

static void scene_on_enter_pass_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	state->text_input_store[0] = 0;
	text_input_reset(state->text_input);
	text_input_set_result_callback(
			state->text_input,
			pass_input_cb,
			state,
			state->text_input_store,
			sizeof(state->text_input_store),
			true);
	text_input_set_validator(
			state->text_input,
			pass_input_validator,
			state);
	text_input_set_header_text(
			state->text_input,
			"Password (empty for no encr.)");
	text_input_set_minimum_length(state->text_input, 0);

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

static bool scene_on_event_pass_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		case ESubGhzChatEvent_PassEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_ChatInput);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

static void scene_on_exit_pass_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}

static void scene_on_enter_chat_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_chat_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	state->text_input_store[0] = 0;
	text_input_reset(state->text_input);
	text_input_set_result_callback(
			state->text_input,
			chat_input_cb,
			state,
			state->text_input_store,
			sizeof(state->text_input_store),
			true);
	text_input_set_validator(
			state->text_input,
			NULL,
			NULL);
	text_input_set_header_text(
			state->text_input,
			"Message");
	text_input_set_minimum_length(state->text_input, 0);

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

static bool scene_on_event_chat_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		case ESubGhzChatEvent_MsgEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_ChatBox);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

static void scene_on_exit_chat_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}

static void scene_on_enter_chat_box(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_chat_box");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_box_reset(state->chat_box);
	text_box_set_text(state->chat_box,
			furi_string_get_cstr(state->chat_box_store));
	text_box_set_focus(state->chat_box, TextBoxFocusEnd);

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_ChatBox);
}

static bool scene_on_event_chat_box(void* context, SceneManagerEvent event)
{
	UNUSED(event);

	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_box");

	furi_assert(context);

	return false;
}

static void scene_on_exit_chat_box(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_box");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_box_reset(state->chat_box);
}

static void (*const esubghz_chat_scene_on_enter_handlers[])(void*) = {
	scene_on_enter_freq_input,
	scene_on_enter_pass_input,
	scene_on_enter_chat_input,
	scene_on_enter_chat_box
};

static bool (*const esubghz_chat_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
	scene_on_event_freq_input,
	scene_on_event_pass_input,
	scene_on_event_chat_input,
	scene_on_event_chat_box
};

static void (*const esubghz_chat_scene_on_exit_handlers[])(void*) = {
	scene_on_exit_freq_input,
	scene_on_exit_pass_input,
	scene_on_exit_chat_input,
	scene_on_exit_chat_box
};

static const SceneManagerHandlers esubghz_chat_scene_event_handlers = {
	.on_enter_handlers = esubghz_chat_scene_on_enter_handlers,
	.on_event_handlers = esubghz_chat_scene_on_event_handlers,
	.on_exit_handlers = esubghz_chat_scene_on_exit_handlers,
	.scene_num = ESubGhzChatScene_MAX};

static bool kbd_lock_msg_display(ESubGhzChatState *state)
{
	return (state->kbd_lock_msg_ticks != 0);
}

static bool kbd_lock_msg_reset_timeout(ESubGhzChatState *state)
{
	if (state->kbd_lock_msg_ticks == 0) {
		return false;
	}

	if (furi_get_tick() - state->kbd_lock_msg_ticks > KBD_UNLOCK_TIMEOUT) {
		return true;
	}

	return false;
}

static void kbd_lock_msg_reset(ESubGhzChatState *state, bool backlight_off)
{
	state->kbd_lock_msg_ticks = 0;
	state->kbd_lock_count = 0;

	if (backlight_off) {
		notification_message(state->notification,
				&sequence_display_backlight_off);
	}
}

static void kbd_lock(ESubGhzChatState *state)
{
	state->kbd_locked = true;
	kbd_lock_msg_reset(state, true);
}

static void kbd_unlock(ESubGhzChatState *state)
{
	state->kbd_locked = false;
	kbd_lock_msg_reset(state, false);
}

static bool esubghz_chat_custom_event_callback(void* context, uint32_t event)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_custom_event_callback");
	furi_assert(context);
	ESubGhzChatState* state = context;
	return scene_manager_handle_custom_event(state->scene_manager, event);
}

static bool esubghz_chat_navigation_event_callback(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_navigation_event_callback");
	furi_assert(context);
	ESubGhzChatState* state = context;
	return scene_manager_handle_back_event(state->scene_manager);
}

static void esubghz_chat_tick_event_callback(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_tick_event_callback");

	furi_assert(context);
	ESubGhzChatState* state = context;

	if (kbd_lock_msg_reset_timeout(state)) {
		kbd_lock_msg_reset(state, true);
	}

	size_t avail = furi_stream_buffer_bytes_available(
			state->rx_collection_buffer);
	if (avail > 0) {
		uint32_t since_last_rx = furi_get_tick() -
			state->last_time_rx_data;
		if (avail == RX_TX_BUFFER_SIZE || since_last_rx >
				MESSAGE_COMPLETION_TIMEOUT) {
			size_t rx_size = furi_stream_buffer_receive(
					state->rx_collection_buffer,
					state->rx_buffer,
					avail, 0);
			post_rx(state, rx_size);
			furi_stream_buffer_reset(state->rx_collection_buffer);
		}
	}

	scene_manager_handle_tick_event(state->scene_manager);
}

static void esubghz_hooked_draw_callback(Canvas* canvas, void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_hooked_draw_callback");

	furi_assert(context);
	ESubGhzChatState* state = context;

	state->orig_draw_cb(canvas, state->view_dispatcher);

	if (state->kbd_locked) {
		canvas_set_font(canvas, FontPrimary);
		elements_multiline_text_framed(canvas, 42, 30, "Locked");
	}

	if (kbd_lock_msg_display(state)) {
		canvas_set_font(canvas, FontSecondary);
		elements_bold_rounded_frame(canvas, 14, 8, 99, 48);
		elements_multiline_text(canvas, 65, 26, "To unlock\npress:");
		canvas_draw_icon(canvas, 65, 42, &I_Pin_back_arrow_10x8);
		canvas_draw_icon(canvas, 80, 42, &I_Pin_back_arrow_10x8);
		canvas_draw_icon(canvas, 95, 42, &I_Pin_back_arrow_10x8);
		canvas_draw_icon(canvas, 16, 13, &I_WarningDolphin_45x42);
	}
}

static void esubghz_hooked_input_callback(InputEvent* event, void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_hooked_input_callback");

	furi_assert(context);
	ESubGhzChatState* state = context;

	if (state->kbd_locked) {
		if (state->kbd_lock_count == 0) {
			state->kbd_lock_msg_ticks = furi_get_tick();
		}

		if (event->key == InputKeyBack && event->type ==
				InputTypeShort) {
			state->kbd_lock_count++;
		}

		if (state->kbd_lock_count >= KBD_UNLOCK_CNT) {
			kbd_unlock(state);
		}

		// do not handle the event
		return;
	}

	// if we are in the chat view, allow locking
	if (state->view_dispatcher->current_view ==
			text_box_get_view(state->chat_box)) {
		if (event->key == InputKeyOk) {
			if (event->type == InputTypeLong) {
				kbd_lock(state);
			}

			/* do not handle any Ok key events to prevent blocking
			 * of other keys */
			return;
		}
	}

	state->orig_input_cb(event, state->view_dispatcher);
}

static bool helper_strings_alloc(ESubGhzChatState *state)
{
	furi_assert(state);

	state->name_prefix = furi_string_alloc();
	if (state->name_prefix == NULL) {
		return false;
	}

	state->msg_input = furi_string_alloc();
	if (state->msg_input == NULL) {
		furi_string_free(state->name_prefix);
		return false;
	}

	return true;
}

static void helper_strings_free(ESubGhzChatState *state)
{
	furi_assert(state);

	furi_string_free(state->name_prefix);
	furi_string_free(state->msg_input);
}

static bool chat_box_alloc(ESubGhzChatState *state)
{
	furi_assert(state);

	state->chat_box = text_box_alloc();
	if (state->chat_box == NULL) {
		return false;
	}

	state->chat_box_store = furi_string_alloc();
	if (state->chat_box_store == NULL) {
		text_box_free(state->chat_box);
		return false;
	}

	furi_string_reserve(state->chat_box_store, CHAT_BOX_STORE_SIZE);
	furi_string_set_char(state->chat_box_store, 0, 0);
	text_box_set_text(state->chat_box,
			furi_string_get_cstr(state->chat_box_store));
	text_box_set_focus(state->chat_box, TextBoxFocusEnd);

	return true;
}

static void chat_box_free(ESubGhzChatState *state)
{
	furi_assert(state);

	text_box_free(state->chat_box);
	furi_string_free(state->chat_box_store);
}

int32_t esubghz_chat(void)
{
	gcm_initialize();

	int32_t err = -1;

	FURI_LOG_I(APPLICATION_NAME, "Starting...");

	ESubGhzChatState *state = malloc(sizeof(ESubGhzChatState));
	if (state == NULL) {
		goto err_alloc;
	}
	memset(state, 0, sizeof(*state));

	state->scene_manager = scene_manager_alloc(
			&esubghz_chat_scene_event_handlers, state);
	if (state->scene_manager == NULL) {
		goto err_alloc_sm;
	}

	state->view_dispatcher = view_dispatcher_alloc();
	if (state->view_dispatcher == NULL) {
		goto err_alloc_vd;
	}

	if (!helper_strings_alloc(state)) {
		goto err_alloc_hs;
	}

	state->text_input = text_input_alloc();
	if (state->text_input == NULL) {
		goto err_alloc_ti;
	}

	if (!chat_box_alloc(state)) {
		goto err_alloc_cb;
	}

	state->rx_collection_buffer = furi_stream_buffer_alloc(
			RX_TX_BUFFER_SIZE,
			RX_TX_BUFFER_SIZE);
	if (state->rx_collection_buffer == NULL) {
		goto err_alloc_rcb;
	}

	// set chat name prefix
	// TODO: handle escape chars here somehow
	furi_string_printf(state->name_prefix, "\033[0;33m%s\033[0m: ",
			furi_hal_version_get_name_ptr());

	/* no error handling here, don't know how */
	state->notification = furi_record_open(RECORD_NOTIFICATION);

	state->orig_draw_cb = state->view_dispatcher->view_port->draw_callback;
	state->orig_input_cb = state->view_dispatcher->view_port->input_callback;
	view_port_draw_callback_set(state->view_dispatcher->view_port,
			esubghz_hooked_draw_callback, state);
	view_port_input_callback_set(state->view_dispatcher->view_port,
			esubghz_hooked_input_callback, state);

	view_dispatcher_enable_queue(state->view_dispatcher);

	view_dispatcher_set_event_callback_context(state->view_dispatcher, state);
	view_dispatcher_set_custom_event_callback(
			state->view_dispatcher,
			esubghz_chat_custom_event_callback);
	view_dispatcher_set_navigation_event_callback(
			state->view_dispatcher,
			esubghz_chat_navigation_event_callback);
	view_dispatcher_set_tick_event_callback(
			state->view_dispatcher,
			esubghz_chat_tick_event_callback,
			TICK_INTERVAL);

	view_dispatcher_add_view(state->view_dispatcher, ESubGhzChatView_Input,
			text_input_get_view(state->text_input));
	view_dispatcher_add_view(state->view_dispatcher, ESubGhzChatView_ChatBox,
			text_box_get_view(state->chat_box));

	/* no error handling here, don't know how */
	Gui *gui = furi_record_open(RECORD_GUI);
	view_dispatcher_attach_to_gui(state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

	scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_FreqInput);
	view_dispatcher_run(state->view_dispatcher);

	err = 0;

	furi_record_close(RECORD_GUI);
	view_dispatcher_remove_view(state->view_dispatcher, ESubGhzChatView_Input);
	view_dispatcher_remove_view(state->view_dispatcher, ESubGhzChatView_ChatBox);

	furi_record_close(RECORD_NOTIFICATION);

	// clear the key and potential password
	esubghz_chat_explicit_bzero(state->text_input_store,
			sizeof(state->text_input_store));
	gcm_zero_ctx(&(state->gcm_ctx));

	furi_stream_buffer_free(state->rx_collection_buffer);

err_alloc_rcb:
	chat_box_free(state);

err_alloc_cb:
	text_input_free(state->text_input);

err_alloc_ti:
	helper_strings_free(state);

err_alloc_hs:
	view_dispatcher_free(state->view_dispatcher);

err_alloc_vd:
	scene_manager_free(state->scene_manager);

err_alloc_sm:
	free(state);

err_alloc:
	if (err != 0) {
		FURI_LOG_E(APPLICATION_NAME, "Failed to launch (alloc error)!");
	} else {
		FURI_LOG_I(APPLICATION_NAME, "Clean exit.");
	}

	return err;
}
