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
#include "DrawMainSurfaceColorOutline.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

static BOOL IsInsideArea(PIXEL* pixels, U32 count, RECTANGLEPTR rects, PIXEL pixel)
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

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 width, S32 height, S32 result, U32 count, RECTANGLEPTR rects)
{
    Initialize(state);

    state->Actions.DrawMainSurfaceColorOutline(x, y, width, height, WHITE_PIXEL);

    U32 match = 0;
    PIXEL* pixels = state->Surface.Main;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; xx++) { if (pixels[xx] == WHITE_PIXEL) { match = match + 1; } }

    CONST BOOL success = match == result && IsInsideArea(pixels, count, rects, WHITE_PIXEL);

    if (!success)
    {
        SavePixels(MakeFileName("DrawMainSurfaceColorOutline", "bmp", event->Action),
            pixels, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    }

    event->Result = success;
}

#define EXECUTE(A, S, E, X, Y, W, H, R, RC, RCT) { E->Action = A; Execute(S, E, X, Y, W, H, R, RC, RCT); if (!E->Result) { return; } }

VOID DrawMainSurfaceColorOutline(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        // No pixels.
        {
            RECTANGLE rect = { 100, 100, 100, 2 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 0 OY: 0", state, event, 100, 100, 100, 0, 100 * 2, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect[3] = { {1, 0, 98, 1}, {99, 0, 1, 100}, {1, 99, 98, 1} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 0 OY: 0", state, event, 0, 0, 100, 100, 100 + 2 * 98, 3, rect);
        }

        {
            RECTANGLE rect[3] = { {1, 0, 1023, 1}, {1023, 1, 1, 766}, {1, 1023, 1023, 1} };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 0 OY: 0", state, event, 0, 0, 1024, 768, 2 * 1023 + 766, 3, rect);
        }

        {
            RECTANGLE rect[4] = { {50, 50, 100, 1}, {50, 51, 1, 98}, {50, 149, 100, 1}, {149, 51, 1, 98} };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 0 OY: 0", state, event, 50, 50, 100, 100, 2 * 100 + 2 * 98, 4, rect);
        }

        // Overlap.
        {
            RECTANGLE rect[3] = { {1000, 700, 24, 1}, {1000, 701, 1, 67}, {0, 701, 1, 1} };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 0 OY: 0", state, event, 1000, 700, 100, 100, 24 + 67 + 1, 3, rect);
        }

        {
            RECTANGLE rect[2] = { {1, 49, 48, 1}, {49, 0, 1, 50} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 0 OY: 0", state, event, -50, -50, 100, 100, 48 + 50, 2, rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 0 OY: 0", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect[4] = { {901, 601, 1, 100}, {902, 601, 98, 1}, {1000, 601, 1, 100}, {902, 700, 98, 1} };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 0 OY: 0", state, event, 1000, 700, -100, -100, 2 * 100 + 2 * 98, 4, rect);
        }

        // No overlap.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 2000 Y: 2000 W: 100 H: 100 OX: 0 OY: 0", state, event, 2000, 2000, 100, 100, 0, 1, &rect);
        }
    }

    // Offset 0:0 to 75:100
    {
        state->Actions.OffsetSurfaces(75, 100);

        // No pixels.
        {
            RECTANGLE rect = { 175, 200, 100, 2 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 75 OY: 100", state, event, 100, 100, 100, 0, 2 * 100, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect[3] = { {76, 100, 99, 1}, {174, 101, 1, 98}, {76, 199, 99, 1} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 75 OY: 100", state, event, 0, 0, 100, 100, 99 + 98 + 99, 3, rect);
        }

        {
            RECTANGLE rect[3] = { {0, 100, 74, 2}, { 74, 1, 1, 767}, {76, 99, 948, 2} };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 75 OY: 100", state, event, 0, 0, 1024, 768, 74 * 2 + 767 + 948 * 2, 3, rect);
        }

        {
            RECTANGLE rect[4] = { {125, 150, 100, 1}, {125, 151, 1, 98}, {125, 249, 100, 1}, {224, 151, 1, 98} };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 75 OY: 100", state, event, 50, 50, 100, 100, 2 * 100 + 2 * 98, 4, rect);
        }

        // Overlap.
        {
            RECTANGLE rect[2] = { {51, 33, 1, 68}, {52, 33, 24, 1} };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 75 OY: 100", state, event, 1000, 700, 100, 100, 24 + 68, 2, rect);
        }

        {
            RECTANGLE rect[2] = { {76, 149, 49, 1}, {124, 100, 1, 49} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 75 OY: 100", state, event, -50, -50, 100, 100, 49 + 49, 2, rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 75 OY: 100", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect[2] = {{51, 33, 1, 68}, {52, 33, 24, 1}};
            EXECUTE("X: 1000 Y: 700 W: 157 H: 263 OX: 75 OY: 100", state, event, 1000, 700, 157, 263, 68 + 24, 2, rect);
        }

        // No overlap.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 2000 Y: 2000 W: 100 H: 100 OX: 75 OY: 100", state, event, 2000, 2000, 100, 100, 0, 1, &rect);
        }
    }

    // Offset 75:100 to -225:-25 (-300:-125)
    {
        state->Actions.OffsetSurfaces(-300, -125);

        // No pixels.
        {
            RECTANGLE rect = { 899, 74, 100, 2 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: -225 OY: -25", state, event, 100, 100, 100, 0, 100 * 2, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect[4] = { {800, 73, 99, 1}, {898, 0, 1, 73}, {800, 742, 99, 1}, {898, 743, 1, 25} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: -225 OY: -25", state, event, 0, 0, 100, 100, 99 + 73 + 99 + 25, 4, rect);
        }

        {
            RECTANGLE rect[3] = { {0, 742, 798, 2}, {798, 1, 1, 767}, {800, 741, 224, 2} };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: -225 OY: -25", state, event, 0, 0, 1024, 768, 2 * 798 + 767 + 2 * 224, 3, rect);
        }

        {
            RECTANGLE rect[4] = { {849, 24, 100, 1}, {849, 25, 1, 98}, {849, 123, 100, 1}, {948, 25, 1, 98} };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: -225 OY: -25", state, event, 50, 50, 100, 100, 2 * 100 + 2 * 98, 4, rect);
        }

        // Overlap.
        {
            RECTANGLE rect[2] = { {775, 675, 1, 68}, {776, 675, 24, 1} };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: -225 OY: -25", state, event, 1000, 700, 100, 100, 68 + 24, 2, rect);
        }

        {
            RECTANGLE rect[3] = { {800, 23, 49, 1}, {848, 0, 1, 23}, {848, 742, 1, 26} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: -225 OY: -25", state, event, -50, -50, 100, 100, 49 + 23 + 26 , 3, rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: -225 OY: -25", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect[4] = { {676, 576, 100, 1}, {676, 577, 1, 98},{676, 675, 100, 1}, {775, 577, 1, 98} };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: -225 OY: -25", state, event, 1000, 700, -100, -100, 100 * 2 + 98 * 2, 4, rect);
        }

        // No overlap.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 2000 Y: 2000 W: 100 H: 100 OX: -225 OY: -25", state, event, 2000, 2000, 100, 100, 0, 1, &rect);
        }
    }

    // Offset -225:-25 to 740:647 (965:672)
    {
        state->Actions.OffsetSurfaces(965, 672);

        // No pixels.
        {
            RECTANGLE rect = { 840, 747, 100, 2 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 740 OY: 647", state, event, 100, 100, 100, 0, 100 * 2, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect[3] = { {741, 647, 99, 1}, {839, 648, 1, 99}, {741, 746, 98, 1} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 740 OY: 647", state, event, 0, 0, 100, 100, 99 + 99 + 98, 3, rect);
        }

        {
            RECTANGLE rect[3] = { {0, 647, 739, 2}, {739, 1, 1, 767}, {741, 646, 283, 2} };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 740 OY: 647", state, event, 0, 0, 1024, 768, 2 * 739 + 767 + 2 * 283, 3, rect);
        }

        {
            RECTANGLE rect[6] = { {790, 0, 1, 29}, {791, 28, 99, 1}, {889, 0, 1, 28}, {790, 697, 1, 71}, {791, 697, 99, 1}, {889, 698, 1, 70} };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 740 OY: 647", state, event, 50, 50, 100, 100, 29 + 99 + 28 + 71 + 99 + 70, 6, rect);
        }

        // Overlap.
        {
            RECTANGLE rect[2] = { {716, 580, 1, 68}, {717, 580, 24, 1} };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 740 OY: 647", state, event, 1000, 700, 100, 100, 24 + 68, 2, rect);
        }

        {
            RECTANGLE rect[2] = { {741, 696, 49, 1}, {789, 647, 1, 49} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 740 OY: 647", state, event, -50, -50, 100, 100, 49 + 49, 2, rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 740 OY: 647", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect[4] = { {617, 481, 100, 1}, {617, 482, 1, 98}, {617, 580, 100, 1}, {716, 482, 1, 98} };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 740 OY: 647", state, event, 1000, 700, -100, -100, 2 * 100 + 2 * 98, 4, rect);
        }

        // No overlap.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 2000 Y: 2000 W: 100 H: 100 OX: 740 OY: 647", state, event, 2000, 2000, 100, 100, 0, 1, &rect);
        }
    }

    // Offset 740:647 to -48:-18 (-788:-665)
    {
        state->Actions.OffsetSurfaces(-788, -665);

        {
            RECTANGLE rect[6] = { {0, 81, 52, 1}, {51, 1, 1, 80}, {977, 80, 47, 1}, {977, 749, 47, 1}, {0, 750, 52, 1}, {51, 751, 1, 17} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: -48 OY: -18", state, event, 0, 0, 100, 100, 52 + 80 + 47 + 47 + 52 + 17, 6, rect);
        }
    }

    // Offset -48:-18 to 0:0 (48:18)
    {
        state->Actions.OffsetSurfaces(48, 18);
    }
}