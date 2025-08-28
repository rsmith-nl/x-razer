// file: x-razer.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-18 14:53:46 +0200
// Last modified: 2025-08-28T19:23:37+0200

#include "cairo-imgui.h"
#include "razer-usb.h"
#include "rc.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cairo/cairo.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  GUI_context *ctx;
  RC_data clr;
  USB_data kb;
} State;


SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  (void)argc;
  (void)argv;
  // Initialize state needed in all functions.
  static State s = {0};
  // Create GUI context.
  static GUI_context ctx = {0};
  s.ctx = &ctx;
  // Read rcfile.
  read_rc(&s.clr);
  // Initialize USB.
  usb_init(&s.kb);
  // Set a theme for the GUI.
  gui_theme_dark(&ctx);
  // Make context available to other callbacks.
  *appstate = &s;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  // The SDL_AppIterate callback should run ≈10× per second.
  SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "10");
  // Create window and renderer.
  int w = 500;
  int h = 165;
  if (!SDL_CreateWindowAndRenderer("x-razer", w, h, 0,
                                   &s.window, &s.renderer)) {
    SDL_Log("Couldn't create a window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  // Render on vsync to prevent tearing
  SDL_SetRenderVSync(s.renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
  // Create texture for cairo to render to.
  s.texture = SDL_CreateTexture(s.renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, w, h);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  (void)appstate;
  State *s = appstate;
  // GUI definition starts here.
  gui_begin(s->renderer, s->texture, s->ctx);
  // RGB
  gui_label(s->ctx, 10, 24, "Red");
  gui_label(s->ctx, 10, 54, "Green");
  gui_label(s->ctx, 10, 84, "Blue");
  static int red = 0, green = 0, blue = 0;
  red = s->clr.red;
  green = s->clr.green;
  blue = s->clr.blue;
  static GUI_rgb samplecolor = {0};
  samplecolor.r = (double)red/255.0;
  samplecolor.g = (double)green/255.0;
  samplecolor.b = (double)blue/255.0;
  static char bred[10] = {0}, bgreen[10] = {0}, bblue[10] = {0};
  if (gui_slider(s->ctx, 60, 20, &red)) {
    samplecolor.r = (double)red/255.0;
    s->clr.red = red;
  }
  if (gui_slider(s->ctx, 60, 50, &green)) {
    samplecolor.g = (double)green/255.0;
    s->clr.green = green;
  }
  if (gui_slider(s->ctx, 60, 80, &blue)) {
    samplecolor.b = (double)blue/255.0;
    s->clr.blue = blue;
  }
  snprintf(bred, 9, "%d", red);
  snprintf(bgreen, 9, "%d", green);
  snprintf(bblue, 9, "%d", blue);
  gui_label(s->ctx, 355, 24, bred);
  gui_label(s->ctx, 355, 54, bgreen);
  gui_label(s->ctx, 355, 84, bblue);
  gui_colorsample(s->ctx, 390.0, 10.0, 100.0, 100.0, &samplecolor);
  // Close button
  if (gui_button(s->ctx, 10, 120, "Close")) {
    return SDL_APP_SUCCESS;
  }
  // Choose a theme
  static const char *btns[2] = {"light", "dark"};
  static int radio = 1;
  gui_label(s->ctx, 90, 105, "Theme");
  if (gui_radiobuttons(s->ctx, 90, 120, 2, btns, &radio)) {
    if (radio == 0) {
      gui_theme_light(s->ctx);
      // puts("switching to light theme.");
    } else if (radio == 1) {
      gui_theme_dark(s->ctx);
      // puts("switching to dark theme.");
    }
  }
  // Show cursor position to help with layout.
  //char buf[80] = {0};
  //snprintf(buf, 79, "x = %d, y = %d", s->ctx->mouse_x, s->ctx->mouse_y);
  //gui_label(s->ctx, 180, 130, buf);
  // Show messages.
  if (s->clr.ok == false) {
    gui_label(s->ctx, 160, 130, "Could not read RC file!");
  }
  if (s->kb.errormsg != 0) {
    gui_label(s->ctx, 160, 150, s->kb.errormsg);
  } else {
    gui_label(s->ctx, 160, 150, s->kb.product_name);
  }
  // Apply changes button
  if (gui_button(s->ctx, 400, 120, "Apply")) {
    // TODO: set color using libusb.
    write_rc(&s->clr);
  }
  // You can still draw to s->ctx here...
  // End of GUI definition
  gui_end(s->ctx);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  State *s = appstate;
  return gui_process_events(s->ctx, event);
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  State *s = appstate;
  (void)result;
  // Clean up.
  usb_exit();
  SDL_DestroyTexture(s->texture);
  SDL_DestroyWindow(s->window);
  SDL_DestroyRenderer(s->renderer);
}

