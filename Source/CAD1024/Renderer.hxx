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

#include <Basic.hxx>
#include <Native.Basic.hxx>

#define MAX_RENDERER_WIDTH 1024
#define MAX_RENDERER_HEIGHT 768

typedef struct RendererStateContainer
{
    struct
    {
        PIXEL Main[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];     // 0x1001d588
        PIXEL Back[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];     // 0x1019ddac
        PIXEL Stencil[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];  // 0x1031e5d0
    } Surfaces;
} RENDERERSTATECONTAINER, * LPRENDERERSTATECONTAINER;

EXTERN RENDERERSTATECONTAINER RendererState;

VOID InitializeRenderer(VOID);
BOOL InitializeDirectDraw(HWND hwnd, BOOL fullscreen);
VOID RestoreDisplayMode(VOID);
BOOL InitializeWindow(S32 width, S32 height);
VOID SetPixelColorMasks(CONST U32 r, CONST U32 g, CONST U32 b);
VOID ReleaseRendererSurface(VOID);
BOOL LockRendererSurface(VOID);
VOID UnlockRendererSurface(VOID);
VOID OffsetSurfaces(S32 x, S32 y);
VOID ConvertColorsExtra(PIXEL* input, PIXEL* output, S32 count);
VOID ConvertColors(PIXEL* input, PIXEL* output, S32 count);
S32 FUN_10003320(U8* param_1, U8* param_2); // TODO
VOID FUN_100033c0(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5); // TODO
VOID FUN_10003360(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5); // TODO
VOID FUN_10001e90(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7); // TODO
VOID FUN_10001f10(S32 param_1, S32 param_2, S32 param_3); // TODO
VOID FUN_10004390(S32 param_1, S32 param_2, LPVOID param_3); // TODO
VOID FUN_100046b6(S32 param_1, S32 param_2, LPVOID param_3); // TODO
VOID FUN_100049e6(S32 param_1, S32 param_2, U16 param_3, LPVOID param_4); // TODO
VOID FUN_10005e31(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10005ac6(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_1000579c(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10007928(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
VOID WriteBackSurfaceMainSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
VOID DrawBackSurfaceColorPoint(S32 x, S32 y, PIXEL pixel);
VOID FUN_10001ed0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
VOID FUN_10001f40(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7); // TODO
VOID FUN_10002fb0(S32 x, S32 y, S32 width, S32 height); // TODO
VOID FUN_100050df(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
VOID FUN_100064b6(S32 x, S32 y, LPVOID param_3); // TODO
VOID FUN_1000618d(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10004db0(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10006ef8(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10007292(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10006b21(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_100067ad(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10007662(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10007fbc(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10007be8(S32 x, S32 y, U16 param_3, LPVOID param_4); // TODO
VOID FUN_100053c3(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_100017e0(S32 x, S32 y, PIXEL pixel); // TODO
VOID DrawMainSurfaceColorRectangle(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceColorBox(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceHorizontalColorLine(S32 x, S32 y, S32 width, PIXEL pixel);
VOID DrawMainSurfaceVerticalColorLine(S32 x, S32 y, S32 height, PIXEL pixel);
VOID FUN_100016c0(S32 x, S32 y, S32 width, S32 height, S32 param_5); // TODO
VOID FUN_100023e0(S32 x, S32 y, S32 width, S32 height, PIXEL pixel); // TODO
VOID FUN_10002020(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5); // TODO
VOID FUN_10002b90(S32 x, S32 y, S32 width, S32 height);
VOID WriteMainSurfaceRendererSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
VOID FUN_100018a0(S32 x, S32 y, S32 width, S32 height, S32 param_5, S32 param_6, S32 param_7, S32 param_8); // TODO
VOID FUN_10002780(S32 x, S32 y, S32 width, S32 height); // TODO
VOID DrawBackSurfaceWindowRectangle(VOID);
VOID FUN_10002990(S32 x, S32 y, S32 param_3, S32 sx, S32 sy, S32 param_6, S32 param_7, PIXEL* pixels); // TODO
VOID FUN_10002a30(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7, S32 param_8, S32 param_9, S32 param_10); // TODO
VOID FUN_10008ecd(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_1000a4f3(S32 param_1, S32 param_2, S32 param_3, S32 param_4, LPVOID param_5, LPVOID param_6); // TODO
VOID FUN_10009eb3(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
VOID ReleaseDirectDraw(VOID);