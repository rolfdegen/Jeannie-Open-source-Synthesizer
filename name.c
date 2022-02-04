#include "usb_names.h"

#define MIDI_NAME   {'J','E','A','N','N','I','E'}
#define MIDI_NAME_LEN  7

#define MANUFACTURER_NAME  {'T','u','b','e','O','h','m','.','c','o','m',}
#define MANUFACTURER_NAME_LEN 11

struct usb_string_descriptor_struct usb_string_product_name = {
  2 + MIDI_NAME_LEN * 2,
  3,
  MIDI_NAME
};

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
  2 + MANUFACTURER_NAME_LEN * 2,
  3,
  MANUFACTURER_NAME
};
