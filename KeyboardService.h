/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 * Copyright (c) 2015 ARM Limited
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 *
 * Some data structures here are ported from the mbed Microcontroller Library,
 * also licensed under Apache license 2.0
 * ---------
 *
 *  TODO(lc525): add function to KeyboardService for sending arbitrary multi-key
 *  combinations (i.e CTRL+SPACE, ALT+TAB, etc)
 */
#ifndef _BT_KBD_SERVICE_H_
#define _BT_KBD_SERVICE_H_

#define BLE_ROLE_BROADCASTER 1
#define BLE_ROLE_PERIPHERAL 1
#define BLE_UUID_DESCRIPTOR_REPORT_REFERENCE 0x2908

#include "KeyBuffer.h"
#include "USB_HID.h"

#include <stdint.h>
#include <mbed.h>

#include "ble/BLE.h"
#include "ble/GattCharacteristic.h"

namespace btsvc {

  template<uint32_t KEYBUFFER_SIZE>
  class KeyboardService : public mbed::Stream {
    public:
      const static uint16_t UUID = GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE;

      KeyboardService(BLEDevice &ble, uint8_t reportTickerDelay=80);

      /* GattAttribute::Handle_t getValueHandle() const
       * {
       *     //return _ledState.getValueHandle();
       * } */

    private:
      GattAttribute** getInputReportDescriptors();
      GattAttribute** getOutputReportDescriptors();
      GattAttribute** getFeatureReportDescriptors();

      void startReportTicker();
      void stopReportTicker();
      void onDataSent(unsigned count);
      ble_error_t send(const Report_t report);
      ble_error_t sendAllKeysUp();
      ble_error_t sendKeyDown(uint8_t key, uint8_t modifier);

    public:
      void connect(const ble::ConnectionCompleteEvent &event);
      void disconnect(const ble::DisconnectionCompleteEvent &event);
      bool isConnected();
      void sendCallback();
      // Stream implementation
      virtual int _putc(int c);
      virtual int _getc();

    private:
      BLEDevice &_ble;
      bool _connected;
      unsigned long _failed_reports;

      mReport_t _input_report;
      uint8_t _input_report_len;
      ReportRef_t _input_report_ref_data;
      GattAttribute _input_report_ref_desc;
      GattCharacteristic _input_report_charc;

      Report_t _output_report;
      uint8_t _output_report_len;
      ReportRef_t _output_report_ref_data;
      GattAttribute _output_report_ref_desc;
      GattCharacteristic _output_report_charc;

      Report_t _feature_report;
      uint8_t _feature_report_len;
      ReportRef_t _feature_report_ref_data;
      GattAttribute _feature_report_ref_desc;
      GattCharacteristic _feature_report_charc;

      GattCharacteristic _report_map_charc;
      ReadOnlyGattCharacteristic<HIDInformation_t> _hid_info_charc;

      uint8_t _hid_ctl_point_cmd;
      GattCharacteristic _hid_ctl_point_charc;

      uint8_t _hid_protocol_mode;
      ReadOnlyGattCharacteristic<uint8_t> _protocol_mode_charc;

      mbed::Ticker _report_ticker;
      uint32_t _report_ticker_delay;
      bool _report_ticker_active;

      btutil::KeyBuffer<KEYBUFFER_SIZE> _keybuf;

  };

}

#include "KeyboardService.hpp"

#endif
