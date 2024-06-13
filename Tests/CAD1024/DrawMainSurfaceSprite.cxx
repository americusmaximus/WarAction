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

static BOOL AcquireFile(LPCSTR name, LPVOID* content)
{
    HANDLE file = OpenFile(name);

    if (file == INVALID_HANDLE_VALUE) { return FALSE; }

    CONST U32 size = GetFileSize(file, NULL);

    *content = malloc(size);

    if (*content == NULL) { return FALSE; }

    if (!ReadFile(file, *content, size)) { return FALSE; }

    CloseHandle(file);

    return TRUE;
}

static IMAGESPRITEPTR AcquireSprite(LPVOID content, CONST U32 indx)
{
    return (IMAGESPRITEPTR)((ADDR)content + (ADDR)(((U32*)content)[indx]));
}

static VOID Execute(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 ox, S32 oy, S32 wx, S32 wy, LPCSTR name, U32 indx)
{
    Initialize(state);

    LPVOID sprite = NULL;
    CHAR path[MAX_PATH];
    sprintf_s(path, MAX_PATH, "..\\..\\..\\..\\Content\\%s.pck", name);

    if (!AcquireFile(path, &sprite)) { event->Result = FALSE; return; }

    state->Window.X = wx;
    state->Window.Y = wy;

    state->Actions.DrawMainSurfaceSprite(x, y, AcquireSprite(sprite, 0));

    //SavePixels(MakeFileName("DrawMainSurfaceSprite", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    free(sprite);

    event->Result = TRUE;
}


#define EXECUTE(A, S, E, X, Y, OX, OY, WX, WY, NAME, INDX) { E->Action = A; Execute(S, E, X, Y, OX, OY, WX, WY, NAME, INDX); if (!E->Result) { return; } }

VOID DrawMainSurfaceSprite(RENDERERMODULESTATECONTAINERPTR state, MODULEEVENTPTR event)
{
    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 0 Y: 0 OX: 0 OY: 0 WX: 0 WY: 0 GREETINGS", state, event, 0, 0, 0, 0, 0, 0, "greetings", 0);
    }
}