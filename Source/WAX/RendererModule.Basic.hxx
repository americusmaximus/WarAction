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

#pragma once

#include "Basic.hxx"
#include "DirectDraw.hxx"

typedef struct RendererDirectXState
{
    union
    {
        LPDIRECTDRAW DirectDraw;
        LPDIRECTDRAW7 DirectDraw7;
    };
    union
    {
        LPDIRECTDRAWSURFACE Surface;
        LPDIRECTDRAWSURFACE7 Surface7;
    };
} RENDERERDIRECTXSTATE, * LPRENDERERDIRECTXSTATE;

typedef VOID(*INITIALIZERENDERERACTION)(VOID);
typedef BOOL(*INITIALIZEDIRECTDRAWACTION)(HWND hwnd, CONST BOOL fullscreen);
typedef VOID(*RESTOREDISPLAYMODEACTION)(VOID);
typedef BOOL(*INITIALIZEWINDOWACTION)(CONST U32 width, CONST U32 height);

// TODO

typedef VOID(*RELEASEDIRECTDRAWACTION)(VOID);

typedef struct RendererStateActions
{
    INITIALIZERENDERERACTION InitializeRenderer;
    INITIALIZEDIRECTDRAWACTION InitializeDirectDraw;
    RESTOREDISPLAYMODEACTION RestoreDisplayMode;
    INITIALIZEWINDOWACTION InitializeWindow;

    S32 SetRendererColorMask; // TODO
    S32 ReleaseRendererSurface; // TODO
    S32 LockRendererSurface; // TODO
    S32 UnlockRendererSurface; // TODO

    S32 Unk00[51]; // TODO

    RELEASEDIRECTDRAWACTION ReleaseDirectDraw;

    U8 Unk01[336]; // TODO


} RENDERERSTATEACTIONS, * LPRENDERERSTATEACTIONS;

typedef struct RendererStateContainer
{
    U8 Unknown000[0xa96c]; // TODO

    HWND HWND;
    BOOL IsFullScreen;
    RENDERERDIRECTXSTATE DirectDraw;
    RENDERERSTATEACTIONS Actions;
} RENDERERSTATECONTAINER, * LPRENDERERSTATECONTAINER;