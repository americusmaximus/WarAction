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

#include "Basic.hxx"

#include <Graphics.Basic.hxx>
#include <Renderer.Basic.hxx>

#define ADJUSTCOLOR(x)                                                                              \
        (x >> (State.Renderer->RedOffset & 0x1F) & 0xFFFF & State.Renderer->ActualRedMask)          \
        | (x >> (State.Renderer->GreenOffset & 0x1F) & 0xFFFF & State.Renderer->ActualGreenMask)    \
        | (x >> (State.Renderer->BlueOffset & 0x1F) & 0xFFFF & State.Renderer->ActualBlueMask)

#define ADJUSTCOLORS(r, g, b)                                                                       \
        (r >> (State.Renderer->RedOffset & 0x1F) & 0xFFFF & State.Renderer->ActualRedMask)          \
        | (g >> (State.Renderer->GreenOffset & 0x1F) & 0xFFFF & State.Renderer->ActualGreenMask)    \
        | (b >> (State.Renderer->BlueOffset & 0x1F) & 0xFFFF & State.Renderer->ActualBlueMask)

#define ADJUSTSPRITECOLOR(x)                                                                                \
        ((x & 0xF800) >> (State.Renderer->RedOffset & 0x1F) & State.Renderer->ActualRedMask)                \
        | (((x & 0x07E0) << 5) >> (State.Renderer->GreenOffset & 0x1F) & State.Renderer->ActualGreenMask)   \
        | (((x & 0x001F) << 11) >> (State.Renderer->BlueOffset & 0x1F) & State.Renderer->ActualBlueMask)

#define ADJUSTSPRITECOLORS(r, g, b)                                                                     \
        ((r << 8) >> (State.Renderer->RedOffset & 0x1F) & 0xFFFF & State.Renderer->ActualRedMask)       \
        | ((g << 8) >> (State.Renderer->GreenOffset & 0x1F) & 0xFFFF & State.Renderer->ActualGreenMask) \
        | ((b << 8) >> (State.Renderer->BlueOffset & 0x1F) & 0xFFFF & State.Renderer->ActualBlueMask)

VOID WriteBackSurfaceMainSurfaceWindowRectangle(VOID);
VOID WriteMainSurfaceMainSurfaceWindowRectangle(VOID);