/*
Copyright (c) 2025 Americus Maximus

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

#include "BitMap.hxx"
#include "File.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"
#include "SetPixelColorMasks.hxx"
#include "ShadeMainSurfaceRhomb.hxx"

#include <stdio.h>

#define MAX_HORIZONTAL_OFFSET   36
#define MAX_VERTICAL_OFFSET     36

#define MAX_SPRITE_SIZE        1024

static IMAGESPRITEPTR AcquireSprite(LPVOID content, CONST U32 indx)
{
    return (IMAGESPRITEPTR)((ADDR)content + (ADDR)(((U32*)content)[indx]));
}

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 a1, S32 a2, S32 a3, S32 a4)
{
    Initialize(state);

    for (U32 xx = 0; xx < 25; xx++)
    {
        for (U32 xxx = 0; xxx < 20; xxx++)
        {
            CONST S32 h = x + xx * MAX_HORIZONTAL_OFFSET + xxx * MAX_HORIZONTAL_OFFSET;
            CONST S32 v = y + xxx * MAX_VERTICAL_OFFSET;

            state->Actions.DrawMainSurfaceColorBox(h, v, 25, 25, 0xFF);
            state->Actions.ShadeMainSurfaceRhomb(h, v, a1, a2, a3, a4);
        }
    }

    //SavePixels(MakeFileName("ShadeMainSurfaceRhomb", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, A1, A2, A3, A4) { E->Action = A; Execute(S, E, X, Y, A1, A2, A3, A4); if (!E->Result) { return; } }

VOID ShadeMainSurfaceRhomb(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    InitializePixelMasks(state);
    state->Actions.SetPixelColorMasks(0xF800, 0x7E0, 0x1F);

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 0, 0, 0, 0);

        EXECUTE("X: 0 Y: 0 A1: 45 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 45, 0, 0, 0);
        EXECUTE("X: 0 Y: 0 A1: 90 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 90, 0, 0, 0);
        EXECUTE("X: 0 Y: 0 A1: 180 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 180, 0, 0, 0);
        EXECUTE("X: 0 Y: 0 A1: 210 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 210, 0, 0, 0);
        EXECUTE("X: 0 Y: 0 A1: 270 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 270, 0, 0, 0);

        EXECUTE("X: 0 Y: 0 A1: 0 A2: 45 A3: 0 A4: 0", state, event, 0, 0, 0, 45, 0, 0);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 45 A4: 0", state, event, 0, 0, 0, 0, 45, 0);

        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 45", state, event, 0, 0, 0, 0, 0, 45);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 90", state, event, 0, 0, 0, 0, 0, 90);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 180", state, event, 0, 0, 0, 0, 0, 180);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 210", state, event, 0, 0, 0, 0, 0, 210);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 270", state, event, 0, 0, 0, 0, 0, 270);


        EXECUTE("X: 0 Y: 0 A1: 45 A2: 45 A3: 45 A4: 45", state, event, 0, 0, 45, 45, 45, 45);
        EXECUTE("X: 0 Y: 0 A1: 45 A2: 90 A3: 45 A4: 90", state, event, 0, 0, 45, 90, 45, 90);
    }

    // Finalize.
    InitializePixelMasks(state);
}