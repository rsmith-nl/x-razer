// file: razer-usb.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 16:01:44 +0200
// Last modified: 2025-08-28T18:38:47+0200

#include <stdint.h>
#include <stdbool.h>
#include <libusb.h>
#include <string.h>

#include "razer-usb.h"

typedef struct {
  uint8_t status;
  uint8_t transaction_id;
  uint16_t remaining_packets; /* Big Endian */
  uint8_t protocol_type; /*0x0*/
  uint8_t data_size;
  uint8_t command_class;
  uint8_t command_id;
  uint8_t arguments[80];
  uint8_t crc;/*xor'ed bytes of report*/
  uint8_t reserved; /*0x0*/
} Razer_report;

static const char *errors[5] = {
  "Could not initialize USB.",
  "Not a supported keyboard.",
  "Could not retrieve product name.",
  "Could not get a list of USB devices.",
  "Could not get a USB device descriptor.",
};

uint8_t calculate_crc(Razer_report *report)
{
  uint8_t *_report = (uint8_t*)report;
  uint8_t crc = 0;
  for (int j = 2; j < 88; j++) {
    crc ^= _report[j];
  }
  return crc;
}

void usb_init(USB_data *out)
{
  const uint16_t keyboard_ids[] = {
    0x0228, // Blackwidow Elite
    0x021E, // Ornata Chroma
  };
  if (out == 0) {
    return;
  }
  memset(out, 0, sizeof(USB_data));
  if (libusb_init(0) != 0) {
    out->errormsg = errors[0];
    return;
  }
  libusb_device **device_list;
  ssize_t device_count =  libusb_get_device_list(0, &device_list);
  if (device_count == 0) {
    out->errormsg = errors[3];
    return;
  }
  libusb_device_descriptor desc = {0};
  for (int32_t k = 0; k < device_count; k++) {
    if (libusb_get_device_descriptor(device_list[k], &desc) != 0) {
      out->errormsg = errors[4];
      break;
    }
    if (desc.idVendor != 0x1532) { // Not a Razer device.
      continue; // Try next device.
    }
    bool found = false;
    for (int32_t j = 0; j < sizeof(keyboard_ids)/sizeof(keyboard_ids[0]); j++) {
      if (desc.idProduct == keyboard_ids[j]) {
        found = true;
        break;
      }
    }
    if (found == false) { // Not a device in the list.
      out->errormsg = errors[1];
      continue; // Try next device.
    }
    if (libusb_open(device_list[k], &out->handle) != 0) {
      out->handle = 0;
      break;
    }
    if (libusb_get_string_descriptor_ascii(out->handle, desc.iProduct,
                                           (uint8_t*)&out->product_name, 79) != 0) {
      break;
    } else {
    }
  }
  libusb_free_device_list(device_list, 1);
}

void usb_exit(void)
{
  libusb_exit(0);
}

bool usb_set_color(int red, int green, int blue)
{
  return false;
}
