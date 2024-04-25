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
#include "OffsetSurfaces.hxx"

static VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event, S32 x, S32 y)
{
     //SavePixels(MakeFileName("OffsetSurfacesMain", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
     //SavePixels(MakeFileName("OffsetSurfacesBack", "bmp", event->Action), state->Surface.Back, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
     //SavePixels(MakeFileName("OffsetSurfacesStencil", "bmp", event->Action), state->Surface.Stencil, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y) { E->Action = A; Execute(S, E, X, Y); if (!E->Result) { return; } }


VOID OffsetSurfaces(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    Initialize(state);

    // Move from 0:0 to 225:173
    {
        state->Actions.OffsetSurfaces(225, 173);

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
            {
                state->Actions.DrawMainSurfaceColorRectangle(xx, yy, 1, 1, (PIXEL)(WHITE_PIXEL - (xx + xx * yy)));
            }
        }

        EXECUTE("X: 225 Y: 173", state, event, 225, 173);
    }

    // Move from 225:173 to 0:0
    {
        state->Actions.OffsetSurfaces(-225, -173);

        EXECUTE("X: 0 Y: 0", state, event, 225, 173);
    }
}