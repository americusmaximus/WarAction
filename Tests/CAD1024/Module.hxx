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

#include <Basic.hxx>
#include <Native.Basic.hxx>
#include <RendererModule.Import.hxx>
#include <RendererModule.Export.hxx>

#define MAX_RENDERER_WIDTH 1024
#define MAX_RENDERER_HEIGHT 768

typedef struct ModuleEvent
{
    LPCSTR  Name;
    LPCSTR  Action;
    BOOL    Result;
} MODULEEVENT, * MODULEEVENTPTR;

typedef struct Rectangle
{
    S32     X;
    S32     Y;
    S32     Width;
    S32     Height;
} RECTANGLE, * RECTANGLEPTR;

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#define WIDTH(x, width)  MAX(0, MIN(MAX_RENDERER_WIDTH, x + width))
#define HEIGHT(y, height)  MAX(0, MIN(MAX_RENDERER_HEIGHT, y + height))