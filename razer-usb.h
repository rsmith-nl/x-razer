// file: razer-usb.h
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 16:30:17 +0200
// Last modified: 2025-08-28T18:37:55+0200

#pragma once

#include <stdbool.h>
#include <libusb.h>

typedef struct {
  const char *errormsg;
  char product_name[80];
  libusb_device_handle *handle;
} USB_data;

// If the initialization is succecfull, out->errormsg is 0.
// Otherwise it points to an error message.
extern void usb_init(USB_data *out);
extern void usb_exit(void);

extern bool usb_set_color(int red, int green, int blue);
