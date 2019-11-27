/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 * ---------
 *
 * Presentation remote implemented as a bluetooth HID keyboard service that
 * sends commands such as "advance slide", "previous slide" and "blank screen"
 * to a connected computer.
 *
 * The events triggering
 * presentation remote commands are configurable, and in the demo the outputs
 * from a sensor measuring muscle contractions (MyoWare) are used.
 *
 */
#ifndef _MYOKBD_PRESENTATION_REMOTE_H_
#define _MYOKBD_PRESENTATION_REMOTE_H_

#include <ble/Gap.h>
/* #include <ble/gap/AdvertisingDataTypes.h>
 * #include <ble/gap/Events.h> */
#include <ble/services/BatteryService.h>
#include <ble/services/DeviceInformationService.h>

#include "config.h"
#include "KeyboardService.h"

namespace myokbd {

  class PresentationRemote : public ble::Gap::EventHandler {
  public:
    PresentationRemote(BLEDevice &ble, events::EventQueue &event_queue,
                       const char dev_name[] = MYOKBD_BT_DEVICE_NAME ) :
      _ble(ble),
      _event_queue(event_queue),
      _dev_name(dev_name),
      _uuid_list { btsvc::KeyboardService::UUID,
                   GattService::UUID_DEVICE_INFORMATION_SERVICE,
                   GattService::UUID_BATTERY_SERVICE
                   },
      _init_done_led(digitalPinToPinName(LED_PWR), 1),
      _err_led(LED1, 0),
      _data_src(analogPinToPinName(A0)),
      _bt_kbd_svc(NULL),
      _bt_devinfo_svc(NULL),
      _bt_batt_svc(NULL),
      _adv_data_builder(_adv_buffer) { }

    ~PresentationRemote() {
      delete _bt_kbd_svc;
    }

    void start() {
      bool enableBonding = true;
      bool enableMITMProtection = false;
      _ble.gap().setEventHandler(this);

      _ble.init(this, &PresentationRemote::onInitComplete);
      _ble.securityManager().init(enableBonding, enableMITMProtection, SecurityManager::IO_CAPS_NONE);

      //TODO(lc525): remove once MyoSecurityEventHandler is implemented
      _ble.securityManager().setPairingRequestAuthorisation(false);

      _event_queue.call_every(500, this, &PresentationRemote::onTick);

      _event_queue.dispatch_forever();
    }

  private:
    void onInitComplete(BLEDevice::InitializationCompleteCallbackContext *params) {
      if(params->error != BLE_ERROR_NONE) {
        return;
      }

      _bt_devinfo_svc = new DeviceInformationService(_ble,
                                                     MANUFACTURER_NAME,
                                                     MODEL_NUMBER,
                                                     SERIAL_NUMBER,
                                                     HW_REV,
                                                     FW_REV,
                                                     SW_REV);
      _bt_batt_svc = new BatteryService(_ble);
      _bt_kbd_svc = new btsvc::KeyboardService(_ble);

      startAdvertising();
    }

    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) override {
      _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    }

    void onTick(void) {
      //this is run in interrupt context, do most of the work on the main thread
      //instead
      _init_done_led = !_init_done_led;
    }

    void onPasskeyDisplay(Gap::Handle_t handle, const SecurityManager::Passkey_t passkey) {
    }

    void onSecuritySetupComplete(Gap::Handle_t handle,
                                    SecurityManager::SecurityCompletionStatus_t status) {
    }

    void startAdvertising() {
      ble::AdvertisingParameters adv_param(
          ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
          ble::adv_interval_t(ble::millisecond_t(1000))
      );

      _adv_data_builder.setFlags();
      _adv_data_builder.setLocalServiceList(
          mbed::make_Span(_uuid_list, 3)
      );
      _adv_data_builder.setName(_dev_name);
      _adv_data_builder.setAppearance(ble::adv_data_appearance_t::KEYBOARD);

      ble_error_t error = _ble.gap().setAdvertisingParameters(
          ble::LEGACY_ADVERTISING_HANDLE,
          adv_param
      );

      if(error) {
        return;
      }

      error = _ble.gap().setAdvertisingPayload(
          ble::LEGACY_ADVERTISING_HANDLE,
          _adv_data_builder.getAdvertisingData()
      );

      if(error) {
        return;
      }

      error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

      if(error) {
        return;
      }
    }

  private:
    BLEDevice &_ble;
    events::EventQueue &_event_queue;

    const char* _dev_name;

    mbed::DigitalOut _init_done_led;
    mbed::DigitalOut _err_led;
    mbed::AnalogIn _data_src;

    btsvc::KeyboardService *_bt_kbd_svc;
    DeviceInformationService *_bt_devinfo_svc;
    BatteryService *_bt_batt_svc;
    UUID _uuid_list[3];

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
  };

} /** myokbd namespace end **/

#endif /* _MYOKBD_PRESENTATION_REMOTE_H_ */
