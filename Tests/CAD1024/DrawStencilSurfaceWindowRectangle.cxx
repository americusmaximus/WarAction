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

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 ox, S32 oy)
{
    Initialize(state);

    state->Actions.DrawStencilSurfaceWindowRectangle();

    //SavePixels(MakeFileName("DrawStencilSurfaceWindowRectangle", "bmp", event->Action), state->Surface.Stencil, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, OX, OY) { E->Action = A; Execute(S, E, X, Y, OX, OY); if (!E->Result) { return; } }

VOID DrawStencilSurfaceWindowRectangle(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);
        
        EXECUTE("X: 123 Y: 277 OX: 0 OY: 0", state, event, 123, 277, 0, 0);
        EXECUTE("X: 423 Y: 345 OX: 0 OY: 0", state, event, 423, 345, 0, 0);
    }

    // Offset 0:0 to 123:516
    {
        state->Actions.OffsetSurfaces(123, 516);

        EXECUTE("X: 123 Y: 277 OX: 123 OY: 516", state, event, 123, 277, 123, 516);
        EXECUTE("X: 423 Y: 345 OX: 123 OY: 516", state, event, 423, 345, 123, 516);
    }

    // Offset 123:516 to 0:0 (-123:-516)
    {
        state->Actions.OffsetSurfaces(-123, -516);
    }
}