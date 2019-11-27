/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 * ---------
 */
#ifndef _BT_KBD_SERVICE_H_
#define _BT_KBD_SERVICE_H_

#include "ble/BLE.h"

namespace btsvc {
  class KeyboardService {
    public:
      const static uint16_t UUID = 0xA000;
      const static uint16_t LED_STATE_CHARACTERISTIC_UUID = 0xA001;
      bool initialAttribute = true;

      KeyboardService(BLEDevice &ble):
        _ble(ble),
        _ledState(LED_STATE_CHARACTERISTIC_UUID, &initialAttribute)
      {

        static bool serviceAdded = false;
        if(serviceAdded) {
          return;
        }
        GattCharacteristic *charTable[] = {&_ledState};
        GattService keyboardService(UUID,
                                    charTable,
                                    sizeof(charTable) / sizeof(GattCharacteristic *));
        _ble.addService(keyboardService);
        serviceAdded = true;

      }

      GattAttribute::Handle_t getValueHandle() const
      {
          return _ledState.getValueHandle();
      }

    private:
      BLEDevice &_ble;
      ReadWriteGattCharacteristic<bool> _ledState;
  };
}

#endif
