#include "PresentationRemote.h"
#include <mbed.h>

using namespace myokbd;

events::EventQueue PresentationRemote::_event_queue(20 * EVENTS_EVENT_SIZE);
