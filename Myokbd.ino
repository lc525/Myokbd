/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 * ---------
 *
 * Myokbd
 *
 * An Mbed OS based bluetooth pheripheral (keyboard) sending commands based
 * on EMG signals.
 *
 * Demo Application: Presentation Remote
 *
 * Tested targets:
 *  - Arduino Nano 33 BLE
 *
 * This makes use of the Mbed OS bluetooth stack rather than the ArduinoBLE
 * library because of the reduced functionality of the latter.
 *
 * Please first follow the instructions in config.h in order to set up
 * parameters in accordance to your usecases
 */

#include "config.h"
#include "PresentationRemote.h"

#include <mbed.h>
#include <ble/BLE.h>
#include <ble/gap/Gap.h>
#include <ble/services/DeviceInformationService.h>

using namespace mbed;
using namespace myokbd;

static events::EventQueue event_queue(10 * EVENTS_EVENT_SIZE);

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

void setup() {
  BLEDevice &ble = BLEDevice::Instance();
  ble.onEventsToProcess(schedule_ble_events);

  PresentationRemote pr(ble, event_queue, MYOKBD_BT_DEVICE_NAME);
  pr.start();
}

void loop() {
  // no need for any code here as we use event-based dispatching
}
