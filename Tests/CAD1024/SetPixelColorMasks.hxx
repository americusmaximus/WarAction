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

#include "Module.hxx"

typedef struct ColorMaskResult
{
    U16 ActualColorMask;
    U16 ActualRedMask;
    U16 ActualGreenMask;
    U16 ActualBlueMask;

    U16 InitialColorMask;
    U16 InitialRedMask;
    U16 InitialGreenMask;
    U16 InitialBlueMask;

    U16 RedOffset;
    U16 GreenOffset;
    U16 BlueOffset;

    U16 ActualColorBitsCopy;
    U16 ShadeColorMask;
    U16 ShadeColorMaskCopy;
    U16 InvertedActualColorMask;
    U32 InvertedActualColorMaskCopy;
    U32 InitialRGBMask;
    U32 ActualRGBMask;
    DOUBLEPIXEL ShadePixel;
} COLORMASKRESULT, * COLORMASKRESULTPTR;

VOID InitializePixelMasks(RENDERERPTR state);
VOID SetPixelColorMasks(RENDERERPTR state, MODULEEVENTPTR event);