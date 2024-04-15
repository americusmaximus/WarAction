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

#include "Initialize.hxx"
#include "WriteBackSurfaceMainSurfaceRectangle.hxx"

static VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    Initialize(state);

    for (U32 x = 0; x < MAX_RENDERER_WIDTH; x = x + 10)
    {
        for (U32 y = 0; y < MAX_RENDERER_HEIGHT; y = y + 10)
        {
            state->Actions.DrawBackSurfaceColorPoint(x, y, WHITE_PIXEL);
        }
    }

    state->Actions.WriteBackSurfaceMainSurfaceRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    for (U32 y = 0; y < MAX_RENDERER_HEIGHT; y++)
    {
        for (U32 x = 0; x < MAX_RENDERER_WIDTH; x++)
        {
            CONST U32 indx = y * MAX_RENDERER_WIDTH + x;
            if (state->Surface.Back[indx] != state->Surface.Main[indx]) { event->Result = FALSE; return; }
        }
    }

    event->Result = TRUE;
}

#define EXECUTE(A, S, E) { E->Action = A; Execute(S, E); if (!E->Result) { return; } }

VOID WriteBackSurfaceMainSurfaceRectangle(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    // Initialize.
    Initialize(state);

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("OX: 0 OY: 0", state, event);
    }

    // Offset from 0:0 to (131:-97)
    {
        state->Actions.OffsetSurfaces(131, -97);

        EXECUTE("OX: 131 OY: -97", state, event);
    }

    // Offset back to 0:0 (-131, 97)
    {
        state->Actions.OffsetSurfaces(-131, 97);
    }

    // Finalize.
    Initialize(state);
}