// file: razer-usb.h
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 16:30:17 +0200
// Last modified: 2025-08-28T16:32:36+0200

#pragma once

#include <libusb.h>

typedef struct {
  char product_name[80];
  libusb_device_handle *handle;
} USB_data;

extern bool usb_init(USB_data *out);
extern void usb_exit(void);
