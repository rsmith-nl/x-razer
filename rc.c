// file: rc.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 18:48:01 +0200
// Last modified: 2026-02-22T11:47:28+0100

#include "rc.h"
#include "sbuf.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

// Macro to skip whitespace in a string.
#define SKIPWS(ptr) \
  while (*(ptr) == ' ' || *(ptr) == '\t' || *(ptr) == '\r' || *(ptr) == '\n') {(ptr)++;}

#define CLAMP(val) ((val)<0)?0:((val)>255?255:(val))


static const char *filename = "/.x-razerrc";

// Read the RC file.
// If succesful, the color is stored in result.
// result->ok is true if reading succeeded.
void read_rc(RC_data *result)
{
  assert(result);
  Sbuf sbuf = {0};
  result->ok = false;
  result->red = result->green = result->blue = 0;
  const char *home = getenv("HOME");
  if (home == 0) {
    return;
  }
  sbuf_appends(&sbuf, home);
  sbuf_appends(&sbuf, filename);
  if (sbuf.error == true) {
    return;
  }
  FILE *rcfile = fopen(sbuf.data, "r");
  if (rcfile == 0) {
    return;
  }
  sbuf_reset(&sbuf);
  sbuf.used = fread(sbuf.data, 1, SBUF_SIZE, rcfile);
  fclose(rcfile);
  if (sbuf.used == 0) {
    return;
  }
  char *cur = sbuf.data;
  SKIPWS(cur);
  long red = strtol(cur, &cur, 10);
  CLAMP(red);
  SKIPWS(cur);
  long green = strtol(cur, &cur, 10);
  CLAMP(green);
  SKIPWS(cur);
  long blue = strtol(cur, &cur, 10);
  CLAMP(blue);
  if (errno == EINVAL) {
    return;
  }
  result->red = red;
  result->green = green;
  result->blue = blue;
  result->ok = true;
  return;
}

void write_rc(RC_data *result)
{
  assert(result);
  Sbuf sbuf = {0};
  result->ok = false;
  const char *home = getenv("HOME");
  if (home == 0) {
    return;
  }
  sbuf_appends(&sbuf, home);
  sbuf_appends(&sbuf, filename);
    if (sbuf.error == true) {
    return;
  }
  FILE *rcfile = fopen(sbuf.data, "w");
  if (rcfile == 0) {
    return;
  }
  fprintf(rcfile, "%d\n%d\n%d\n", result->red, result->green, result->blue);
  fclose(rcfile);
  result->ok = true;
}
