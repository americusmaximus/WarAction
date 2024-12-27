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
#include "FilePath.hxx"
#include "Initialize.hxx"
#include "ReadMainSurfaceSurfaceRectangle.hxx"

static PIXEL* AcquirePixels(S32 x, S32 y, S32 width, S32 height, S32 stride, PIXEL pixel)
{
    CONST U32 min = MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL);
    CONST U32 size = 2 * stride * (y + height) * sizeof(PIXEL);

    PIXEL* result = (PIXEL*)malloc(min < size ? size : min);

    if (result == NULL) { return result; }

    ZeroMemory(result, size);

    for (S32 yy = y; yy < (height + y); yy++)
    {
        for (S32 xx = x; xx < (width + x); xx++)
        {
            result[yy * stride + xx] = pixel;
        }
    }

    return result;
}

static VOID ReleasePixels(PIXEL* pixels)
{
    if (pixels != NULL) { free(pixels); }
}

static BOOL IsInsideArea(PIXEL* pixels, S32 width, S32 height, S32 stride, U32 count, RECTANGLEPTR rects, PIXEL pixel)
{
    U32 match = 0;

    for (S32 xx = 0; xx < width; xx++)
    {
        for (S32 yy = 0; yy < height; yy++)
        {
            if (pixels[yy * stride + xx] == pixel) { match = match + 1; }
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

        for (S32 xx = 0; xx < width; xx++)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                CONST U32 indx = yy * stride + xx;

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

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, S32 result, S32 count, RECTANGLEPTR rects)
{
    Initialize(state);

    BOOL success = FALSE;

    PIXEL* destination = AcquirePixels(dx, dy, width, height, stride, BLACK_PIXEL);
    if (destination == NULL) { event->Result = FALSE; return; }

    state->Actions.DrawMainSurfaceColorRectangle(sx, sy, width, height, WHITE_PIXEL);
    state->Actions.ReadMainSurfaceSurfaceRectangle(sx, sy, width, height, dx, dy, stride, destination);

    {
        U32 match = 0;

        for (S32 yy = 0; yy < dy + height; yy++)
        {
            for (S32 xx = 0; xx < dx + width; xx++)
            {
                if (destination[yy * stride + xx] == WHITE_PIXEL) { match = match + 1; }
            }
        }

        success = match == result && IsInsideArea(destination, dx + width, dy + height, stride, count, rects, WHITE_PIXEL);

        if (!success)
        {
            SavePixels(MakeFileName("ReadMainSurfaceSurfaceRectangle", "bmp", event->Action), destination, dx + width, dy + height, stride);
        }
    }

    ReleasePixels(destination);

    event->Result = success;
}

#define EXECUTE(A, S, E, SX, SY, W, H, DX, DY, STR, R, RC, RECT) { E->Action = A; Execute(S, E, SX, SY, W, H, DX, DY, STR, R, RC, RECT); if (!E->Result) { return; } }

VOID ReadMainSurfaceSurfaceRectangle(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    Initialize(state);

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        RECTANGLE rect = { 63, 49, 431, 117 };
        EXECUTE("SX: 105 SY: 99 W: 431 H: 117 DX: 63: DY: 49: STR: 1561 OX: 0 OY: 0",
            state, event, 105, 99, 431, 117, 63, 49, 1561, 431 * 117, 1, &rect);
    }

    // Offset from 0:0 to 131:-97 (131:-97)
    {
        state->Actions.OffsetSurfaces(131, -97);

        RECTANGLE rect = { 63, 49, 431, 117 };
        EXECUTE("SX: 105 SY: 99 W: 431 H: 117 DX: 63: DY: 49: STR: 1561 OX: 131 OY: -97",
            state, event, 105, 99, 431, 117, 63, 49, 1561, 431 * 117, 1, &rect);
    }

    // Offset from 131:-97 to 251:453 (120:550)
    {
        state->Actions.OffsetSurfaces(120, 550);

        RECTANGLE rect = { 63, 49, 431, 117 };
        EXECUTE("SX: 105 SY: 99 W: 431 H: 117 DX: 63: DY: 49: STR: 1561 OX: 251 OY: 453",
            state, event, 105, 99, 431, 117, 63, 49, 1561, 431 * 117, 1, &rect);
    }

    // Offset back to 0:0 (-120, -550)
    {
        state->Actions.OffsetSurfaces(-120, -550);
    }

    // Finalize.
    Initialize(state);
}