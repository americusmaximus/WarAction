/*
Copyright (c) 2024 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Graphics.Basic.hxx"
#include "Native.Basic.hxx"

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

// Defines graphics interface to be used by the renderer:
// DirectDraw7, is the option is enabled,
// DirectDraw, if the option is disabled.
// NOTE: The options is disabled by default to, as in the original game.
#ifdef _WIN64
#define ACTIVATE_MODERN_MODE TRUE
#endif

// Defines the graphics mode to be used by the renderer:
// 32-bit colors, if the option is enabled,
// 16-bit colors, if the option is enabled.
// NOTE: The option is disabled by default, to match the original game. TODO
#define ACTIVATE_COMPLETE_RGBA_MODE FALSE

#if ACTIVATE_COMPLETE_RGBA_MODE
// TODO: NOT IMPLEMENTED
#define DEFAULT_SCREEN_DEPTH_BITS           GRAPHICS_BITS_PER_PIXEL_32
#define DEFAULT_SCREEN_DEPTH_BYTES          (DEFAULT_SCREEN_DEPTH_BITS >> 3)
#define DEFAULT_SCREEN_DEPTH_COLOR_MASK     0xFFFFFFFF

typedef U32 PIXEL;
#else
#define DEFAULT_SCREEN_DEPTH_BITS           GRAPHICS_BITS_PER_PIXEL_16
#define DEFAULT_SCREEN_DEPTH_BYTES          (DEFAULT_SCREEN_DEPTH_BITS >> 3)
#define DEFAULT_SCREEN_DEPTH_COLOR_MASK     0x0000FFFF

typedef U16 PIXEL;
#endif