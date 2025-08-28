// file: rc.h
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 18:48:01 +0200
// Last modified: 2025-08-28T19:58:23+0200

#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool ok;
  uint8_t red, green, blue;
} RC_data;

// Read or write ~/.x-razerrc.
extern void read_rc(RC_data *result);
extern void write_rc(RC_data *result);
