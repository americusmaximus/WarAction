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
#include "DrawMainSurfaceAnimationSpriteVersion4.hxx"
#include "File.hxx"
#include "FilePath.hxx"
#include "Initialize.hxx"

#include <stdio.h>

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

            state->Actions.DrawMainSurfaceAnimationSpriteVersion2(x + xx * 25, y, 100, palette, sprite);
        }
    }

    //SavePixels(MakeFileName("DrawMainSurfaceAnimationSpriteVersion4_Back", "bmp", event->Action), state->Surface.Back, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    //SavePixels(MakeFileName("DrawMainSurfaceAnimationSpriteVersion4_Main", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);
    //SavePixels(MakeFileName("DrawMainSurfaceAnimationSpriteVersion4_Stencil", "bmp", event->Action), state->Surface.Stencil, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    free(animation);
    free(palette);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, OX, OY, WX, WY, NAME, INDX) { E->Action = A; Execute(S, E, X, Y, OX, OY, WX, WY, NAME, INDX); if (!E->Result) { return; } }

VOID DrawMainSurfaceAnimationSpriteVersion4(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 100 Y: 100 OX: 0 OY: 0 WX: 0 WY: 0", state, event, 100, 100, 0, 0, 0, 0, "KATYAFAD", 0);
    }
}