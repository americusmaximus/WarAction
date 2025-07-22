/*
Copyright (c) 2024 - 2025 Americus Maximus

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
#include "DrawMainSurfaceAnimationSpriteVersion1A.hxx"
#include "File.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"
#include "SetPixelColorMasks.hxx"

#include <stdio.h>

static IMAGESPRITEPTR AcquireSprite(LPVOID content, CONST U32 indx)
{
    return (IMAGESPRITEPTR)((ADDR)content + (ADDR)(((U32*)content)[indx]));
}

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 ox, S32 oy, S32 wx, S32 wy, LPCSTR name, U32 indx)
{
    Initialize(state);

    LPVOID animation = NULL;
    LPVOID palette = NULL;

    {
        CHAR path[MAX_PATH];
        sprintf_s(path, MAX_PATH, "..\\..\\..\\..\\Content\\%s.ani", name);

        if (!AcquireFile(path, &animation)) { event->Result = FALSE; return; }
    }

    {
        CHAR path[MAX_PATH];
        sprintf_s(path, MAX_PATH, "..\\..\\..\\..\\Content\\%s.col", name);

        if (!AcquireFile(path, &palette)) { free(animation); event->Result = FALSE; return; }
    }

    state->Window.X = wx;
    state->Window.Y = wy;

    {
        ANIMATIONSPRITEHEADERPTR header = (ANIMATIONSPRITEHEADERPTR)animation;

        for (U32 xx = 0; xx < header->Count; xx++)
        {
            IMAGEPALETTESPRITEPTR sprite = (IMAGEPALETTESPRITEPTR)((ADDR)animation + (ADDR)header->Offsets[xx]);

            state->Actions.DrawMainSurfaceAnimationSpriteVersion1A(x + xx * 25, y, 100, (ANIMATIONPIXEL*)palette, sprite);
        }
    }

    SavePixels(MakeFileName("DrawMainSurfaceAnimationSpriteVersion1A_Back", "bmp", event->Action), state->Surface.Back, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    SavePixels(MakeFileName("DrawMainSurfaceAnimationSpriteVersion1A_Main", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    SavePixels(MakeFileName("DrawMainSurfaceAnimationSpriteVersion1A_Stencil", "bmp", event->Action), state->Surface.Stencil, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    free(animation);
    free(palette);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, OX, OY, WX, WY, NAME, INDX) { E->Action = A; Execute(S, E, X, Y, OX, OY, WX, WY, NAME, INDX); if (!E->Result) { return; } }

VOID DrawMainSurfaceAnimationSpriteVersion1A(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    InitializePixelMasks(state);
    state->Actions.SetPixelColorMasks(0xF800, 0x7E0, 0x1F);

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 100 Y: 100 OX: 0 OY: 0 WX: 0 WY: 0", state, event, 100, 100, 0, 0, 0, 0, "d_star_", 0);
    }

    // Finalize.
    InitializePixelMasks(state);
}