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
#include "PresentationController.h"

#include <mbed.h>
#include <ble/BLE.h>
#include <ble/gap/Gap.h>

using namespace mbed;
using namespace myokbd;


void setup() {
  //Serial.begin(115200);
  BLEDevice &ble = BLEDevice::Instance();

  PresentationRemote pr(ble);
  pr.start();
  PresentationController(&pr, analogPinToPinName(A0));
}

void loop() {
// execution path never reaches here; control passed to mbed event loop in
// PresentationRemote
}
