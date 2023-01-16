#pragma once

#include <furi.h>
#include <input/input.h>
#include <stdio.h>

#include "flizzer_tracker.h"
#include "sound_engine/sound_engine_defs.h"
#include "tracker_engine/tracker_engine_defs.h"
#include "util.h"

#include "input/pattern.h"
#include "input/sequence.h"
#include "input/songinfo.h"
#include "input/instrument.h"
#include "input/instrument_program.h"

void process_input_event(FlizzerTrackerApp *tracker, FlizzerTrackerEvent *event);