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
#include "DrawMainSurfaceColorRectangle.hxx"
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

static VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event, S32 x, S32 y, S32 width, S32 height, S32 result, U32 count, LPRECTANGLE rects)
{
    Initialize(state);

    state->Actions.DrawMainSurfaceColorRectangle(x, y, width, height, WHITE_PIXEL);

    U32 match = 0;
    PIXEL* pixels = state->Surface.Main;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; xx++) { if (pixels[xx] == WHITE_PIXEL) { match = match + 1; } }

    CONST BOOL success = match == result && IsInsideArea(pixels, count, rects, WHITE_PIXEL);

    if (!success)
    {
        SavePixels(MakeFileName("DrawMainSurfaceColorRectangle", "bmp", event->Action),
            pixels, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    }

    event->Result = success;
}

#define EXECUTE(A, S, E, X, Y, W, H, R, RC, RCT) { E->Action = A; Execute(S, E, X, Y, W, H, R, RC, RCT); if (!E->Result) { return; } }

VOID DrawMainSurfaceColorRectangle(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        // No pixels.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 0 OY: 0", state, event, 100, 100, 100, 0, 0, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect = { 0, 0, 100, 100 };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 0 OY: 0", state, event, 0, 0, 100, 100, 100 * 100, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 1024, 768 };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 0 OY: 0", state, event, 0, 0, 1024, 768, 1024 * 768, 1, &rect);
        }

        {
            RECTANGLE rect = { 50, 50, 100, 100 };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 0 OY: 0", state, event, 50, 50, 100, 100, 100 * 100, 1, &rect);
        }

        // Overlap.
        {
            RECTANGLE rect = { 1000, 700, 24, 68 };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 0 OY: 0", state, event, 1000, 700, 100, 100, 24 * 68, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 50, 50 };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 0 OY: 0", state, event, -50, -50, 100, 100, 50 * 50, 1, &rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 0 OY: 0", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 0 OY: 0", state, event, 1000, 700, -100, -100, 0, 1, &rect);
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
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 75 OY: 100", state, event, 100, 100, 100, 0, 0, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect = { 0 + 75, 0 + 100, 100, 100 };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 75 OY: 100", state, event, 0, 0, 100, 100, 100 * 100, 1, &rect);
        }

        {
            RECTANGLE rect[2] = { {0 + 75, 0 + 0, 1024 - 75, 1}, {0, 1, 1024, 767} }; // ???
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 75 OY: 100", state, event, 0, 0, 1024, 768, 1024 * 768 - 75 /* ??? */, 2, rect);
        }

        {
            RECTANGLE rect = { 50 + 75, 50 + 100, 100, 100 };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 75 OY: 100", state, event, 50, 50, 100, 100, 100 * 100, 1, &rect);
        }

        // Overlap.
        {
            RECTANGLE rect = { 1000 + 75 - 1024, 700 + 100 - 768 + 1, 24, 68 }; // ???
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 75 OY: 100", state, event, 1000, 700, 100, 100, 24 * 68, 1, &rect);
        }

        {
            RECTANGLE rect = { 75, 100, 50, 50 };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 75 OY: 100", state, event, -50, -50, 100, 100, 50 * 50, 1, &rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 75 OY: 100", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 75 OY: 100", state, event, 1000, 700, -100, -100, 0, 1, &rect);
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
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: -225 OY: -25", state, event, 100, 100, 100, 0, 0, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect[2] = { {75 - 300 + 1024, 0, 100, 74}, {75 - 300 + 1024, 742, 100, 26} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: -225 OY: -25", state, event, 0, 0, 100, 100, 100 * 100, 2, rect);
        }

        {
            RECTANGLE rect[2] = { { 799, 0, 225, 1 }, {0, 1, 1024, 767} };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: -225 OY: -25", state, event, 0, 0, 1024, 768, 1024 * 767 + 225, 2, rect);
        }

        {
            RECTANGLE rect = {849, 24, 100, 100};
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: -225 OY: -25", state, event, 50, 50, 100, 100, 100 * 100, 1, &rect);
        }

        // Overlap.
        {
            RECTANGLE rect = { 775, 675, 24, 68 };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: -225 OY: -25", state, event, 1000, 700, 100, 100, 24 * 68, 1, &rect);
        }

        {
            RECTANGLE rect[2] = { {799, 0, 50, 24}, {799, 742, 50, 26} };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: -225 OY: -25", state, event, -50, -50, 100, 100, 50 * 50, 2, rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: -225 OY: -25", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: -225 OY: -25", state, event, 1000, 700, -100, -100, 0, 1, &rect);
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
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 100 Y: 100 W: 100 H: 0 OX: 740 OY: 647", state, event, 100, 100, 100, 0, 0, 1, &rect);
        }

        // Within the area.
        {
            RECTANGLE rect = {740, 647, 100, 100};
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: 740 OY: 647", state, event, 0, 0, 100, 100, 100 * 100, 1, &rect);
        }

        {
            RECTANGLE rect[2] = { {0, 1, 1024, 767}, {740, 0, 284, 1} };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 OX: 740 OY: 647", state, event, 0, 0, 1024, 768, 1024 * 767 + 284 /* ??? */, 2, rect);
        }

        {
            RECTANGLE rect[2] = { {790, 0, 100, 29}, {790, 697, 100, 71} };
            EXECUTE("X: 50 Y: 50 W: 100 H: 100 OX: 740 OY: 647", state, event, 50, 50, 100, 100, 100 * 100, 2, rect);
        }

        // Overlap.
        {
            RECTANGLE rect = { 716, 580, 24, 68 };
            EXECUTE("X: 1000 Y: 700 W: 100 H: 100 OX: 740 OY: 647", state, event, 1000, 700, 100, 100, 24 * 68, 1, &rect);
        }

        {
            RECTANGLE rect = { 740, 647, 50, 50 };
            EXECUTE("X: -50 Y: -50 W: 100 H: 100 OX: 740 OY: 647", state, event, -50, -50, 100, 100, 50 * 50, 1, &rect);
        }

        // Negative dimensions.
        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: -150 Y: -150 W: 100 H: 100 OX: 740 OY: 647", state, event, -150, -150, 100, 100, 0, 1, &rect);
        }

        {
            RECTANGLE rect = { 0, 0, 0, 0 };
            EXECUTE("X: 1000 Y: 700 W: -100 H: -100 OX: 740 OY: 647", state, event, 1000, 700, -100, -100, 0, 1, &rect);
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
            RECTANGLE rect[4] = { {0, 1, 52, 81}, {0, 750, 52, 18}, {976, 0, 48, 81}, {976, 749, 48, 19} };
            EXECUTE("X: 0 Y: 0 W: 100 H: 100 OX: -48 OY: -18", state, event, 0, 0, 100, 100, 9948 /* ??? */, 4, rect);
        }
    }

    // Offset -48:-18 to 0:0 (48:18)
    {
        state->Actions.OffsetSurfaces(48, 18);
    }
}