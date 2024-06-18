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
#include "DrawMainSurfaceColorBox.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

static BOOL IsInsideArea(PIXEL* pixels, U32 count, RECTANGLEPTR rects, PIXEL pixel)
{
    if (count == 0) { return TRUE; }

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

        if (count == 1) { sum = sum + w + (w - 2) + h + (h - 2); }
        else { sum = sum + w * h; }

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
            {
                CONST U32 indx = yy * MAX_RENDERER_WIDTH + xx;

                if (((x <= xx && xx < (w + x)) && (y == yy || yy == (h + y - 1)))
                    || (x == xx || xx == (w + x - 1)) && (y == yy && yy == (h + y)))
                {
                    if (pixels[indx] != pixel) { mismatch = TRUE; break; }

                    //pixels[indx] = BLACK_PIXEL;
                }
            }

            if (mismatch) { break; }
        }
    }

    return match == sum && !mismatch;
}

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 width, S32 height, S32 result, U32 count, RECTANGLEPTR rects)
{
    Initialize(state);

    state->Actions.DrawMainSurfaceColorBox(x, y, width, height, WHITE_PIXEL);

    U32 match = 0;
    PIXEL* pixels = state->Surface.Main;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; xx++) { if (pixels[xx] == WHITE_PIXEL) { match = match + 1; } }

    CONST BOOL success = match == result && IsInsideArea(pixels, count, rects, WHITE_PIXEL);

    if (!success)
    {
        SavePixels(MakeFileName("DrawMainSurfaceColorBox", "bmp", event->Action),
            pixels, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    }

    event->Result = success;
}

#define EXECUTE(A, S, E, X, Y, W, H, R, RC, RCT) { E->Action = A; Execute(S, E, X, Y, W, H, R, RC, RCT); if (!E->Result) { return; } }

VOID DrawMainSurfaceColorBox(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        // No pixels.
        {
            RECTANGLE rect = { 100, 99, 100, 2 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 0 OY: 0", state, event, 100, 100, 100, 0, 200, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect = { 0, 0, 100, 100 };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 0 OY: 0", state, event, 0, 0, 100, 100, 100 * 100 - 98 * 98, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 1024, 768 };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 0 OY: 0", state, event, 0, 0, 1024, 768, 1024 * 768 - 1022 * 766, 1, &rect);
        }

        {
            RECTANGLE rect = { 50, 50, 100, 100 };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 0 OY: 0", state, event, 50, 50, 100, 100, 100 * 100 - 98 * 98, 1, &rect);
        }

        // Overlap.
        {
            RECTANGLE rect[2] = { {1000, 700, 1, 67}, {1001, 700, 24, 1} };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 0 OY: 0", state, event, 1000, 700, 100, 100, 68 + 23, 2, rect);
        }

        {
            RECTANGLE rect[2] = { { 0, 49, 50, 1 }, {49, 0, 1, 49} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 0 OY: 0", state, event, -50, -50, 100, 100, 99, 2, rect);
        }

        // Negative dimensions.
        {
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 0 OY: 0", state, event, -150, -150, 100, 100, 0, 0, NULL);
        }

        {
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 0 OY: 0", state, event, 1000, 700, -100, -100, 0, 0, NULL);
        }

        // No overlap.
        {
            EXECUTE("X: 2000 Y: 2000 W: 100 H: 100 OX: 0 OY: 0", state, event, 2000, 2000, 100, 100, 0, 0, NULL);
        }
    }

    // Offset 0:0 to 75:100
    {
        state->Actions.OffsetSurfaces(75, 100);

        // No pixels.
        {
            RECTANGLE rect = {175, 199, 100, 2};
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 75 OY: 100", state, event, 100, 100, 100, 0, 200, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect = { 75, 100, 100, 100 };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 75 OY: 100", state, event, 0, 0, 100, 100, 100 * 100 - 98 * 98, 1, &rect);
        }

        {
            RECTANGLE rect[5] =
            {
                {0, 100, 74, 2},
                {74, 1, 1, 99},
                {75, 0, 1, 100},
                {74, 100, 2, 668},
                {76, 99, 948, 2}
            };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 75 OY: 100", state, event, 0, 0, 1024, 768, 3579, 5, rect);
        }

        {
            RECTANGLE rect = { 125, 150, 100, 100 };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 75 OY: 100", state, event, 50, 50, 100, 100, 100 * 100 - 98 * 98, 1, &rect);
        }

        // Overlap.
        {
            RECTANGLE rect[2] = { {51, 33, 1, 68}, {52, 33, 23, 1} };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 75 OY: 100", state, event, 1000, 700, 100, 100, 68 + 23, 2, rect);
        }

        {
            RECTANGLE rect[2] = { {75, 149, 50, 1}, {124, 100, 1, 49} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 75 OY: 100", state, event, -50, -50, 100, 100, 50 + 49, 2, rect);
        }

        // Negative dimensions.
        {
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 75 OY: 100", state, event, -150, -150, 100, 100, 0, 0, NULL);
        }

        {
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 75 OY: 100", state, event, 1000, 700, -100, -100, 0, 0, NULL);
        }

        // No overlap.
        {
            EXECUTE("X: 2000 Y: 2000 W: 100 H: 100 OX: 75 OY: 100", state, event, 2000, 2000, 100, 100, 0, 0, NULL);
        }
    }

    // Offset 75:100 to -225:-25 (-300:-125)
    {
        state->Actions.OffsetSurfaces(-300, -125);

        // No pixels.
        {
            RECTANGLE rect = { 899, 73, 100, 2 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: -225 OY: -25", state, event, 100, 100, 100, 0, 200, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect[6] =
            {
                {799, 0, 1, 73},
                {799, 73, 100, 1},
                {898, 0, 1, 73},
                {799, 742, 1, 26},
                {800, 742, 99, 1},
                {898, 743, 1, 25}
            };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: -225 OY: -25", state, event, 0, 0, 100, 100, 396, 6, rect);
        }

        {
            RECTANGLE rect[4] =
            {
                {798, 1, 1, 767},
                {799, 0, 1, 768},
                {800, 741, 224, 2},
                {0, 742, 798, 2}
            };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: -225 OY: -25", state, event, 0, 0, 1024, 768, 3579, 4, rect);
        }
    }

    // Offset -225:-25 to 0:0 (225:25)
    {
        state->Actions.OffsetSurfaces(225, 25);
    }
}