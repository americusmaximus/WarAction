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
#include "DrawMainSurfaceSprite.hxx"
#include "File.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

#include <stdio.h>

static IMAGEPALETTESPRITEPTR AcquireSprite(LPVOID content, CONST U32 indx)
{
    return (IMAGEPALETTESPRITEPTR)((ADDR)content + (ADDR)(((U32*)content)[indx + 1]));
}

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 ox, S32 oy, S32 wx, S32 wy, LPCSTR name, U32 indx)
{
    Initialize(state);

    LPVOID image = NULL;
    CHAR path[MAX_PATH];
    sprintf_s(path, MAX_PATH, "..\\..\\..\\..\\Content\\%s.fnt", name);

    if (!AcquireFile(path, &image)) { event->Result = FALSE; return; }

    state->Window.X = wx;
    state->Window.Y = wy;

    PIXEL palette[256];

    for (U32 x = 0; x < 256; x++)
    {
        palette[x] = x * 0xFF;
    }

    U32 dx = 0;
    U32 dy = 0;
    for (U32 x = 0; x < 256; x++)
    {
        dx += 15;

        if (dx >= 512)
        {
            dx = 0;
            dy = dy + 30;
        }

        IMAGEPALETTESPRITEPTR sprite = AcquireSprite(image, x);

        state->Actions.DrawMainSurfacePaletteSprite(dx + 10, y + dy, palette, sprite);
    }

    //SavePixels(MakeFileName("DrawMainSurfacePaletteSprite", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    free(image);

    event->Result = TRUE;
}


#define EXECUTE(A, S, E, X, Y, OX, OY, WX, WY, NAME, INDX) { E->Action = A; Execute(S, E, X, Y, OX, OY, WX, WY, NAME, INDX); if (!E->Result) { return; } }

VOID DrawMainSurfacePaletteSprite(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 0 Y: 0 OX: 0 OY: 0 WX: 0 WY: 0", state, event, 0, 0, 0, 0, 0, 0, "font", 0);
    }
}