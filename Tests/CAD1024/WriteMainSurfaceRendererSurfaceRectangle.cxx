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
#include "WriteMainSurfaceRendererSurfaceRectangle.hxx"

static VOID Init(LPRENDERERMODULESTATECONTAINER state)
{
    Initialize(state);

    if (state->Surface.Renderer != NULL)
    {
        ZeroMemory(state->Surface.Renderer, MAX_RENDERER_HEIGHT * state->Pitch * sizeof(PIXEL));
    }
}

VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event, S32 x, S32 y, S32 width, S32 height, S32 count, LPRECTANGLE rect)
{
    //SavePixels(MakeFileName("WriteMainSurfaceRendererSurfaceRectangle", "bmp", event->Action), state->Surface.Renderer, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, state->Pitch / sizeof(PIXEL));

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, W, H, RC, RCT) { E->Action = A; Execute(S, E, X, Y, W, H, RC, RCT); if (!E->Result) { return; } }

VOID WriteMainSurfaceRendererSurfaceRectangle(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    // Initialize.
    HWND hwnd = InitializeWindow();

    state->Actions.InitializeRenderer();
    state->Actions.InitializeDirectDraw(hwnd, FALSE);
    state->Actions.InitializeWindow(MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    state->Actions.LockRendererSurface();

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        // White at 0:0
        {
            Init(state);

            state->Actions.DrawMainSurfaceColorRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, WHITE_PIXEL);
            state->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

            RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
            EXECUTE("W X: 0 Y: 0 W: 1024 H: 768 OX: 0 OY: 0", state, event, 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, 1, &rect);
        }

        // Colors at 0:0
        {
            Init(state);

            for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
            {
                for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
                {
                    state->Actions.DrawMainSurfaceColorRectangle(xx, yy, 1, 1, (PIXEL)(WHITE_PIXEL - (xx + xx * yy)));
                }
            }

            state->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

            RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
            EXECUTE("C X: 0 Y: 0 W: 1024 H: 768 OX: 0 OY: 0", state, event, 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, 1, &rect);
        }
    }

    // Offset from 0:0 to (131:-97)
    {
        state->Actions.OffsetSurfaces(131, -97);

        // White at 0:0
        {
            Init(state);

            state->Actions.DrawMainSurfaceColorRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, WHITE_PIXEL);
            state->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

            RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
            EXECUTE("W X: 0 Y: 0 W: 1024 H: 768 OX: 131 OY: -97", state, event, 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, 1, &rect);
        }

        // Colors at 0:0
        {
            Init(state);

            for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
            {
                for (U32 yy = 0; yy < MAX_RENDERER_HEIGHT; yy++)
                {
                    state->Actions.DrawMainSurfaceColorRectangle(xx, yy, 1, 1, (PIXEL)(WHITE_PIXEL - (xx + xx * yy)));
                }
            }

            state->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

            RECTANGLE rect = { 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT };
            EXECUTE("C X: 0 Y: 0 W: 1024 H: 768 OX: 131 OY: -97", state, event, 0, 0, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, 1, &rect);
        }
    }

    // Offset back to 0:0 (-131, 97)
    {
        state->Actions.OffsetSurfaces(-131, 97);
    }

    // Finalize.
    state->Actions.UnlockRendererSurface();
    state->Actions.RestoreDisplayMode();

    ReleaseWindow(hwnd);
}
