/* ---------
 * Copyright 2020 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 * Copyright (c) 2015 ARM Limited
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 *
 * Some data structures here are ported from the mbed Microcontroller Library,
 * also licensed under Apache license 2.0
 * ---------
 */
#ifndef _KEYBOARD_CONFIG_H_
#define _KEYBOARD_CONFIG_H_

#include "USB_HID.h"

namespace btsvc {

  class KbdConfig {
    public:
      static ReportMap_t  ReportMapDescriptor;
      static uint8_t ReportMapLen;

      static HIDInformation_t HIDInfo;

      static uint8_t InputReportData[];
      static const uint8_t EmptyInputReportData[];
      static const uint8_t InputReportLen;

      static uint8_t OutputReportData[];
      static const uint8_t OutputReportLen;
  };

}

#endif

