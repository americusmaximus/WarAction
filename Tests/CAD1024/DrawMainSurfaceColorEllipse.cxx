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
#include "DrawMainSurfaceColorEllipse.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 size, S32 step, S32 result)
{
    Initialize(state);

    state->Actions.DrawMainSurfaceColorEllipse(x, y, size, WHITE_PIXEL, step);

    U32 match = 0;
    PIXEL* pixels = state->Surface.Main;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; xx++) { if (pixels[xx] == WHITE_PIXEL) { match = match + 1; } }

    CONST BOOL success = match == result;

    if (!success)
    {
        SavePixels(MakeFileName("DrawMainSurfaceColorEllipse", "bmp", event->Action),
            pixels, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    }

    event->Result = success;
}

#define EXECUTE(A, S, E, X, Y, SZ, ST, R) { E->Action = A; Execute(S, E, X, Y, SZ, ST, R); if (!E->Result) { return; } }

VOID DrawMainSurfaceColorEllipse(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 100 Y: 100 S: 10 ST: 0 OX: 0 OY: 0", state, event, 100, 100, 10, 5, 52);
        EXECUTE("X: 100 Y: 100 S: 10 ST: 100 OX: 0 OY: 0", state, event, 100, 100, 10, 100, 52);
        EXECUTE("X: 100 Y: 100 S: 10 ST: 100000 OX: 0 OY: 0", state, event, 100, 100, 10, 100000, 37);

        EXECUTE("X: 100 Y: 100 S: 50 ST: 50 OX: 0 OY: 0", state, event, 100, 100, 50, 50, 225);
        EXECUTE("X: 100 Y: 100 S: 150 ST: -50 OX: 0 OY: 0", state, event, 100, 100, 150, -50, 537);
        EXECUTE("X: 100 Y: 100 S: 150 ST: 200000 OX: 0 OY: 0", state, event, 100, 100, 150, 200000, 187);
    }

    // Offset 0:0 to 75:100
    {
        state->Actions.OffsetSurfaces(75, 100);

        EXECUTE("X: 100 Y: 100 S: 10 ST: 0 OX: 75 OY: 100", state, event, 100, 100, 10, 5, 52);
        EXECUTE("X: 100 Y: 100 S: 10 ST: 100 OX: 75 OY: 100", state, event, 100, 100, 10, 100, 52);
        EXECUTE("X: 100 Y: 100 S: 10 ST: 100000 OX: 75 OY: 100", state, event, 100, 100, 10, 100000, 37);

        EXECUTE("X: 100 Y: 100 S: 50 ST: 50 OX: 75 OY: 100", state, event, 100, 100, 50, 50, 225);
        EXECUTE("X: 100 Y: 100 S: 150 ST: -50 OX: 75 OY: 100", state, event, 100, 100, 150, -50, 537);
        EXECUTE("X: 100 Y: 100 S: 150 ST: 200000 OX: 75 OY: 100", state, event, 100, 100, 150, 200000, 187);
    }

    // Offset 75:100 to -225:-25 (-300:-125)
    {
        state->Actions.OffsetSurfaces(-300, -125);

        EXECUTE("X: 100 Y: 100 S: 10 ST: 0 OX: -300 OY: -125", state, event, 100, 100, 10, 5, 52);
        EXECUTE("X: 100 Y: 100 S: 10 ST: 100 OX: -300 OY: -125", state, event, 100, 100, 10, 100, 52);
        EXECUTE("X: 100 Y: 100 S: 10 ST: 100000 OX: -300 OY: -125", state, event, 100, 100, 10, 100000, 37);

        EXECUTE("X: 100 Y: 100 S: 50 ST: 50 OX: -300 OY: -125", state, event, 100, 100, 50, 50, 225);
        EXECUTE("X: 100 Y: 100 S: 150 ST: -50 OX: -300 OY: -125", state, event, 100, 100, 150, -50, 537);
        EXECUTE("X: 100 Y: 100 S: 150 ST: 200000 OX: -300 OY: -125", state, event, 100, 100, 150, 200000, 187);
    }

    // Offset -225:-25 to 0:0 (225:25)
    {
        state->Actions.OffsetSurfaces(225, 25);
    }
}