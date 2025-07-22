/*
Copyright (c) 2025 Americus Maximus

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
#include "DrawBackSurfaceText.hxx"
#include "File.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"
#include "SetPixelColorMasks.hxx"

#include <stdio.h>

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 ox, S32 oy, S32 wx, S32 wy, LPCSTR text)
{
    Initialize(state);

    BINASSETCONTENTPTR font = NULL;
    if (!AcquireFile("..\\..\\..\\..\\Content\\TXTA.PCK", (LPVOID*)&font)) { event->Result = FALSE; return; }

    state->Window.X = wx;
    state->Window.Y = wy;

    PIXEL palette[MAX_PALETTE_SIZE];

    for (U32 x = 0; x < MAX_PALETTE_SIZE; x++)
    {
        palette[x] = 0xFF; // x * 0xFF;
    }

    state->Actions.DrawBackSurfaceText(x, y, text, font, palette);

    SavePixels(MakeFileName("DrawBackSurfaceTextMain", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    SavePixels(MakeFileName("DrawBackSurfaceTextBack", "bmp", event->Action), state->Surface.Back, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    SavePixels(MakeFileName("DrawBackSurfaceTextStencil", "bmp", event->Action), state->Surface.Stencil, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    free(font);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, OX, OY, WX, WY, TXT) { E->Action = A; Execute(S, E, X, Y, OX, OY, WX, WY, TXT); if (!E->Result) { return; } }

VOID DrawBackSurfaceText(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    InitializePixelMasks(state);
    state->Actions.SetPixelColorMasks(0xF800, 0x7E0, 0x1F);

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 0 Y: 0 OX: 0 OY: 0 WX: 0 WY: 0", state, event, 0, 0, 0, 0, 0, 0, "Test, 1, 2, 3, 4");
    }

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 100 Y: 100 OX: 0 OY: 0 WX: 0 WY: 0", state, event, 100, 100, 0, 0, 0, 0, "100 x 100");
    }

    // Finalize.
    InitializePixelMasks(state);
}
