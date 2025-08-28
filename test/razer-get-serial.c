// file: razer-get-serial.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-27 22:43:50 +0200
// Last modified: 2025-08-28T11:08:20+0200

// Compile with “cc -std=c11 -o razer-get-serial razer-get-serial.c -lusb”

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <libusb.h>

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


uint8_t calculate_crc(Razer_report *report) {
  uint8_t *_report = (uint8_t*)report;
  uint8_t crc = 0;
  for (int j = 2; j < 88; j++) {
    crc ^= _report[j];
  }
  return crc;
}

void print_serial(libusb_device_handle *handle){
  // First control message
  int bytes_transferred = 0;
  Razer_report out_message = {
    .status = 0x00,
    .transaction_id = 0xff,
    .remaining_packets = 0x00,
    .protocol_type = 0x00,
    .data_size = 0x16,
    .command_class = 0x00,
    .command_id = 0x82,
  };
  out_message.crc = calculate_crc(&out_message);
  bytes_transferred = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x01,
                      (uint8_t*)&out_message, 90, 0);
  if (bytes_transferred != 90) {
    //fprintf(stderr, "First control message failed.\n");
    return;
  }
  // Second control message
  Razer_report return_message;
  bytes_transferred = libusb_control_transfer(handle, 0xa1, 0x01, 0x300, 0x01,
                      (uint8_t*)&return_message, 90, 0);
  if (bytes_transferred != 90) {
    //fprintf(stderr, "Second control message failed.\n");
    return;
  }
  //fprintf(stderr, "DEBUG: status = %hhx\n", return_message.status);
  uint8_t datasize = return_message.data_size;
  if (datasize >= 80) {
    datasize = 79;
  }
  unsigned char sn[80] = {0};
  memcpy(sn, return_message.arguments, datasize);
  printf("Serial number: %s\n", sn);
}

void print_firmware(libusb_device_handle *handle){
  // First control message
  int bytes_transferred = 0;
  Razer_report out_message = {
    .status = 0x00,
    .transaction_id = 0xff,
    .remaining_packets = 0x00,
    .protocol_type = 0x00,
    .data_size = 0x02,
    .command_class = 0x00,
    .command_id = 0x81,
  };
  out_message.crc = calculate_crc(&out_message);
  bytes_transferred = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x01,
                      (uint8_t*)&out_message, 90, 0);
  if (bytes_transferred != 90) {
    //fprintf(stderr, "First control message failed.\n");
    return;
  }
  // Second control message
  Razer_report return_message;
  bytes_transferred = libusb_control_transfer(handle, 0xa1, 0x01, 0x300, 0x01,
                      (uint8_t*)&return_message, 90, 0);
  if (bytes_transferred != 90) {
    //fprintf(stderr, "Second control message failed.\n");
    return;
  }
  //fprintf(stderr, "DEBUG: status = %hhx\n", return_message.status);
  printf("Firmware version: v%d.%d\n", return_message.arguments[0],
      return_message.arguments[1]);
}




int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  //fprintf(stderr, "DEBUG: starting razer-get-serial.c\n");
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
  //fprintf(stderr, "INFO: found %ld devices\n", device_count);
  libusb_device_descriptor desc = {0};
  libusb_device_handle *handle = 0;
  for (int32_t k = 0; k < device_count; k++) {
    if (libusb_get_device_descriptor(device_list[k], &desc) != 0) {
      goto exit;
    }
    if (desc.idVendor != 0x1532) {
      continue;
    }
    if (libusb_open(device_list[k], &handle) != 0) {
      continue;
    }
    unsigned char buf[80] = {0};
    if (libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, 79) > 0) {
      printf("Manufacturer: %s\n", buf);
    }
    memset(buf, 0, 80);
    if (libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, 79) > 0) {
      printf("Product: %s\n", buf);
    }
    print_serial(handle);
    print_firmware(handle);
  }
exit:
  libusb_free_device_list(device_list, 1);
  libusb_exit(0);
  return 0;
}
