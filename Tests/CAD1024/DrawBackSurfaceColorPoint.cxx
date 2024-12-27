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

#include "BitMap.hxx"
#include "DrawBackSurfaceColorPoint.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 result, S32 rx, S32 ry)
{
    Initialize(state);

    state->Actions.DrawBackSurfaceColorPoint(x, y, WHITE_PIXEL);

    U32 match = 0;
    PIXEL* pixels = state->Surface.Back;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; xx++) { if (pixels[xx] == WHITE_PIXEL) { match = match + 1; } }

    CONST BOOL success = match == result && (result == 0 || state->Surface.Back[ry * MAX_RENDERER_WIDTH + rx] == WHITE_PIXEL);

    if (!success)
    {
        SavePixels(MakeFileName("DrawBackSurfaceColorPoint", "bmp", event->Action),
            pixels, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    }

    event->Result = success;
}

#define EXECUTE(A, S, E, X, Y, R, RX, RY) { E->Action = A; Execute(S, E, X, Y, R, RX, RY); if (!E->Result) { return; } }

VOID DrawBackSurfaceColorPoint(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 0 Y: 0 OX: 0 OY: 0", state, event, 0, 0, 1, 0, 0);
        EXECUTE("X: 100 Y: 100 OX: 0 OY: 0", state, event, 100, 100, 1, 100, 100);
        EXECUTE("X: 1200 Y: 100 OX: 0 OY: 0", state, event, 1200, 100, 0, 0, 0);
        EXECUTE("X: 100 Y: 1200 OX: 0 OY: 0", state, event, 1000, 1200, 0, 0, 0);
        EXECUTE("X: -100 Y: -1200 OX: 0 OY: 0", state, event, -100, -1200, 0, 0, 0);
    }

    // Offset 0:0 to 171:29 (171:29)
    {
        state->Actions.OffsetSurfaces(171, 29);

        EXECUTE("X: 0 Y: 0 OX: 171 OY: 29", state, event, 0, 0, 1, 0 + 171, 0 + 29);
        EXECUTE("X: 100 Y: 100 OX: 171 OY: 29", state, event, 100, 100, 1, 100 + 171, 100 + 29);
        EXECUTE("X: 1200 Y: 100 OX: 171 OY: 29", state, event, 1200, 100, 0, 0, 0);
        EXECUTE("X: 100 Y: 1200 OX: 171 OY: 29", state, event, 1000, 1200, 0, 0, 0);
        EXECUTE("X: -100 Y: -1200 OX: 171 OY: 29", state, event, -100, -1200, 0, 0, 0);
        EXECUTE("X: 1000 Y: 750 OX: 171 OY: 29", state, event, 1000, 750, 1, 147, 12);
        EXECUTE("X: 318 Y: 41 OX: 171 OY: 29", state, event, 318, 41, 1, 489, 70);
    }

    // Offset 171:29 to -48:-18 (-219:-47)
    {
        state->Actions.OffsetSurfaces(-219, -47);

        EXECUTE("X: 0 Y: 0 OX: -219 OY: -18", state, event, 0, 0, 1, 976, 749);
        EXECUTE("X: 100 Y: 100 OX: -219 OY: -18", state, event, 100, 100, 1, 52, 82);
        EXECUTE("X: 1200 Y: 100 OX: -219 OY: -18", state, event, 1200, 100, 0, 0, 0);
        EXECUTE("X: 100 Y: 1200 OX: -219 OY: -18", state, event, 1000, 1200, 0, 0, 0);
        EXECUTE("X: -100 Y: -1200 OX: -219 OY: -18", state, event, -100, -1200, 0, 0, 0);
        EXECUTE("X: 1000 Y: 750 OX: -219 OY: -18", state, event, 1000, 750, 1, 952, 732);
        EXECUTE("X: 318 Y: 41 OX: -219 OY: -18", state, event, 318, 41, 1, 270, 23);
    }

    // Offset -48:-18 to 0:0 (48:18)
    {
        state->Actions.OffsetSurfaces(48, 18);
    }
}