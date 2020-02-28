#include "KeyboardConfig.h"

using namespace btsvc;

/*
 * Declare constant variables required by the KeyboardService
 */
ReportMap_t  btsvc::KbdConfig::ReportMapDescriptor = {
  USAGE_PAGE(1),      0x01,                         // Generic Desktop
  USAGE(1),           0x06,                         // Keyboard //TODO(lc525): change to Keypad?
  COLLECTION(1),      0x01,                         // Application
  USAGE_PAGE(1),      0x07,                         // Usage Page (Key Codes)
  USAGE_MINIMUM(1),   0xE0,                         // Usage Minimum (224)
  USAGE_MAXIMUM(1),   0xE7,                         // Usage Maximum (231)
  LOGICAL_MINIMUM(1), 0x00,                         // Logical Minimum (0)
  LOGICAL_MAXIMUM(1), 0x01,                         // Logical Maximum (1)
  REPORT_SIZE(1),     0x01,                         // Report Size (1)
  REPORT_COUNT(1),    0x08,                         // Report Count (8)
  INPUT(1),           0x02,                         // Modifier (Data, Variable, Abs)

  REPORT_SIZE(1),     0x08,                         // Report Size (8)
  REPORT_COUNT(1),    0x01,                         // Report Count (1)
  INPUT(1),           0x01,                         // Reserved (Constant)

  // Output report (5 bytes for leds + 3 padding)
  REPORT_SIZE(1),     0x01,                         // Report Size (1)
  REPORT_COUNT(1),    0x05,                         // Report Count (5)
  USAGE_PAGE(1),      0x08,                         // Usage Page (Page# for LEDs)
  USAGE_MINIMUM(1),   0x01,                         // Usage Minimum (1)
  USAGE_MAXIMUM(1),   0x05,                         // Usage Maximum (5)
  OUTPUT(1),          0x02,                         // LEDS (Data, Variable, Abs)

  REPORT_SIZE(1),     0x03,                         // Report Size (3)
  REPORT_COUNT(1),    0x01,                         // Report Count (1)
  OUTPUT(1),          0x01,                         // Padding (Const, Array, Abs)

  // Keys (6 bytes)
  REPORT_SIZE(1),     0x08,                         // Report Size (8)
  REPORT_COUNT(1),    0x06,                         // Report Count (6)
  LOGICAL_MINIMUM(1), 0x00,                         // Logical Minimum (0)
  LOGICAL_MAXIMUM(1), 0x65,                         // Logical Maximum (101 keys)
  USAGE_PAGE(1),      0x07,                         // Usage Page (Key codes)
  USAGE_MINIMUM(1),   0x00,                         // Usage Minimum (0)
  USAGE_MAXIMUM(1),   0x65,                         // Usage Maximum (101)
  INPUT(1),           0x00,                         // Keys (Data, Array, 6 bytes)

  //TODO(lc525): implement feature report
  /* USAGE(1),           0x05,                         // Usage (Vendor Defined)
   * LOGICAL_MINIMUM(1), 0x00,                         // Logical Minimum (0)
   * LOGICAL_MAXIMUM(2), 0xFF, 0x00,                   // Logical Maximum (255)
   * REPORT_SIZE(1),     0x08,                         // Report Size (8 bit)
   * REPORT_COUNT(1),    0x02,                         // Report Count (2)
   * FEATURE(1),         0x02,                         // Feature (Data, Variable, Absolute) */

  END_COLLECTION                                    // End Collection (Application)
};

HIDInformation_t btsvc::KbdConfig::HIDInfo={
  HID_VERSION_1_11,
  0x00,       // No Localization
  0x03        // RemoteWake | NormallyConnectable
};

uint8_t btsvc::KbdConfig::ReportMapLen = sizeof(KbdConfig::ReportMapDescriptor);

// report data containing pressed keys
uint8_t btsvc::KbdConfig::InputReportData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
// empty report data for releasing all keys
const uint8_t btsvc::KbdConfig::EmptyInputReportData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
const uint8_t btsvc::KbdConfig::InputReportLen = sizeof(KbdConfig::EmptyInputReportData);
// report data containing keyboard output (LEDs)
uint8_t btsvc::KbdConfig::OutputReportData[] = { 0 };
const uint8_t btsvc::KbdConfig::OutputReportLen = sizeof(KbdConfig::OutputReportData);

