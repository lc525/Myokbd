/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 * ---------
 */
#include "KeyboardService.h"
#include "KeyboardConfig.h"
#include "Keyboard_types.h"

#include "KeyBuffer.h"
#include "USB_HID.h"
#include <type_traits>

#include <ble/Gap.h>

using namespace btsvc;
using namespace btutil;

// KeyboardService constructor
template<uint32_t BUFFER_SIZE>
KeyboardService<BUFFER_SIZE>::KeyboardService(BLEDevice &ble,
                                              uint8_t reportTickerDelay) :
  _ble(ble),
  _connected(false),
  _failed_reports(0),
  _report_ticker_active(false),
  _report_ticker_delay(reportTickerDelay),

  // define report containing pressed key data
  _input_report(KbdConfig::InputReportData),
  _input_report_len(KbdConfig::InputReportLen),
  _input_report_ref_desc(BLE_UUID_DESCRIPTOR_REPORT_REFERENCE,
      (uint8_t *)&_input_report_ref_data, 2, 2),
  _input_report_charc(GattCharacteristic::UUID_REPORT_CHAR,
      (uint8_t *)_input_report, _input_report_len, _input_report_len,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
      getInputReportDescriptors(), 1),

  // define report containing feedback data (LED triggers such as CAPS LOCK,
  // NUM_LOCK, etc)
  _output_report(KbdConfig::OutputReportData),
  _output_report_len(KbdConfig::OutputReportLen),
  _output_report_ref_desc(BLE_UUID_DESCRIPTOR_REPORT_REFERENCE,
      (uint8_t *)&_output_report_ref_data, 2, 2),
  _output_report_charc(GattCharacteristic::UUID_REPORT_CHAR,
      (uint8_t *)_output_report, _output_report_len, _output_report_len,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE |
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
      getOutputReportDescriptors(), 1),

  //TODO(lc525): add feature reports as needed
  _feature_report(NULL),
  _feature_report_len(0),
  _feature_report_ref_desc(BLE_UUID_DESCRIPTOR_REPORT_REFERENCE,
      (uint8_t *)&_feature_report_ref_data, 2, 2),
  _feature_report_charc(GattCharacteristic::UUID_REPORT_CHAR,
      (uint8_t *)_feature_report, _feature_report_len, _feature_report_len,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
      getFeatureReportDescriptors(), 1),

  // define keyboard report map
  _report_map_charc(
      GattCharacteristic::UUID_REPORT_MAP_CHAR,
      const_cast<uint8_t*>(KbdConfig::ReportMapDescriptor),
      KbdConfig::ReportMapLen, KbdConfig::ReportMapLen,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
  ),

  // define HID info characteristic (see HIDInformation_t for data)
  _hid_info_charc(
      GattCharacteristic::UUID_HID_INFORMATION_CHAR,
      &KbdConfig::HIDInfo
  ),

  // define HID control point characteristic (suspend/resume)
  _hid_ctl_point_cmd(0),
  _hid_ctl_point_charc(
      GattCharacteristic::UUID_HID_CONTROL_POINT_CHAR,
      &_hid_ctl_point_cmd, 1, 1,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE
  ),

  // define protocol mode for the HID Service (Boot Protocol or Report Protocol)
  _hid_protocol_mode(REPORT_PROTOCOL),
  _protocol_mode_charc(
      GattCharacteristic::UUID_PROTOCOL_MODE_CHAR,
      &_hid_protocol_mode,
      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE
  )
{
        static bool serviceAdded = false;
        if(serviceAdded) {
          return;
        }

        GattCharacteristic *cTable[] = {
                                         &_hid_info_charc,
                                         &_report_map_charc,
                                         &_protocol_mode_charc,
                                         &_hid_ctl_point_charc,
                                         &_input_report_charc,
                                         &_output_report_charc,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL // nulls reserved for future use
                                       };
        uint8_t cTable_idx = 6;

        // for when the feature report is implemented
        if(_feature_report_len)
          cTable[cTable_idx++] = &_feature_report_charc;

        GattService keyboardService(
            UUID,
            cTable,
            cTable_idx
        );
        _ble.addService(keyboardService);
        _ble.gattServer().onDataSent(this, &KeyboardService::onDataSent);
        serviceAdded = true;
}


template<uint32_t BUFFER_SIZE>
GattAttribute** KeyboardService<BUFFER_SIZE>::getInputReportDescriptors() {
  _input_report_ref_data.ID = 0;
  _input_report_ref_data.type = INPUT_REPORT;

  static GattAttribute* descs[] = {
    &_input_report_ref_desc,
  };

  return descs;
}

template<uint32_t BUFFER_SIZE>
GattAttribute** KeyboardService<BUFFER_SIZE>::getOutputReportDescriptors() {
  _output_report_ref_data.ID = 0;
  _output_report_ref_data.type = OUTPUT_REPORT;

  static GattAttribute* descs[] = {
    &_output_report_ref_desc,
  };

  return descs;
}

template<uint32_t BUFFER_SIZE>
GattAttribute** KeyboardService<BUFFER_SIZE>::getFeatureReportDescriptors() {
  _feature_report_ref_data.ID = 0;
  _feature_report_ref_data.type = FEATURE_REPORT;

  static GattAttribute* descs[] = {
    &_feature_report_ref_desc,
  };

  return descs;
}

