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

#include "ConvertColors.hxx"
#include "SetPixelColorMasks.hxx"

static PIXEL Convert(PIXEL pixel, U32 ro, U32 go, U32 bo, U32 rm, U32 gm, U32 bm)
{
#if ACTIVATE_COMPLETE_RGBA_MODE
    // TODO
#else
    CONST PIXEL r = (((pixel & 0xF800) <<  0) >> (ro & 0x1f)) & rm;
    CONST PIXEL g = (((pixel & 0x07E0) <<  5) >> (go & 0x1f)) & gm;
    CONST PIXEL b = (((pixel & 0x001F) << 11) >> (bo & 0x1f)) & bm;

    return (PIXEL)(r | g | b);
#endif
}

static VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event, U32 ro, U32 go, U32 bo, U32 rm, U32 gm, U32 bm)
{
    CONST U32 count = 1 << (sizeof(PIXEL) << 3);

    for (U32 x = 0; x < count; x++)
    {
        PIXEL src = x;
        PIXEL dst = 0;

        state->Actions.ConvertColors(&src, &dst, 1);

        CONST PIXEL result = Convert(src, ro, go, bo, rm, gm, bm);

        if (dst != result) { event->Result = FALSE; return; }
    }

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, RO, GO, BO, RM, GM, BM) { E->Action = A; Execute(S, E, RO, GO, BO, RM, GM, BM); if (!E->Result) { return; } }

VOID ConvertColors(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    // Initialize.
    InitializePixelMasks(state);

    {
        InitializePixelMasks(state);
        state->Actions.SetPixelColorMasks(0, 0, 0);

        EXECUTE("R: 0 G: 0 B: 0", state, event, state->RedOffset, state->GreenOffset, state->BlueOffset,
            state->ActualRedMask, state->ActualGreenMask, state->ActualBlueMask);
    }

    {
        InitializePixelMasks(state);
        state->Actions.SetPixelColorMasks(0xF800, 0x7E0, 0x1F);

        EXECUTE("R: 0xF800 G: 0x7E0 B: 0x1F", state, event, state->RedOffset, state->GreenOffset, state->BlueOffset,
            state->ActualRedMask, state->ActualGreenMask, state->ActualBlueMask);
    }

    {
        InitializePixelMasks(state);
        state->Actions.SetPixelColorMasks(0x7E0, 0x1F, 0xF800);

        EXECUTE("R: 0x7E0 G: 0x1F B: 0xF800", state, event, state->RedOffset, state->GreenOffset, state->BlueOffset,
            state->ActualRedMask, state->ActualGreenMask, state->ActualBlueMask);
    }

    // Finalize.
    InitializePixelMasks(state);
}
