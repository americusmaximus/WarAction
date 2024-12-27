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
#include "WriteSurfaceSurfaceRectangle.hxx"

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

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 sx, S32 sy, S32 sstr, S32 dx, S32 dy, S32 dstr, S32 width, S32 height, S32 result, S32 count, RECTANGLEPTR rects)
{
    BOOL success = FALSE;

    PIXEL* source = AcquirePixels(sx, sy, width, height, sstr, WHITE_PIXEL);
    if (source == NULL) { event->Result = FALSE; return; }

    PIXEL* destination = AcquirePixels(dx, dy, width, height, dstr, BLACK_PIXEL);
    if (destination == NULL) { event->Result = FALSE; return; }

    state->Actions.WriteSurfaceSurfaceRectangle(sx, sy, sstr, source, dx, dy, dstr, destination, width, height);

    {
        U32 match = 0;

        for (S32 yy = 0; yy < dy + height; yy++)
        {
            for (S32 xx = 0; xx < dx + width; xx++)
            {
                if (destination[yy * dstr + xx] == WHITE_PIXEL) { match = match + 1; }
            }
        }

        success = match == result && IsInsideArea(destination, dx + width, dy + height, dstr, count, rects, WHITE_PIXEL);

        if (!success)
        {
            SavePixels(MakeFileName("WriteSurfaceSurfaceRectangle", "bmp", event->Action), destination, dx + width, dy + height, dstr);
        }
    }

    ReleasePixels(destination);
    ReleasePixels(source);

    event->Result = success;
}

#define EXECUTE(A, S, E, SX, SY, SSTR, DX, DY, DSTR, W, H, R, RC, RCT) { E->Action = A; Execute(S, E, SX, SY, SSTR, DX, DY, DSTR, W, H, R, RC, RCT); if (!E->Result) { return; } }

VOID WriteSurfaceSurfaceRectangle(RENDERERPTR state, MODULEEVENTPTR event)
{
    Initialize(state);

    {
        RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
        EXECUTE("SX: 0 SY: 0 SSTR: 2048 DX: 0 DY: 0 DSTR: 2048 W: 1024 H: 768 OX: 0 OY: 0",
            state, event, 0, 0, MAX_RENDERER_WIDTH * sizeof(PIXEL), 0, 0, MAX_RENDERER_WIDTH * sizeof(PIXEL),
            MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT, 1, &rect);
    }

    {
        RECTANGLE rect = { 199, 219, 153, 173 };
        EXECUTE("SX: 99 SY: 99 SSTR: 1024 DX: 199 DY: 219 DSTR: 1024 W: 153 H: 173 OX: 0 OY: 0",
            state, event, 99, 99, MAX_RENDERER_WIDTH, 199, 219, MAX_RENDERER_WIDTH, 153, 173, 153 * 173, 1, &rect);
    }

    {
        RECTANGLE rect = { 700, 219, 333, 227 };
        EXECUTE("SX: 99 SY: 99 SSTR: 1555 DX: 700 DY: 219 DSTR: 3618 W: 333 H: 227 OX: 0 OY: 0",
            state, event, 99, 99, 1555, 700, 219, 3618, 333, 227, 333 * 227, 1, &rect);
    }
}
