// file: razer-get-serial.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-27 22:43:50 +0200
// Last modified: 2025-08-28T00:16:29+0200

// Compile with “cc -o razer-get-serial razer-get-serial.c -lusb”

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <libusb.h>


int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  fprintf(stderr, "DEBUG: starting razer-get-serial.c\n");
  if (libusb_init(0) != 0) {
    fputs("libusb_init failed.\n", stderr);
    return 1;
  }
  libusb_device **device_list;
  ssize_t device_count =  libusb_get_device_list(0, &device_list);
  if (device_count == 0) {
    fputs("no devices found\n", stderr);
    goto exit;
  }
  fprintf(stderr, "INFO: found %ld devices\n", device_count);
  libusb_device_descriptor desc = {0};
  libusb_device_handle *handle = 0;
  for (int32_t k = 0; k < device_count; k++) {
    if (libusb_get_device_descriptor(device_list[k], &desc) != 0) {goto exit;}
    if (desc.idVendor != 0x1532) {
      continue;
    }
    if (libusb_open(device_list[k], &handle) != 0) {continue;}
    unsigned char buf[80] = {0};
    if (libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, 79) > 0) {
      printf("Manufacturer: %s\n", buf);
    }
    memset(buf, 0, 80);
    if (libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, 79) > 0) {
      printf("Product: %s\n", buf);
    }
    memset(buf, 0, 80);
    if (libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buf, 79) > 0) {
      printf("Serial number: %s\n", buf);
      goto exit;
    }
  }
  if (handle == 0) goto exit;
  // First control message
  int bytes_transferred = 0;
  unsigned char serial_message[90] = {0};
  serial_message[0] = 0x00; // status
  serial_message[1] = 0xff; // transaction id.
  serial_message[2] = 0x00; // protocol type
  serial_message[3] = 0x00; // protocol type
  serial_message[4] = 0x00; // remaining packets
  serial_message[5] = 0x16; // data size
  serial_message[6] = 0x16; // command class
  serial_message[7] = 0x82; // command id
  uint8_t checksum = 0;
  for (int32_t k = 2; k < 88; k++) {
    checksum ^= serial_message[k];
  }
  serial_message[88] = checksum;
  bytes_transferred = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x01,
      serial_message, 90, 0);
  if (bytes_transferred != 90) {
    fprintf(stderr, "First control message failed.\n");
    goto exit;
  }
  // Second control message
  unsigned char return_message[90] = {0};
  bytes_transferred = libusb_control_transfer(handle, 0xa1, 0x01, 0x300, 0x01,
      return_message, 90, 0);
  if (bytes_transferred != 90) {
    fprintf(stderr, "Second control message failed.\n");
    goto exit;
  }
  fprintf(stderr, "DEBUG: status = %hhx\n", return_message[0]);
  uint8_t datasize = return_message[5];
  if (datasize >= 80) {
    datasize = 79;
  }
  unsigned char sn[80] = {0};
  memcpy(sn, return_message+8, datasize);
  printf("Serial number: %s\n", sn);

exit:
  libusb_free_device_list(device_list, 1);
  libusb_exit(0);
  return 0;
}
