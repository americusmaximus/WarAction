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
#include "DrawMainSurfaceVerticalColorLine.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

static BOOL IsInsideArea(PIXEL* pixels, U32 count, LPRECTANGLE rects, PIXEL pixel)
{
    U32 match = 0;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
    {
        for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
        {
            if (pixels[yy * MAX_RENDERER_WIDTH + xx] == pixel) { match = match + 1; }
        }
    }

    U32 sum = 0;

    BOOL mismatch = FALSE;

    for (U32 zz = 0; zz < count; zz++)
    {
        CONST U32 x = rects[zz].X;
        CONST U32 y = rects[zz].Y;
        CONST U32 w = rects[zz].Width;
        CONST U32 h = rects[zz].Height;

        sum = sum + w * h;

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
            {
                CONST U32 indx = yy * MAX_RENDERER_WIDTH + xx;

                if ((x <= xx && xx < (w + x)) && (y <= yy && yy < (h + y)))
                {
                    if (pixels[indx] != pixel) { mismatch = TRUE; break; }

                    pixels[indx] = BLACK_PIXEL;
                }
            }

            if (mismatch) { break; }
        }
    }

    return match == sum && !mismatch;
}

static VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event, S32 x, S32 y, S32 length, S32 result, U32 count, LPRECTANGLE rects)
{
    Initialize(state);

    state->Actions.DrawMainSurfaceVerticalColorLine(x, y, length, WHITE_PIXEL);

    U32 match = 0;
    PIXEL* pixels = state->Surface.Main;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; xx++) { if (pixels[xx] == WHITE_PIXEL) { match = match + 1; } }

    CONST BOOL success = match == result && IsInsideArea(pixels, count, rects, WHITE_PIXEL);

    if (!success)
    {
        SavePixels(MakeFileName("DrawMainSurfaceVerticalColorLine", "bmp", event->Action), pixels,
            MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    }

    event->Result = success;
}

#define EXECUTE(A, S, E, X, Y, L, R, RC, RCT) { E->Action = A; Execute(S, E, X, Y, L, R, RC, RCT); if (!E->Result) { return; } }

VOID DrawMainSurfaceVerticalColorLine(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        {
            RECTANGLE rect = { 0, 0, 1, 100 };
            EXECUTE("X: 0 Y: 0 L: 100 OX: 0 OY: 0", state, event, 0, 0, 100, 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 100, 100, 1, 100 };
            EXECUTE("X: 100 Y: 100 L: 100 OX: 0 OY: 0", state, event, 100, 100, 100, 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 1, 768 };
            EXECUTE("X: 0 Y: 0 L: 1024 OX: 0 OY: 0", state, event, 0, 0, 1024, 768, 1, &rect);
        }

        {
            RECTANGLE rect = { 50, 50, 1, 100 };
            EXECUTE("X: 50 Y: 50 L: 100 OX: 0 OY: 0", state, event, 50, 50, 100, 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 1000, 700, 1, 68 };
            EXECUTE("X: 1000 Y: 700 L: 100 OX: 0 OY: 0", state, event, 1000, 700, 100, 68, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -50 Y: -50 L: 100 OX: 0 OY: 0", state, event, -50, -50, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 L: 100 OX: 0 OY: 0", state, event, -150, -150, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 1000 Y: 700 L: -100 OX: 0 OY: 0", state, event, 1000, 700, -100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 2000 Y: 2000 L: 100 OX: 0 OY: 0", state, event, 2000, 2000, 100, 0, 1, &rect);
        }
    }

    // Offset 0:0 to 75:100
    {
        state->Actions.OffsetSurfaces(75, 100);

        {
            RECTANGLE rect = { 75, 100, 1, 100 };
            EXECUTE("X: 0 Y: 0 L: 100 OX: 75 OY: 100", state, event, 0, 0, 100, 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 175, 200, 1, 100 };
            EXECUTE("X: 100 Y: 100 L: 100 OX: 75 OY: 100", state, event, 100, 100, 100, 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 75, 0, 1, 768 };
            EXECUTE("X: 0 Y: 0 L: 1024 OX: 75 OY: 100", state, event, 0, 0, 1024, 768, 1, &rect);
        }

        {
            RECTANGLE rect = { 125, 150, 1, 100 };
            EXECUTE("X: 50 Y: 50 L: 100 OX: 75 OY: 100", state, event, 50, 50, 100, 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 51, 33, 1, 68 };
            EXECUTE("X: 1000 Y: 700 L: 100 OX: 75 OY: 100", state, event, 1000, 700, 100, 68, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -50 Y: -50 L: 100 OX: 75 OY: 100", state, event, -50, -50, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 L: 100 OX: 75 OY: 100", state, event, -150, -150, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 1000 Y: 700 L: -100 OX: 75 OY: 100", state, event, 1000, 700, -100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 2000 Y: 2000 L: 100 OX: 75 OY: 100", state, event, 2000, 2000, 100, 0, 1, &rect);
        }
    }

    // Offset 75:100 to 0:0 (-75:-100)
    {
        state->Actions.OffsetSurfaces(-75, -100);
    }
}