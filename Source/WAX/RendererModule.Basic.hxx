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

typedef struct RendererModuleDirectXState
{
#ifdef ACTIVATE_MODERN_MODE
        LPDIRECTDRAW7 Instance;
        LPDIRECTDRAWSURFACE7 Surface;
#else
        LPDIRECTDRAW Instance;
        LPDIRECTDRAWSURFACE Surface;
#endif
} RENDERERMODULEDIRECTXSTATE, * LPRENDERERMODULEDIRECTXSTATE;

typedef VOID(*INITIALIZERENDERERACTION)(VOID);
typedef BOOL(*INITIALIZEDIRECTDRAWACTION)(HWND hwnd, BOOL fullscreen);
typedef VOID(*RESTOREDISPLAYMODEACTION)(VOID);
typedef BOOL(*INITIALIZEWINDOWACTION)(S32 width, S32 height);
typedef VOID(*SETPIXELCOLORMASKSACTION)(CONST U32 r, CONST U32 g, CONST U32 b);
typedef VOID(*RELEASERENDERERSURFACEACTION)(VOID);
typedef BOOL(*LOCKRENDERERSURFACEACTION)(VOID);
typedef VOID(*UNLOCKRENDERERSURFACEACTION)(VOID);
typedef VOID(*OFFSETSURFACESACTION)(S32 x, S32 y);
typedef VOID(*CONVERTCOLORSEXTRAACTION)(PIXEL* input, PIXEL* output, S32 count);
typedef VOID(*CONVERTCOLORSACTION)(PIXEL* input, PIXEL* output, S32 count);
typedef S32(*FUN_10003320ACTION)(U8* param_1, U8* param_2); // TODO
typedef VOID(*FUN_100033C0ACTION)(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10003360ACTION)(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10001E90ACTION)(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7); // TODO
typedef VOID(*FUN_10001F10ACTION)(S32 param_1, S32 param_2, S32 param_3); // TODO
typedef VOID(*FUN_10004390ACTION)(S32 param_1, S32 param_2, LPVOID param_3); // TODO
typedef VOID(*FUN_100046B6ACTION)(S32 param_1, S32 param_2, LPVOID param_3); // TODO
typedef VOID(*FUN_100049E6ACTION)(S32 param_1, S32 param_2, U16 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_10005E31ACTION)(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10005AC6ACTION)(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_1000579CACTION)(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_10007928ACTION)(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
typedef VOID(*WRITEBACKSURFACEMAINSURFACERECTANGLEACTION)(S32 x, S32 y, S32 width, S32 height);
typedef VOID(*DRAWBACKSURFACECOLORPOINTACTION)(S32 x, S32 y, PIXEL pixel);
typedef VOID(*FUN_10001ED0ACTION)(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
typedef VOID(*FUN_10001F40ACTION)(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7); // TODO
typedef VOID(*FUN_10002FB0ACTION)(S32 x, S32 y, S32 width, S32 height); // TODO
typedef VOID(*FUN_100050DFACTION)(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_100064B6ACTION)(S32 x, S32 y, LPVOID param_3); // TODO
typedef VOID(*FUN_1000618DACTION)(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_10004DB0ACTION)(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10006EF8ACTION)(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10007292ACTION)(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10006B21ACTION)(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_100067ADACTION)(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_10007662ACTION)(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_10007FBCACTION)(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_10007BE8ACTION)(S32 x, S32 y, U16 param_3, LPVOID param_4); // TODO
typedef VOID(*FUN_100053C3ACTION)(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_100017E0ACTION)(S32 x, S32 y, PIXEL pixel); // TODO
typedef VOID(*DRAWMAINSURFACECOLORRECTANGLEACTION)(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
typedef VOID(*DRAWMAINSURFACECOLORBOXACTION)(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
typedef VOID(*DRAWMAINSURFACEHORIZONTALCOLORLINEACTION)(S32 x, S32 y, S32 length, PIXEL pixel);
typedef VOID(*DRAWMAINSURFACEVERTICALCOLORLINEACTION)(S32 x, S32 y, S32 length, PIXEL pixel);
typedef VOID(*FUN_100016C0ACTION)(S32 x, S32 y, S32 width, S32 height, S32 param_5); // TODO
typedef VOID(*FUN_100023E0ACTION)(S32 x, S32 y, S32 width, S32 height, PIXEL pixel); // TODO
typedef VOID(*FUN_10002020ACTION)(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5); // TODO
typedef BOOL(*READRENDERERSURFACERECTANGLEACTION)(S32 x, S32 y, S32 width, S32 height);
typedef BOOL(*WRITEMAINSURFACERENDERERSURFACERECTANGLEACTION)(S32 x, S32 y, S32 width, S32 height);
typedef VOID(*FUN_100018A0ACTION)(S32 x, S32 y, S32 width, S32 height, S32 param_5, S32 param_6, S32 param_7, S32 param_8); // TODO
typedef VOID(*FUN_10002780ACTION)(S32 x, S32 y, S32 width, S32 height); // TODO
typedef VOID(*DRAWSTENCILSURFACEWINDOWRECTANGLEACTION)(VOID);
typedef BOOL(*WRITERENDERERSURFACESURFACERECTANGLEACTION)(S32 x, S32 y, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* pixels);
typedef VOID(*FUN_10002A30ACTION)(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7, S32 param_8, S32 param_9, S32 param_10); // TODO
typedef VOID(*FUN_10008ECDACTION)(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, LPVOID param_5); // TODO
typedef VOID(*FUN_1000A4F3ACTION)(S32 param_1, S32 param_2, S32 param_3, S32 param_4, LPVOID param_5, LPVOID param_6); // TODO
typedef VOID(*FUN_10009EB3ACTION)(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
typedef VOID(*RELEASEDIRECTDRAWACTION)(VOID);

typedef struct RendererStateActions
{
    INITIALIZERENDERERACTION                        InitializeRenderer;
    INITIALIZEDIRECTDRAWACTION                      InitializeDirectDraw;
    RESTOREDISPLAYMODEACTION                        RestoreDisplayMode;
    INITIALIZEWINDOWACTION                          InitializeWindow;
    SETPIXELCOLORMASKSACTION                        SetPixelColorMasks;
    RELEASERENDERERSURFACEACTION                    ReleaseRendererSurface;
    LOCKRENDERERSURFACEACTION                       LockRendererSurface;
    UNLOCKRENDERERSURFACEACTION                     UnlockRendererSurface;
    OFFSETSURFACESACTION                            OffsetSurfaces;
    CONVERTCOLORSEXTRAACTION                        ConvertColorsExtra;
    CONVERTCOLORSACTION                             ConvertColors;
    FUN_10003320ACTION FUN_10003320; // TODO
    FUN_100033C0ACTION FUN_100033c0; // TODO
    FUN_10003360ACTION FUN_10003360; // TODO
    FUN_10001E90ACTION FUN_10001e90; // TODO
    FUN_10001F10ACTION FUN_10001f10; // TODO
    FUN_10004390ACTION FUN_10004390; // TODO
    FUN_100046B6ACTION FUN_100046b6; // TODO
    FUN_100049E6ACTION FUN_100049e6; // TODO
    FUN_10005E31ACTION FUN_10005e31; // TODO
    FUN_10005AC6ACTION FUN_10005ac6; // TODO
    FUN_1000579CACTION FUN_1000579c; // TODO
    FUN_10007928ACTION FUN_10007928; // TODO
    WRITEBACKSURFACEMAINSURFACERECTANGLEACTION      WriteBackSurfaceMainSurfaceRectangle;
    DRAWBACKSURFACECOLORPOINTACTION                 DrawBackSurfaceColorPoint;
    FUN_10001ED0ACTION FUN_10001ed0; // TODO
    FUN_10001F40ACTION FUN_10001f40; // TODO
    FUN_10002FB0ACTION FUN_10002fb0_0; // TODO
    FUN_100050DFACTION FUN_100050df; // TODO
    FUN_100064B6ACTION FUN_100064b6; // TODO
    FUN_1000618DACTION FUN_1000618d; // TODO
    FUN_10004DB0ACTION FUN_10004db0; // TODO
    FUN_10006EF8ACTION FUN_10006ef8; // TODO
    FUN_10007292ACTION FUN_10007292; // TODO
    FUN_10006B21ACTION FUN_10006b21; // TODO
    FUN_100067ADACTION FUN_100067ad; // TODO
    FUN_10007662ACTION FUN_10007662; // TODO
    FUN_10007FBCACTION FUN_10007fbc; // TODO
    FUN_10007BE8ACTION FUN_10007be8; // TODO
    FUN_100053C3ACTION FUN_100053c3; // TODO
    FUN_100017E0ACTION FUN_100017e0; // TODO
    DRAWMAINSURFACECOLORRECTANGLEACTION             DrawMainSurfaceColorRectangle;
    DRAWMAINSURFACECOLORBOXACTION                   DrawMainSurfaceColorBox;
    DRAWMAINSURFACEHORIZONTALCOLORLINEACTION        DrawMainSurfaceHorizontalColorLine;
    DRAWMAINSURFACEVERTICALCOLORLINEACTION          DrawMainSurfaceVerticalColorLine;
    FUN_100016C0ACTION FUN_100016c0; // TODO
    FUN_100023E0ACTION FUN_100023e0; // TODO
    FUN_10002020ACTION FUN_10002020; // TODO
    READRENDERERSURFACERECTANGLEACTION              ReadRendererSurfaceRectangle;
    WRITEMAINSURFACERENDERERSURFACERECTANGLEACTION  WriteMainSurfaceRendererSurfaceRectangle;
    FUN_10002FB0ACTION FUN_10002fb0_1; // TODO
    FUN_100018A0ACTION FUN_100018a0; // TODO
    FUN_10002780ACTION FUN_10002780; // TODO
    DRAWSTENCILSURFACEWINDOWRECTANGLEACTION         DrawStencilSurfaceWindowRectangle;
    WRITERENDERERSURFACESURFACERECTANGLEACTION      WriteRendererSurfaceSurfaceRectangle;
    FUN_10002A30ACTION FUN_10002a30; // TODO
    FUN_10008ECDACTION FUN_10008ecd; // TODO
    FUN_1000A4F3ACTION FUN_1000a4f3; // TODO
    FUN_10009EB3ACTION FUN_10009eb3; // TODO
    RELEASEDIRECTDRAWACTION                         ReleaseDirectDraw;
} RENDERERMODULESTATEACTIONS, * LPRENDERERMODULESTATEACTIONS;

typedef struct Rectangle
{
    S32 X;
    S32 Y;
    S32 Width;
    S32 Height;
} RECTANGLE, * LPRECTANGLE;

typedef struct RendererModuleSurface
{
    S32 Offset; // Offset within the surface data. Originally in bytes, presently in pixels.
    S32 Y; // The surface offset along the "Y" axis in pixels.

    S32 Width;
    S32 Height;

    S32 Stride;

    PIXEL* Main; // Holds the final frame image, excluding UI.
    PIXEL* Back; // Holds the frame background, this includes ground, buildings, rails, trees, bushes, etc.
    PIXEL* Stencil; // Holds a stencil buffer of the frame. THis includes buildings, fences, power poles.

    PIXEL* Renderer; // The DirectDraw surface.
} RENDERERMODULESURFACE, * LPRENDERERMODULESURFACE;

typedef struct RendererModuleStateContainer
{
    RECTANGLE Window;
    RENDERERMODULESURFACE Surface;

    U16 ActualRedMask;
    U16 InitialRedMask;
    U16 ActualGreenMask;
    U16 InitialGreenMask;
    U16 ActualBlueMask;
    U16 InitialBlueMask;

    U16 RedOffset;
    U16 GreenOffset;
    U16 BlueOffset;

    U16 Unk16; // TODO
    U16 ActualColorBits;
    U16 Unk18; // TODO
    U16 ActualColorMask;
    U16 InitialColorMask;
    U16 Unk21; // TODO
    U16 Unk22; // TODO
    U16 Unk23; // TODO
    U16 Unk24; // TODO
    U32 Unk25; // TODO
    U32 Unk26; // TODO
    U32 Unk27; // TODO
    U32 Pitch;
    U16 Unk29; // TODO
    U16 Unk30; // TODO

    U8 Unknown000[43264]; // TODO

    HWND HWND;
    BOOL IsFullScreen;
    RENDERERMODULEDIRECTXSTATE DirectDraw;
    RENDERERMODULESTATEACTIONS Actions;
} RENDERERMODULESTATECONTAINER, * LPRENDERERMODULESTATECONTAINER;