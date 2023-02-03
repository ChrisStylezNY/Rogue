#include "flizzer_tracker.h"
#include "diskop.h"
#include "init_deinit.h"
#include "input_event.h"
#include "util.h"
#include "view/instrument_editor.h"
#include "view/pattern_editor.h"

#include "font.h"
#include <flizzer_tracker_icons.h>

void draw_callback(Canvas *canvas, void *ctx)
{
    TrackerViewModel *model = (TrackerViewModel *)ctx;
    FlizzerTrackerApp *tracker = (FlizzerTrackerApp *)(model->tracker);

    canvas_set_color(canvas, ColorXOR);

    if (tracker->is_loading)
    {
        canvas_draw_str(canvas, 10, 10, "Loading...");
        return;
    }

    if (tracker->is_saving)
    {
        canvas_draw_str(canvas, 10, 10, "Saving...");
        return;
    }

    canvas_set_custom_u8g2_font(canvas, u8g2_font_tom_thumb_4x6_tr);

    switch (tracker->mode)
    {
        case PATTERN_VIEW:
        {
            if (tracker->tracker_engine.song == NULL)
            {
                stop();
                tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
            }

            draw_songinfo_view(canvas, tracker);
            draw_sequence_view(canvas, tracker);
            draw_pattern_view(canvas, tracker);
            break;
        }

        case INST_EDITOR_VIEW:
        {
            draw_instrument_view(canvas, tracker);
            draw_instrument_program_view(canvas, tracker);
            break;
        }

        default:
            break;
    }
}

bool input_callback(InputEvent *input_event, void *ctx)
{
    // Проверяем, что контекст не нулевой
    furi_assert(ctx);
    TrackerView *tracker_view = (TrackerView *)ctx;
    FlizzerTrackerApp *tracker = (FlizzerTrackerApp *)(tracker_view->context);

    bool consumed = false;

    if (input_event->key == InputKeyBack && input_event->type == InputTypeShort)
    {
        tracker->period = furi_get_tick() - tracker->current_time;
        tracker->current_time = furi_get_tick();

        tracker->was_it_back_keypress = true;
    }

    else if (input_event->type == InputTypeShort || input_event->type == InputTypeLong)
    {
        tracker->was_it_back_keypress = false;
        tracker->period = 0;
    }

    uint32_t final_period = (tracker->was_it_back_keypress ? tracker->period : 0);

    FlizzerTrackerEvent event = {.type = EventTypeInput, .input = *input_event, .period = final_period};

    if (!(tracker->is_loading) && !(tracker->is_saving))
    {
        furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
    }

    consumed = true;
    return consumed;
}

