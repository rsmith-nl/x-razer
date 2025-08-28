// file: rc.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 18:48:01 +0200
// Last modified: 2025-08-28T19:02:50+0200

#include "rc.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Macro to skip whitespace in a string.
#define SKIPWS(ptr) \
  while (*(ptr) == ' ' || *(ptr) == '\t' || *(ptr) == '\r' || *(ptr) == '\n') {(ptr)++;}

static const char *filename = "/.x-razerrc";

#define BUF_SIZE 4096
// Read the RC file.
// If succesful, the color is stored in result.
// result->ok is true if reading succeeded.
void read_rc(RC_data *result)
{
  assert(result);
  result->ok = false;
  result->red = result->green = result->blue = 0;
  const char *home = getenv("HOME");
  if (home == 0) {
    return;
  }
  size_t bufused = 0;
  char buf[BUF_SIZE] = {0};
  char *cur = buf;
  memcpy(buf, home, strnlen(home, BUF_SIZE-1));
  bufused = strnlen(buf, BUF_SIZE-1);
  cur += bufused;
  memcpy(cur, filename, strlen(filename));
  FILE *rcfile = fopen(buf, "r");
  if (rcfile == 0) {
    return;
  }
  fseek(rcfile, 0, SEEK_END);
  size_t read_size = (size_t)ftell(rcfile);
  if (read_size >= BUF_SIZE) {
    read_size = BUF_SIZE-1;
  }
  rewind(rcfile);
  memset(buf, 0, BUF_SIZE);
  read_size = fread(buf, read_size, 1, rcfile);
  fclose(rcfile);
  if (read_size == 0) {
    return;
  }
  cur = buf;
  SKIPWS(cur);
  long red = strtol(cur, &cur, 10);
  SKIPWS(cur);
  long green = strtol(cur, &cur, 10);
  SKIPWS(cur);
  long blue = strtol(cur, &cur, 10);
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
  result->ok = false;
  const char *home = getenv("HOME");
  if (home == 0) {
    return;
  }
  char buf[BUF_SIZE] = {0};
  char *cur = buf;
  memcpy(buf, home, strnlen(home, BUF_SIZE-1));
  cur += strnlen(buf, BUF_SIZE-1);
  memcpy(cur, filename, strlen(filename));
  FILE *rcfile = fopen(buf, "w");
  if (rcfile == 0) {
    return;
  }
  fprintf(rcfile, "%d\n%d\n%d\n", result->red, result->green, result->blue);
  fclose(rcfile);
  result->ok = true;
}
