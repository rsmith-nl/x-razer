Set the backlight color on some Razer keyboards
###############################################

:date: 2025-08-31
:tags: SDL3, cairo, Razer keyboard, public domain
:author: Roland Smith

.. Last modified: 2025-08-31T13:48:32+0200
.. vim:spelllang=en

Introduction
============

This program can be used to set the backlight color on some Razer keyboard.
It has been tested and confirmed to work on:

* Blackwidow Elite
* Ornata Chroma

You can try other keyboards by adding their product ID's to the
``keyboard_ids`` array in the function ``usb_init`` in the file
``razer-usb.c``.

The user interface is made with an included small immediate mode GUI that
I wrote myself. It relies mostly on mouse input.


Requirements
============

1) UNIX-like operating system. Developed on FreeBSD.
2) ``libusb`` library.
3) ``SDL3`` library built with support for X11 or Wayland. Only X11 has been
   tested.
4) ``Cairo`` graphics library.

To compile it you'll additionaly need;

* BSD or GNU make
* C compiler. Developed with Clang. ``CFLAGS`` might need adjusting for ``gcc``.

Dotfile
=======

This program will try to read/write ``$HOME/.x-razerrc``.
A sample is provided.