int32_t flizzer_tracker_app(void *p)
{
    UNUSED(p);

    Storage *storage = furi_record_open(RECORD_STORAGE);
    bool st = storage_simply_mkdir(storage, FLIZZER_TRACKER_FOLDER);
    UNUSED(st);
    furi_record_close(RECORD_STORAGE);

    FlizzerTrackerApp *tracker = init_tracker(44100, 50, true, 1024);

    // Текущее событие типа кастомного типа FlizzerTrackerEvent
    FlizzerTrackerEvent event;

    tracker->tracker_engine.master_volume = 0x80;

    tracker->song.speed = 5;
    tracker->song.rate = tracker->tracker_engine.rate;
    tracker->song.num_instruments = 2;
    tracker->song.num_patterns = 3;
    tracker->song.num_sequence_steps = 4;
    tracker->song.pattern_length = 64;

    tracker->song.sequence.sequence_step[0].pattern_indices[0] = 0;
    tracker->song.sequence.sequence_step[0].pattern_indices[1] = 1;
    tracker->song.sequence.sequence_step[0].pattern_indices[2] = 2;
    tracker->song.sequence.sequence_step[0].pattern_indices[3] = 2;

    tracker->song.pattern[0].step = malloc(64 * sizeof(TrackerSongPatternStep));
    tracker->song.pattern[1].step = malloc(64 * sizeof(TrackerSongPatternStep));
    tracker->song.pattern[2].step = malloc(64 * sizeof(TrackerSongPatternStep));

    memset(tracker->song.pattern[0].step, 0, 64 * sizeof(TrackerSongPatternStep));
    memset(tracker->song.pattern[1].step, 0, 64 * sizeof(TrackerSongPatternStep));
    memset(tracker->song.pattern[2].step, 0, 64 * sizeof(TrackerSongPatternStep));

    tracker->song.instrument[0] = malloc(sizeof(Instrument));
    tracker->song.instrument[1] = malloc(sizeof(Instrument));

    for (int i = 0; i < 64; ++i)
    {
        set_note(&tracker->song.pattern[0].step[i], MUS_NOTE_NONE);
        set_note(&tracker->song.pattern[1].step[i], MUS_NOTE_NONE);
        set_note(&tracker->song.pattern[2].step[i], MUS_NOTE_NONE);

        set_instrument(&tracker->song.pattern[0].step[i], MUS_NOTE_INSTRUMENT_NONE);
        set_instrument(&tracker->song.pattern[1].step[i], MUS_NOTE_INSTRUMENT_NONE);
        set_instrument(&tracker->song.pattern[2].step[i], MUS_NOTE_INSTRUMENT_NONE);

        set_volume(&tracker->song.pattern[0].step[i], MUS_NOTE_VOLUME_NONE);
        set_volume(&tracker->song.pattern[1].step[i], MUS_NOTE_VOLUME_NONE);
        set_volume(&tracker->song.pattern[2].step[i], MUS_NOTE_VOLUME_NONE);
    }

    for (int i = 0; i < 64; i += 8)
    {
        set_note(&tracker->song.pattern[0].step[0 + i], 12 * 5);
        set_note(&tracker->song.pattern[0].step[2 + i], 12 * 5 + 2);
        set_note(&tracker->song.pattern[0].step[4 + i], 12 * 5 - 2);
        set_note(&tracker->song.pattern[0].step[6 + i], 12 * 5 + 4);

        set_instrument(&tracker->song.pattern[0].step[0 + i], 0);
        set_instrument(&tracker->song.pattern[0].step[2 + i], 0);
        set_instrument(&tracker->song.pattern[0].step[4 + i], 0);
        set_instrument(&tracker->song.pattern[0].step[6 + i], 0);
    }

    for (int i = 0; i < 64; i++)
    {
        set_note(&tracker->song.pattern[1].step[i], 12 * 7 + 11);

        set_instrument(&tracker->song.pattern[1].step[i], 1);
    }

    set_default_instrument(tracker->song.instrument[0]);
    set_default_instrument(tracker->song.instrument[1]);

    tracker->song.instrument[0]->adsr.a = 0x2;
    tracker->song.instrument[0]->adsr.d = 0x9;
    tracker->song.instrument[0]->adsr.volume = 0x80;
    tracker->song.instrument[0]->waveform = SE_WAVEFORM_TRIANGLE;

    tracker->song.instrument[1]->adsr.a = 0x0;
    tracker->song.instrument[1]->adsr.d = 0x3;
    tracker->song.instrument[1]->adsr.volume = 0x18;
    tracker->song.instrument[1]->waveform = SE_WAVEFORM_NOISE;

    tracker->tracker_engine.playing = false;
    play();

    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);

    // Бесконечный цикл обработки очереди событий
    while (!(tracker->quit))
    {
        // Выбираем событие из очереди в переменную event (ждём бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(tracker->event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Наше событие — это нажатие кнопки
        if (event.type == EventTypeInput)
        {
            process_input_event(tracker, &event);
        }

        if (event.type == EventTypeSaveSong)
        {
            save_song(tracker, tracker->filepath);
        }

        if (event.type == EventTypeLoadSong)
        {
            stop_song(tracker);

            tracker->tracker_engine.sequence_position = tracker->tracker_engine.pattern_position = tracker->current_instrument = 0;

            tracker->dialogs = furi_record_open(RECORD_DIALOGS);
            tracker->is_loading = true;

            FuriString *path;
            path = furi_string_alloc();
            furi_string_set(path, FLIZZER_TRACKER_FOLDER);

            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(&browser_options, SONG_FILE_EXT, &I_flizzer_tracker_module);
            browser_options.base_path = FLIZZER_TRACKER_FOLDER;
            browser_options.hide_ext = false;

            bool ret = dialog_file_browser_show(tracker->dialogs, path, path, &browser_options);

            furi_record_close(RECORD_DIALOGS);

            const char *cpath = furi_string_get_cstr(path);

            if (ret && strcmp(&cpath[strlen(cpath) - 4], SONG_FILE_EXT) == 0)
            {
                bool result = load_song_util(tracker, path);
                UNUSED(result);
            }

            else
            {
                furi_string_free(path);
                tracker->is_loading = false;
            }
        }

        if (event.type == EventTypeSetAudioMode)
        {
            sound_engine_PWM_timer_init(tracker->external_audio);

            tracker->sound_engine.external_audio_output = tracker->external_audio;
        }
    }

    stop();

    save_config(tracker);

    deinit_tracker(tracker);

    return 0;
}