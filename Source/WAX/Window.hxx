/*
Copyright (c) 2024 - 2025 Americus Maximus

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

#include "Basic.hxx"
#include "Native.Basic.hxx"

#define VK_NULL                     0
#define VK_INPUT                    0xFF

#define MAX_WINDOW_TITLE_LENGTH     256

typedef enum WindowInputState
{
    WINDOWINPUTSTATE_NONE               = 0,
    WINDOWINPUTSTATE_ACCEPT_MOUSE       = 1,
    WINDOWINPUTSTATE_ACCEPT_KEYBOARD    = 2,
    WINDOWINPUTSTATE_FORCE_DWORD        = 0x7FFFFFFF
} WINDOWINPUTSTATE, * WINDOWINPUTSTATEPTR;

typedef struct Window
{
    HINSTANCE   Instance;
    LPCSTR      Args;
    HWND        HWND;
    WNDCLASSA   Class;
    CHAR        Title[MAX_WINDOW_TITLE_LENGTH];
    DWORD       Style;
    S32         X;
    S32         Y;
    S32         Width;
    S32         Height;
    HMENU       Menu;
    BOOL        IsActive;
} WINDOW, * WINDOWPTR;