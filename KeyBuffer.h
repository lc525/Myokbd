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
#ifndef _KEYBUFFER_H_
#define _KEYBUFFER_H_

#include <stdint.h>
#include <mbed.h>
#include <CircularBuffer.h>

namespace btutil {

/**
 * @class KeyBuffer
 * Ported with some code refactoring from mbed example project
 *
 * Buffer used to store keys to send.
 * A circular buffer with the added capability of putting the last char back in,
 * when we're unable to send it (ie. when BLE stack is busy)
 */
template<uint32_t BUFFER_SIZE>
class KeyBuffer: public mbed::CircularBuffer<uint8_t, BUFFER_SIZE> {
public:
    KeyBuffer() :
        _data_is_pending (false),
        _keyUp_is_pending (false){}

    /** Mark a character as pending. When a freshly popped character cannot be
     * sent, because the underlying stack is busy, we set it as pending, and it
     * will get popped in priority by @ref getPending once reports can be sent
     * again.
     *
     * @param data  The character to send in priority. The second keyUp report
     * is implied.
     */
    void setPending(uint8_t data) {
      MBED_ASSERT(_data_is_pending == false);

      _data_is_pending = true;
      _pending_data = data;
      _keyUp_is_pending = true;
    }

    /** Get pending char. Either from the high priority buffer (set with
     * setPending), or from the circular buffer.
     *
     * @param   data Filled with the pending data, when present
     * @return  true if data was filled
     */
    bool getPending(uint8_t &data){
      if(_data_is_pending) {
        data = _pending_data;
        _data_is_pending = false;
        return true;
      }

      return mbed::CircularBuffer<uint8_t, BUFFER_SIZE>::pop(data);
    }

    bool isSomethingPending(void){
      return _data_is_pending ||
             _keyUp_is_pending ||
             !mbed::CircularBuffer<uint8_t, BUFFER_SIZE>::empty();
    }

    /** Signal that a keyUp report is pending. This means that a character has
     * successfully been sent, but the subsequent keyUp report failed. This
     * report is of highest priority than the next character.
     */
    void setKeyUpPending(void){ _keyUp_is_pending = true; }

    /** Signal that no high-priority report is pending anymore, we can go back
     * to the normal queue.
     */
    void clearKeyUpPending(void){ _keyUp_is_pending = false; }

    bool isKeyUpPending(void) { return _keyUp_is_pending; }

protected:
    bool _data_is_pending;
    uint8_t _pending_data;
    bool _keyUp_is_pending;
};

}

#endif