template<uint32_t BUFFER_SIZE>
void KeyboardService<BUFFER_SIZE>::startReportTicker() {
  if(_report_ticker_active)
    return;
  _report_ticker.attach_us(this, &KeyboardService::sendCallback, _report_ticker_delay * 1000);
  _report_ticker_active = true;
}

template<uint32_t BUFFER_SIZE>
void KeyboardService<BUFFER_SIZE>::stopReportTicker() {
  _report_ticker.detach();
  _report_ticker_active = false;
}

template<uint32_t BUFFER_SIZE>
void KeyboardService<BUFFER_SIZE>::onDataSent(unsigned count) {
  if(!_report_ticker_active && _keybuf.isSomethingPending())
    startReportTicker();
}

/**
 * Sends raw report; Should only be called from sendCallback
 */
template<uint32_t BUFFER_SIZE>
ble_error_t KeyboardService<BUFFER_SIZE>::send(const Report_t report) {
  static unsigned int consecutiveFailures = 0;
  ble_error_t ret = _ble.gattServer().write(_input_report_charc.getValueHandle(),
                                            report,
                                            _input_report_len);

  /*
   * Wait until a buffer is available (onDataSent)
   * TODO. This won't work, because BUSY error is not only returned when we're
   * short of notification buffers, but in other cases as well (e.g. when
   * disconnected). We need to find a reliable way of knowing when we actually
   * need to wait for onDataSent to be called.
   * if (ret == BLE_STACK_BUSY)
   *   stopReportTicker();
   */
  if (ret == BLE_STACK_BUSY)
      consecutiveFailures++;
  else
      consecutiveFailures = 0;

  if (consecutiveFailures > 20) {
      /*
       * We're not transmitting anything anymore. Might as well avoid
       * overloading the system in case it can magically fix itself. Ticker
       * will start again on next _putc call, or on next connection.
       */
      stopReportTicker();
      consecutiveFailures = 0;
  }

  return ret;
}

template<uint32_t BUFFER_SIZE>
ble_error_t KeyboardService<BUFFER_SIZE>::sendAllKeysUp() {
  return send(KbdConfig::EmptyInputReportData);
}

template<uint32_t BUFFER_SIZE>
ble_error_t KeyboardService<BUFFER_SIZE>::sendKeyDown(uint8_t key, uint8_t modifier) {
  _input_report[0] = modifier;
  _input_report[2] = keymap[key].usage;

  return send(_input_report);
}

template<uint32_t BUFFER_SIZE>
void KeyboardService<BUFFER_SIZE>::connect(const ble::ConnectionCompleteEvent &event) {
  this->_connected = true;
  if(!_report_ticker_active && _keybuf.isSomethingPending())
    startReportTicker();
}

template<uint32_t BUFFER_SIZE>
void KeyboardService<BUFFER_SIZE>::disconnect(const ble::DisconnectionCompleteEvent &event) {
  stopReportTicker();
  this->_connected = false;
}

template<uint32_t BUFFER_SIZE>
bool KeyboardService<BUFFER_SIZE>::isConnected() {
  return _connected;
}

/**
  * Pop a key from the internal FIFO, and attempt to send it over BLE
  *
  * keyUp reports should theoretically be sent after every keyDown, but we optimize the
  * throughput by only sending one when strictly necessary:
  * - when we need to repeat the same key
  * - when there is no more key to report
  *
  * In case of error, put the key event back in the buffer, and retry on next tick.
  */
template<uint32_t BUFFER_SIZE>
void KeyboardService<BUFFER_SIZE>::sendCallback(void) {
    ble_error_t ret;
    uint8_t c;
    static uint8_t previousKey = 0;

    if (_keybuf.isSomethingPending() && !_keybuf.isKeyUpPending()) {
        bool hasData = _keybuf.getPending(c);

        /*
          * If something is pending and is not a keyUp, getPending *must* return something. The
          * following is only a sanity check.
          */
        MBED_ASSERT(hasData);

        if (!hasData)
            return;

        if (previousKey == c) {
            /*
              * When the same key needs to be sent twice, we need to interleave a keyUp report,
              * or else the OS won't be able to differentiate them.
              * Push the key back into the buffer, and continue to keyUpCode.
              */
            _keybuf.setPending(c);
        } else {
            ret = sendKeyDown(c, keymap[c].modifier);
            if (ret) {
                _keybuf.setPending(c);
                _failed_reports++;
            } else {
                previousKey = c;
            }

            return;
        }
    }

    ret = sendAllKeysUp();
    if (ret) {
        _keybuf.setKeyUpPending();
        _failed_reports++;
    } else {
        _keybuf.clearKeyUpPending();
        previousKey = 0;

        /* Idle when there is nothing more to send */
        if (!_keybuf.isSomethingPending())
            stopReportTicker();
    }
}

// Stream implementation
template<uint32_t BUFFER_SIZE>
int KeyboardService<BUFFER_SIZE>::_putc(int c){
  if (_keybuf.full()) {
    return ENOMEM;
  }

  _keybuf.push((unsigned char)c);

  if(!_report_ticker_active)
    startReportTicker();

  return 0;
}

template<uint32_t BUFFER_SIZE>
int KeyboardService<BUFFER_SIZE>::_getc(){
  return 0;
}
