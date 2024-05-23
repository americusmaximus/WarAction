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
#include <Graphics.Basic.hxx>
#include <Native.Basic.hxx>

#define MAX_RENDERER_WIDTH GRAPHICS_RESOLUTION_800
#define MAX_RENDERER_HEIGHT GRAPHICS_RESOLUTION_600

typedef enum RendererOutlineSkipOptions
{
    RENDERER_OUTLINE_OPTIONS_SKIP_NONE          = 0,
    RENDERER_OUTLINE_OPTIONS_SKIP_TOP           = 1,
    RENDERER_OUTLINE_OPTIONS_SKIP_BOTTOM        = 2,
    RENDERER_OUTLINE_OPTIONS_SKIP_LEFT          = 4,
    RENDERER_OUTLINE_OPTIONS_SKIP_RIGHT         = 8,
    RENDERER_OUTLINE_OPTIONS_SKIP_FORCE_DWORD   = 0x7fffffff
} RENDEREROUTLINESKIPOPTIONS, * LPRENDEREROUTLINESKIPOPTIONS;

typedef struct RendererStateContainer
{
    struct
    {
        PIXEL Main[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];     // 0x1001d588
        PIXEL Back[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];     // 0x101081ec
        PIXEL Stencil[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];  // 0x101f2e50
    } Surfaces;

    struct
    {
        RENDEREROUTLINESKIPOPTIONS Options; // TODO
        S32 HorizontalDirection; // TODO
        S32 Stride; // TODO
        S32 VerticalDirection; // TODO
        S32 Height; // TODO
        S32 Width; // TODO
    } Outline;
} RENDERERSTATECONTAINER, * LPRENDERERSTATECONTAINER;

EXTERN RENDERERSTATECONTAINER RendererState;

BOOL FUN_10002ba0(S32 x, S32 y, S32 width, S32 height); // TODO
BOOL InitializeDirectX(HWND hwnd, BOOL fullscreen);
BOOL InitializeWindow(S32 width, S32 height);
BOOL LockRendererSurface(VOID);
BOOL WriteMainSurfaceRendererSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
BOOL WriteRendererSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* pixels);
S32 FUN_10003330(U8* param_1, U8* param_2); // TODO
VOID ConvertColors(PIXEL* input, PIXEL* output, S32 count);
VOID ConvertColorsExtra(PIXEL* input, PIXEL* output, S32 count); // TODO: Better name.
VOID DrawBackSurfaceColorPoint(S32 x, S32 y, PIXEL pixel);
VOID DrawMainSurfaceColorBox(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceColorOutline(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceColorRectangle(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceCursor(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID DrawMainSurfaceHorizontalColorLine(S32 x, S32 y, S32 length, PIXEL pixel);
VOID DrawMainSurfaceRectangleColorOverlay(S32 x, S32 y, S32 width, S32 height, S32 param_5); // TODO
VOID DrawMainSurfaceSprite(S32 x, S32 y, SPRITEPTR sprite);
VOID DrawMainSurfaceText(S32 x, S32 y, LPVOID param_3, LPVOID param_4); // TODO
VOID DrawMainSurfaceVerticalColorLine(S32 x, S32 y, S32 length, PIXEL pixel);
VOID DrawStencilSurfaceWindowRectangle(VOID);
VOID FUN_100017f0(S32 x, S32 y, PIXEL pixel); // TODO
VOID FUN_10001ea0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7); // TODO
VOID FUN_10001ee0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
VOID FUN_10001f20(S32 param_1, S32 param_2, S32 param_3); // TODO
VOID FUN_10001f50(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7); // TODO
VOID FUN_10002030(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5); // TODO
VOID FUN_10002fc0(S32 x, S32 y, S32 width, S32 height); // TODO
VOID FUN_10003370(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5); // TODO
VOID FUN_100033d0(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5); // TODO
VOID FUN_100043a0(S32 param_1, S32 param_2, LPVOID param_3); // TODO
VOID FUN_100046c6(S32 param_1, S32 param_2, LPVOID param_3); // TODO
VOID FUN_100049f6(S32 param_1, S32 param_2, U16 param_3, LPVOID param_4); // TODO
VOID FUN_10004dc0(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_100053d3(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_100057ac(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10005ad6(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10005e41(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_1000619d(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_100067bd(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10006f08(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_100072a2(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10007672(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10007938(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10007bf8(S32 x, S32 y, U16 param_3, LPVOID param_4); // TODO
VOID FUN_10007fcc(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10008edd(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_10009ec3(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
VOID FUN_1000a503(S32 param_1, S32 param_2, S32 param_3, S32 param_4, LPVOID param_5, LPVOID param_6); // TODO
VOID Initialize(VOID);
VOID MaskStencilSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
VOID OffsetSurfaces(S32 x, S32 y);
VOID ReadMainSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* surface);
VOID ReleaseRendererSurface(VOID);
VOID RestoreDisplayMode(VOID);
VOID SetPixelColorMasks(CONST U32 r, CONST U32 g, CONST U32 b);
VOID UnlockRendererSurface(VOID);
VOID WriteBackSurfaceMainSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
VOID WriteSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 sstr, PIXEL* input, S32 dx, S32 dy, S32 dstr, PIXEL* output, S32 width, S32 height);
VOID ReleaseDirectX(VOID);