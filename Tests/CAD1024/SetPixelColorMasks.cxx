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

#include "SetPixelColorMasks.hxx"

VOID InitializePixelMasks(RENDERERPTR state)
{
    state->ActualColorMask = 0;
    state->ActualRedMask = 0;
    state->ActualGreenMask = 0;
    state->ActualBlueMask = 0;

    state->InitialColorMask = 0;
    state->InitialRedMask = 0;
    state->InitialGreenMask = 0;
    state->InitialBlueMask = 0;

    state->RedOffset = 0;
    state->GreenOffset = 0;
    state->BlueOffset = 0;

    state->ActualColorBitsCopy = 0;
    state->ShadeColorMaskCopy = 0;
    state->InvertedActualColorMask = 0;
    state->InvertedActualColorMaskCopy = 0;
    state->InvertedActualColorMaskCopy = 0;
    state->InitialRGBMask = 0;
    state->ActualRGBMask = 0;
    state->ShadePixel = 0;
}

BOOL Check(RENDERERPTR state, COLORMASKRESULTPTR result)
{
    BOOL value = TRUE;

    value &= state->ActualColorMask == result->ActualColorMask;
    value &= state->ActualRedMask == result->ActualRedMask;
    value &= state->ActualGreenMask == result->ActualGreenMask;
    value &= state->ActualBlueMask == result->ActualBlueMask;

    value &= state->InitialColorMask == result->InitialColorMask;
    value &= state->InitialRedMask == result->InitialRedMask;
    value &= state->InitialGreenMask == result->InitialGreenMask;
    value &= state->InitialBlueMask == result->InitialBlueMask;

    value &= state->RedOffset == result->RedOffset;
    value &= state->GreenOffset == result->GreenOffset;
    value &= state->BlueOffset == result->BlueOffset;

    value &= state->ActualColorBitsCopy == result->ActualColorBitsCopy;
    value &= state->ShadeColorMask == result->ShadeColorMask;
    value &= state->ShadeColorMaskCopy == result->ShadeColorMaskCopy;
    value &= state->InvertedActualColorMask == result->InvertedActualColorMask;
    value &= state->InvertedActualColorMaskCopy == result->InvertedActualColorMaskCopy;
    value &= state->InitialRGBMask == result->InitialRGBMask;
    value &= state->ActualRGBMask == result->ActualRGBMask;
    value &= state->ShadePixel == result->ShadePixel;

    return value;
}

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, U32 r, U32 g, U32 b, COLORMASKRESULTPTR result)
{
    InitializePixelMasks(state);

    state->Actions.SetPixelColorMasks(r, g, b);

    event->Result = Check(state, result);
}

#define EXECUTE(A, S, E, R, G, B, RRR) { E->Action = A; Execute(S, E, R, G, B, RRR); if (!E->Result) { return; } }

VOID SetPixelColorMasks(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    InitializePixelMasks(state);

    //
    {
        COLORMASKRESULT result = { 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 0x821, 0xF7DE, 0xF7DE, 0xFFFF, 0xFFFF, 0, 0, 0 };
        EXECUTE("R: 0x0 G: 0x0 B: 0x0", state, event, 0, 0, 0, &result);
    }
    {
        COLORMASKRESULT result = { 0x8410, 0xF800, 0x7E0, 0x1F, 0x8410, 0xF800, 0x7E0, 0x1F, 0, 5, 11, 0x821, 0xF7DE, 0xF7DE, 0x7BEF, 0x7BEF, 0x7E0F81F, 0x4008010, 0x00850085 };
        EXECUTE("R: 0xF800 G: 0x7E0 B: 0x1F", state, event, 0xF800, 0x7E0, 0x1F, &result);
    }
    {
        COLORMASKRESULT result = { 0x8410, 0x7E0, 0x1F, 0xF800, 0x8410, 0x7E0, 0x1F, 0xF800, 5, 11, 0, 0x821, 0xF7DE, 0xF7DE, 0x7BEF, 0x7BEF, 0x7E0F81F, 0x4008010, 0x28022802 };
        EXECUTE("R: 0x7E0 G: 0x1F B: 0xF800", state, event, 0x7E0, 0x1F, 0xF800, &result);
    }

    // Finalize.
    InitializePixelMasks(state);
}