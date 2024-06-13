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
#include "MaskStencilSurfaceRectangle.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

static VOID Init(RENDERERMODULESTATECONTAINERPTR state)
{
    Initialize(state);

    for (S32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
    {
        for (S32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            state->Surface.Stencil[yy * MAX_RENDERER_HEIGHT + xx] = WHITE_PIXEL;
        }
    }
}

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 width, S32 height)
{
    Init(state);

    state->Actions.MaskStencilSurfaceRectangle(x, y, width, height);

    //SavePixels(MakeFileName("MaskStencilSurfaceRectangle", "bmp", event->Action), state->Surface.Stencil, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, OX, OY) { E->Action = A; Execute(S, E, X, Y, OX, OY); if (!E->Result) { return; } }

VOID MaskStencilSurfaceRectangle(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 123 Y: 277 OX: W: 100 H: 300 OX: 0 OY: 0", state, event, 123, 277, 100, 300);
        EXECUTE("X: 423 Y: 345 OX: W: 100 H: 111 OX: 0 OY: 0", state, event, 423, 345, 100, 111);
    }

    // Offset 0:0 to 123:516
    {
        state->Actions.OffsetSurfaces(123, 516);

        EXECUTE("X: 123 Y: 277 W: 123 H: 516 OX: 123 OY: 516", state, event, 123, 277, 123, 516);
        EXECUTE("X: 423 Y: 345 W: 123 H: 516 OX: 123 OY: 516", state, event, 423, 345, 123, 516);
    }

    // Offset 123:516 to -77:-99 (-200, -615)
    {
        state->Actions.OffsetSurfaces(-200, -615);

        EXECUTE("X: 123 Y: 277 W: 123 H: 516 OX: -77 OY: -99", state, event, 123, 277, 123, 516);
        EXECUTE("X: 423 Y: 345 W: 222 H: 341 OX: -77 OY: -99", state, event, 423, 345, 222, 341);
    }

    // Offset -77:-99 to -101:-101 (-24, -615)
    {
        state->Actions.OffsetSurfaces(-24, -2);

        EXECUTE("X: 123 Y: 277 W: 123 H: 516 OX: -101 OY: -101", state, event, 123, 277, 123, 516);
        EXECUTE("X: 423 Y: 345 W: 222 H: 341 OX: -101 OY: -101", state, event, 423, 345, 222, 341);
    }

    // Offset -101:-101 to 0:0 (101:101)
    {
        state->Actions.OffsetSurfaces(101, 101);
    }
}