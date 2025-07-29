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

#include "SetPixelColorMasks.hxx"
#include "Initialize.hxx"
#include "FilePath.hxx"
#include "File.hxx"
#include "CleanMainSurfaceRhomb.hxx"
#include "BitMap.hxx"

#include <stdio.h>

#define MAX_HORIZONTAL_OFFSET   36
#define MAX_VERTICAL_OFFSET     36

#define MAX_SPRITE_SIZE        1024

static IMAGESPRITEPTR AcquireSprite(LPVOID content, CONST U32 indx)
{
    return (IMAGESPRITEPTR)((ADDR)content + (ADDR)(((U32*)content)[indx]));
}

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, S32 x, S32 y, S32 a1, S32 a2, S32 a3, S32 a4, LPVOID* sprite)
{
    Initialize(state);

    for (U32 xx = 0; xx < 25; xx++)
    {
        for (U32 xxx = 0; xxx < 20; xxx++)
        {
            CONST S32 h = x + xx * MAX_HORIZONTAL_OFFSET + xxx * MAX_HORIZONTAL_OFFSET;
            CONST S32 v = y + xxx * MAX_VERTICAL_OFFSET;

            state->Actions.CleanMainSurfaceRhomb(h, v, a1, a2, a3, a4, (IMAGEPALETTETILEPTR)(sprite[xxx]));
        }
    }

    SavePixels(MakeFileName("CleanMainSurfaceRhomb", "bmp", event->Action), state->Surface.Main, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT);

    event->Result = TRUE;
}

#define EXECUTE(A, S, E, X, Y, A1, A2, A3, A4, IMAGE) { E->Action = A; Execute(S, E, X, Y, A1, A2, A3, A4, IMAGE); if (!E->Result) { return; } }

VOID CleanMainSurfaceRhomb(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    InitializePixelMasks(state);
    state->Actions.SetPixelColorMasks(0xF800, 0x7E0, 0x1F);

    // Palette
    {
        CHAR path[MAX_PATH];
        sprintf_s(path, MAX_PATH, "..\\..\\..\\..\\Content\\rhomb.pl");

        LPVOID data = NULL;
        if (!AcquireFile(path, &data)) { event->Result = FALSE; return; }

        CopyMemory(state->Rhombs, data, MAX_PALETTE_SIZE * MAX_RHOMB_COUNT * sizeof(PIXEL));

        free(data);
    }

    // RS2
    U32 count = 0;
    LPVOID* sprites = NULL;

    {
        CHAR path[MAX_PATH];
        sprintf_s(path, MAX_PATH, "..\\..\\..\\..\\Content\\mappic.rs2");

        LPVOID map = NULL;
        if (!AcquireFile(path, &map)) { event->Result = FALSE; return; }

        {
            HANDLE file = OpenFile(path);
            if (file == INVALID_HANDLE_VALUE) { event->Result = FALSE; return; }

            count = GetFileSize(file, NULL) / MAX_SPRITE_SIZE;

            CloseHandle(file);
        }

        if (count == 0) { event->Result = FALSE; return; }

        {
            CONST U32 size = count * sizeof(LPVOID);

            sprites = (LPVOID*)malloc(size);

            if (sprites == NULL) { event->Result = FALSE; return; }

            ZeroMemory(sprites, size);
        }

        for (U32 x = 0; x < count; x++)
        {
            sprites[x] = malloc(MAX_SPRITE_SIZE);

            if (sprites[x] == NULL) { event->Result = FALSE; return; }

            CopyMemory(sprites[x], (LPVOID)((ADDR)map + MAX_SPRITE_SIZE * x), MAX_SPRITE_SIZE);
        }
    }

    // Offset to 0:0
    {
        state->Actions.OffsetSurfaces(0, 0);

        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 0, 0, 0, 0, sprites);

        EXECUTE("X: 0 Y: 0 A1: 45 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 45, 0, 0, 0, sprites);
        EXECUTE("X: 0 Y: 0 A1: 90 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 90, 0, 0, 0, sprites);
        EXECUTE("X: 0 Y: 0 A1: 180 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 180, 0, 0, 0, sprites);
        EXECUTE("X: 0 Y: 0 A1: 210 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 210, 0, 0, 0, sprites);
        EXECUTE("X: 0 Y: 0 A1: 270 A2: 0 A3: 0 A4: 0", state, event, 0, 0, 270, 0, 0, 0, sprites);

        EXECUTE("X: 0 Y: 0 A1: 0 A2: 45 A3: 0 A4: 0", state, event, 0, 0, 0, 45, 0, 0, sprites);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 45 A4: 0", state, event, 0, 0, 0, 0, 45, 0, sprites);

        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 45", state, event, 0, 0, 0, 0, 0, 45, sprites);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 90", state, event, 0, 0, 0, 0, 0, 90, sprites);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 180", state, event, 0, 0, 0, 0, 0, 180, sprites);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 210", state, event, 0, 0, 0, 0, 0, 210, sprites);
        EXECUTE("X: 0 Y: 0 A1: 0 A2: 0 A3: 0 A4: 270", state, event, 0, 0, 0, 0, 0, 270, sprites);


        EXECUTE("X: 0 Y: 0 A1: 45 A2: 45 A3: 45 A4: 45", state, event, 0, 0, 45, 45, 45, 45, sprites);
        EXECUTE("X: 0 Y: 0 A1: 45 A2: 90 A3: 45 A4: 90", state, event, 0, 0, 45, 90, 45, 90, sprites);
    }

    {
        for (U32 x = 0; x < count; x++) { free(sprites[x]); }

        free(sprites);
    }

    // Finalize.
    InitializePixelMasks(state);
}