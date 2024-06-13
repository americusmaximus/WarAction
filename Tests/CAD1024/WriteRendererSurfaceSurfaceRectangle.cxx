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
#include "WriteRendererSurfaceSurfaceRectangle.hxx"

static VOID Init(RENDERERMODULESTATECONTAINERPTR state)
{
    Initialize(state);

    if (state->Surface.Renderer != NULL) { ZeroMemory(state->Surface.Renderer, MAX_RENDERER_HEIGHT * state->Pitch * sizeof(PIXEL)); }
}

static PIXEL* AcquirePixels(S32 x, S32 y, S32 width, S32 height, S32 stride, PIXEL pixel)
{
    CONST U32 size = 2 * stride * (x + height) * sizeof(PIXEL);

    PIXEL* result = (PIXEL*)malloc(size);

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

static BOOL IsInsideArea(PIXEL* pixels, U32 stride, U32 count, RECTANGLEPTR rects, PIXEL pixel)
{
    U32 match = 0;

    for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
    {
        for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
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

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
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

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 width, S32 height, S32 dx, S32 dy, S32 stride, S32 result, S32 count, RECTANGLEPTR rects)
{
#if ACTIVE_TRUE_COLOR_MODE
    event->Result = TRUE;
#else
    BOOL success = FALSE;
    PIXEL* pixels = AcquirePixels(x, y, width, height, stride, WHITE_PIXEL);

    if (pixels == NULL) { event->Result = FALSE; return; }

    state->Actions.WriteRendererSurfaceSurfaceRectangle(x, y, width, height, dx, dy, stride, pixels);

    {
        U32 match = 0;
        PIXEL* surface = (PIXEL*)state->Surface.Renderer;
        U32 pitch = state->Pitch / sizeof(PIXEL);

        for (U32 xx = 0; xx < pitch * MAX_RENDERER_HEIGHT; xx++) { if (surface[xx] == WHITE_PIXEL) { match = match + 1; } }

        success = match == result && IsInsideArea(surface, pitch, count, rects, WHITE_PIXEL);

        if (!success)
        {
            SavePixels(MakeFileName("WriteRendererSurfaceSurfaceRectangle", "bmp", event->Action),
                surface, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, pitch);
        }
    }

    ReleasePixels(pixels);

    event->Result = success;
#endif
}

#define EXECUTE(A, S, E, X, Y, W, H, DX, DY, STRIDE, R, RC, RCT) { E->Action = A; Execute(S, E, X, Y, W, H, DX, DY, STRIDE, R, RC, RCT); if (!E->Result) { return; } }

VOID WriteRendererSurfaceSurfaceRectangle(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    HWND hwnd = InitializeWindow();

    state->Actions.Initialize();
    state->Actions.InitializeDirectX(hwnd, FALSE);
    state->Actions.InitializeWindow(MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    state->Actions.LockRendererSurface();

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        {
            Init(state);

            RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 DX: 0 DY: 0 STRIDE: 1024 OX: 0 OY: 0",
                state, event, 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, 0, 0, MAX_RENDERER_WIDTH,
                MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT, 1, &rect);
        }

        {
            Init(state);

            RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
            EXECUTE("X: 0 Y: 0 W: 1024 H: 768 DX: 0 DY: 0 STRIDE: 2000 OX: 0 OY: 0",
                state, event, 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, 0, 0, 2000,
                MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT, 1, &rect);
        }

        {
            Init(state);

            RECTANGLE rect = { 63, 75, 252, 321 };
            EXECUTE("X: 100 Y: 178 W: 253 H: 321 DX: 63 DY: 75 STRIDE: 2048 OX: 0 OY: 0",
                state, event, 100, 178, 253, 321, 63, 75, 2048, 252 * 321, 1, &rect);
        }
    }

    // Offset from 0:0 to (131:-97)
    {
        state->Actions.OffsetSurfaces(131, -97);

        {
            Init(state);

            RECTANGLE rect = { 63, 175, 252, 321 };
            EXECUTE("X: 100 Y: 178 W: 253 H: 321 DX: 63 DY: 175 STRIDE: 1526 OX: 131 OY: -97",
                state, event, 100, 178, 253, 321, 63, 175, 1526, 252 * 321, 1, &rect);
        }

        {
            Init(state);

            RECTANGLE rect = { 63, 175, 252, 327 };
            EXECUTE("X: 100 Y: 178 W: 253 H: 327 DX: 63 DY: 175 STRIDE: 1532 OX: 131 OY: -97",
                state, event, 100, 178, 253, 327, 63, 175, 1532, 252 * 327, 1, &rect);
        }
    }

    // Offset back to 0:0 (-131, 97)
    {
        state->Actions.OffsetSurfaces(-131, 97);

        {
            Init(state);

            RECTANGLE rect = { 1000, 705, 24, 63 };
            EXECUTE("X: 1000 Y: 578 W: 253 H: 321 DX: 1000 DY: 705 STRIDE: 3000 OX: 0 OY: 0",
                state, event, 1000, 578, 253, 321, 1000, 705, 3000, 252 * 63, 1, &rect);
        }
    }

    // Finalize.
    state->Actions.UnlockRendererSurface();
    state->Actions.RestoreDisplayMode();

    ReleaseWindow(hwnd);
}
