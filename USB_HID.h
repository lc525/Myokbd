/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Adapted for use within myokbd as the USBHID_Types.h header was declared
* unsuported in mbed.
*/
#ifndef _USB_HID_H_
#define _USB_HID_H_

#include <stdint.h>
#include <mbed.h>

/* */
#define HID_VERSION_1_11    (0x0111)

/* HID Class */
#define HID_CLASS             (3)
#define HID_SUBCLASS_NONE     (0)
#define HID_SUBCLASS_BOOT     (1)
#define HID_PROTOCOL_NONE     (0)
#define HID_PROTOCOL_KEYBOARD (1)
#define HID_PROTOCOL_MOUSE    (2)

/* Descriptors */
#define HID_DESCRIPTOR          (33)
#define HID_DESCRIPTOR_LENGTH   (0x09)
#define REPORT_DESCRIPTOR       (34)

/* Class requests */
#define GET_REPORT (0x1)
#define GET_IDLE   (0x2)
#define SET_REPORT (0x9)
#define SET_IDLE   (0xa)

/* HID Class Report Descriptor */
/* Short items: size is 0, 1, 2 or 3 specifying 0, 1, 2 or 4 (four) bytes */
/* of data as per HID Class standard */

/* Main items */
#define INPUT(size)             (0x80 | size)
#define OUTPUT(size)            (0x90 | size)
#define FEATURE(size)           (0xb0 | size)
#define COLLECTION(size)        (0xa0 | size)
#define END_COLLECTION           0xc0

/* Global items */
#define USAGE_PAGE(size)        (0x04 | size)
#define LOGICAL_MINIMUM(size)   (0x14 | size)
#define LOGICAL_MAXIMUM(size)   (0x24 | size)
#define PHYSICAL_MINIMUM(size)  (0x34 | size)
#define PHYSICAL_MAXIMUM(size)  (0x44 | size)
#define UNIT_EXPONENT(size)     (0x54 | size)
#define UNIT(size)              (0x64 | size)
#define REPORT_SIZE(size)       (0x74 | size)
#define REPORT_ID(size)         (0x84 | size)
#define REPORT_COUNT(size)      (0x94 | size)
#define PUSH(size)              (0xa4 | size)
#define POP(size)               (0xb4 | size)

/* Local items */
#define USAGE(size)                 (0x08 | size)
#define USAGE_MINIMUM(size)         (0x18 | size)
#define USAGE_MAXIMUM(size)         (0x28 | size)
#define DESIGNATOR_INDEX(size)      (0x38 | size)
#define DESIGNATOR_MINIMUM(size)    (0x48 | size)
#define DESIGNATOR_MAXIMUM(size)    (0x58 | size)
#define STRING_INDEX(size)          (0x78 | size)
#define STRING_MINIMUM(size)        (0x88 | size)
#define STRING_MAXIMUM(size)        (0x98 | size)
#define DELIMITER(size)             (0xa8 | size)

/* [> Keys and modifiers <]
 * #define KEY_LEFT_CTRL     0x80
 * #define KEY_LEFT_SHIFT    0x81
 * #define KEY_LEFT_ALT      0x82
 * #define KEY_LEFT_GUI      0x83
 * #define KEY_RIGHT_CTRL    0x84
 * #define KEY_RIGHT_SHIFT   0x85
 * #define KEY_RIGHT_ALT     0x86
 * #define KEY_RIGHT_GUI     0x87
 * 
 * #define KEY_UP_ARROW      0xDA
 * #define KEY_DOWN_ARROW    0xD9
 * #define KEY_LEFT_ARROW    0xD8
 * #define KEY_RIGHT_ARROW   0xD7
 * #define KEY_BACKSPACE     0xB2
 * #define KEY_TAB           0xB3
 * #define KEY_RETURN        0xB0
 * #define KEY_ESC           0xB1
 * #define KEY_INSERT        0xD1
 * #define KEY_DELETE        0xD4
 * #define KEY_PAGE_UP       0xD3
 * #define KEY_PAGE_DOWN     0xD6
 * #define KEY_HOME          0xD2
 * #define KEY_END           0xD5
 * #define KEY_CAPS_LOCK     0xC1
 * #define KEY_F1            0xC2
 * #define KEY_F2            0xC3
 * #define KEY_F3            0xC4
 * #define KEY_F4            0xC5
 * #define KEY_F5            0xC6
 * #define KEY_F6            0xC7
 * #define KEY_F7            0xC8
 * #define KEY_F8            0xC9
 * #define KEY_F9            0xCA
 * #define KEY_F10           0xCB
 * #define KEY_F11           0xCC
 * #define KEY_F12           0xCD
 *
 * #define MAP_SHIFT 0x80 */


namespace btsvc {

  typedef const uint8_t ReportMap_t[];
  typedef const uint8_t* Report_t;
  typedef uint8_t* mReport_t;

  typedef struct {
      uint16_t bcdHID;
      uint8_t  bCountryCode;
      uint8_t  flags;
  } HIDInformation_t;

  enum ReportType {
      INPUT_REPORT    = 0x1,
      OUTPUT_REPORT   = 0x2,
      FEATURE_REPORT  = 0x3,
  };

  enum ProtocolMode {
      BOOT_PROTOCOL   = 0x0,
      REPORT_PROTOCOL = 0x1,
  };

  typedef struct {
      uint8_t ID;
      uint8_t type;
  } ReportRef_t;

}
#endif
