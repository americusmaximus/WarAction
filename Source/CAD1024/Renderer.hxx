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

#pragma once

#include <BinAsset.hxx>
#include <Graphics.Basic.hxx>
#include <Native.Basic.hxx>

#define MAX_RENDERER_WIDTH  GRAPHICS_RESOLUTION_1024
#define MAX_RENDERER_HEIGHT GRAPHICS_RESOLUTION_768

#define SCREEN_SIZE_IN_PIXELS       (MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT)
#define SCREEN_SIZE_IN_BYTES        (SCREEN_SIZE_IN_PIXELS * sizeof(PIXEL))

#define MAX_TILE_SIZE_HEIGHT        32
#define MAX_TILE_SIZE_WIDTH         64

#define HALF_TILE_SIZE_HEIGHT       (MAX_TILE_SIZE_HEIGHT / 2)
#define HALF_TILE_SIZE_WIDTH        (MAX_TILE_SIZE_WIDTH / 2)

#define SMALL_TILE_X_STEP           2
#define LARGE_TILE_X_STEP           3

// TODO
// Remove these two variables in favor of the variables above
#define TILE_SIZE_HEIGHT            32
#define TILE_SIZE_WIDTH             63
// END TODO

typedef enum OutlintDrawOption
{
    OUTLINEDRAWOPTION_NONE          = 0,
    OUTLINEDRAWOPTION_TOP           = 1,
    OUTLINEDRAWOPTION_BOTTOM        = 2,
    OUTLINEDRAWOPTION_LEFT          = 4,
    OUTLINEDRAWOPTION_RIGHT         = 8,
    OUTLINEDRAWOPTION_FORCE_DWORD   = 0x7FFFFFFF
} OUTLINEDRAWOPTION, * OUTLINEDRAWOPTIONPTR;

typedef enum SpriteType
{
    SPRITETYPE_STATIC           = 0xA1,
    SPRITETYPE_DYNAMIC          = 0xA2,
    SPRITETYPE_ALPHA_CHANNEL    = 0xA3,
    SPRITETYPE_ANIMATION        = 0xA9,
} SPRITETYPE, * SPRITETYPEPTR;

typedef struct RendererStateContainer
{
    BOOL IsTrueColor;

    struct
    {
        PIXEL Main[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];     // 0x1001d588
        PIXEL Back[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];     // 0x1019ddac
        PIXEL Stencil[MAX_RENDERER_WIDTH * (MAX_RENDERER_HEIGHT + 1)];  // 0x1031e5d0
    } Surfaces;

    struct
    {
        OUTLINEDRAWOPTION   Options;                // 0x1001d55c
        S32                 HorizontalStride;       // 0x1001d560
        S32                 Stride;                 // 0x1001d564
        S32                 VerticalStride;         // 0x1001d568
        S32                 Height;                 // 0x1001d56c
        S32                 Width;                  // 0x1001d570
    } Outline;

    struct
    {
        PIXEL*  MinX;   // 0x1001005c
        PIXEL*  MaxX;   // 0x10010060
        U32     Width;  // 0x10010064

        struct
        {
            S16 X;      // 0x10010070
            S16 Y;      // 0x10010072
            S16 Width;  // 0x10010074
            S16 Height; // 0x10010076
        } Window;

        PIXEL*  X;                  // 0x1001007c

        U32     ColorMask;          // 0x10010084
        U32     AdjustedColorMask;  // 0x10010088

        S32     Height;             // 0x1001009a
        S32     Overage;            // 0x1001009e
    } Sprite;

    struct
    {
        U32     ColorMask;          // 0x10010030
        S32     DisplayedHalfs;     // 0x10010034
        PIXEL*  Stencil;            // 0x10010038
        S8      Unk04;              // 0x1001003c

        S32     Diff;               // 0x1001003d
        S32     Height;             // 0x10010041
        S32     TempHeight;         // 0x10010045
        S8      Unk08;              // 0x10010049

        struct
        {
            S16 X;             // 0x1001004a
            S16 Y;             // 0x1001004c
            S16 Width;         // 0x1001004e
            S16 Height;        // 0x10010050
        } Window;
    } Tile;

    struct
    {
        ADDR    Offset;                         // 0x100100b6
        U32     Stride;                         // 0x100100ba

        struct
        {
            S16 X;                              // 0x100100be
            S16 Y;                              // 0x100100c2
            S16 Width;                          // 0x100100c6
            S16 Height;                         // 0x100100ca
        } Window;

        LPVOID                  Palette;        // 0x100100ce
        IMAGEPALETTESPRITEPTR   Sprite;         // 0x100100d2
    } UI;

    struct {
        U8* Pointer;                    // 0x10012ad8      // Pointer to current fog tile. The previous and next fog tiles are used for calculations
        U32 BlocksCount;                // 0x10012adc      // Number of blocks to be copied
        S32 LineStep;                   // 0x10012ae0      // Size in bytes between two lines
        S32 DstRowStride;               // 0x10012ae4
        U32 ActualRGBMask;              // 0x10012ae8
    } FogRenderParams;

    struct
    {
        S32 Unk01;                      // 0x1049edd0
        U32 TempBlocksCount;            // 0x1049edd4
        S32 ValidRowsBlockCount;        // 0x1049edd8     // Number of valid row blocks (8 rows each)
        U32 Unk04;                      // 0x1049eddc
    } FogBlockParams;

    struct
    {
        S32 Unk01;                      // 0x1001d574
        S32 Unk02;                      // 0x1001d578
        S32 ExcessRowsBlockCount;       // 0x1001d57c     // Number of extra row blocks (8 rows each)
        S32 Unk04;                      // 0x1001d580
    } FogBlockParams2;
} RENDERERSTATECONTAINER, * RENDERERSTATECONTAINERPTR;

EXTERN RENDERERSTATECONTAINER RendererState;

BOOL InitializeDirectX(HWND hwnd, BOOL fullscreen);
BOOL InitializeWindow(S32 width, S32 height);
BOOL LockRendererSurface(VOID);
BOOL WriteMainSurfaceRendererSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
BOOL WriteRendererSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* pixels);
S32 AcquireTextLength(LPCSTR text, BINASSETCOLLECTIONCONTENTPTR asset);
VOID CleanMainSurfaceRhomb(S32 x, S32 y, S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, IMAGEPALETTETILEPTR tile);
VOID ConvertAllColors(PIXEL* input, PIXEL* output, S32 count);
VOID ConvertVisibleColors(PIXEL* input, PIXEL* output, S32 count);
VOID DrawBackSurfaceColorPoint(S32 x, S32 y, PIXEL pixel);
VOID DrawBackSurfacePaletteShadeSprite(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawBackSurfaceRhomb(S32 x, S32 y, S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, IMAGEPALETTETILEPTR input);
VOID DrawBackSurfaceShadowSprite(S32 x, S32 y, DOUBLEPIXEL color, IMAGEPALETTESPRITEPTR sprite);
VOID DrawBackSurfaceText(S32 x, S32 y, LPCSTR text, BINASSETCOLLECTIONCONTENTPTR asset, PIXEL* palette);
VOID DrawMainSurfaceAnimationSpriteStencil(S32 x, S32 y, U16 level, LPVOID palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfaceAnimationSpriteVersion4(S32 x, S32 y, U16 level, LPVOID palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfaceColorBox(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceColorEllipse(S32 x, S32 y, S32 size, PIXEL pixel, S32 step);
VOID DrawMainSurfaceColorOutline(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceColorPoint(S32 x, S32 y, PIXEL pixel);
VOID DrawMainSurfaceColorRectangle(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceColorShadeRectangle(S32 x, S32 y, S32 width, S32 height, PIXEL pixel);
VOID DrawMainSurfaceHorizontalColorLine(S32 x, S32 y, S32 length, PIXEL pixel);
VOID DrawMainSurfaceMaskRhomb(S32 x, S32 y, S32 color);
VOID DrawMainSurfacePaletteBlendSprite(S32 x, S32 y, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfacePaletteSprite(S32 x, S32 y, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfacePaletteSpriteBackStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfacePaletteSpriteFrontStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfacePaletteSpriteStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfaceShadowSprite(S32 x, S32 y, DOUBLEPIXEL color, IMAGEPALETTESPRITEPTR sprite);
VOID DrawMainSurfaceSprite(S32 x, S32 y, IMAGESPRITEPTR sprite);
VOID DrawMainSurfaceText(S32 x, S32 y, LPCSTR text, BINASSETCOLLECTIONCONTENTPTR asset, PIXEL* palette);
VOID DrawMainSurfaceVerticalColorLine(S32 x, S32 y, S32 height, PIXEL pixel);
VOID DrawStencilSurfaceWindowRectangle(VOID);
VOID DrawSurfaceRhomb(S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, S32 x, S32 y, S32 stride, IMAGEPALETTETILEPTR tile, PIXEL* output);
VOID DrawUISprite(S32 x, S32 y, IMAGEPALETTESPRITEPTR sprite, LPVOID pal, IMAGESPRITEUIPTR output);
VOID BlendMainSurfaceWithFogOfWar(S32 x, S32 y, S32 width, S32 height); // TODO
VOID FUN_10004390(S32 param_1, S32 param_2, LPVOID param_3); // TODO
VOID FUN_100046b6(S32 param_1, S32 param_2, LPVOID param_3); // TODO
VOID FUN_100049e6(S32 param_1, S32 param_2, U16 param_3, LPVOID param_4); // TODO
VOID FUN_100053c3(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_1000579c(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10005ac6(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5); // TODO
VOID FUN_100067ad(S32 x, S32 y, S32 param_3, LPVOID param_4); // TODO
VOID FUN_10007be8(S32 x, S32 y, U16 param_3, LPVOID param_4); // TODO
VOID FUN_10009eb3(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6); // TODO
VOID FUN_1000a4f3(S32 param_1, S32 param_2, S32 param_3, S32 param_4, LPVOID param_5, LPVOID param_6); // TODO
VOID Initialize(VOID);
VOID MaskStencilSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
VOID MoveStencilSurface(S32 x, S32 y, S32 width, S32 height, S32 offset);
VOID OffsetSurfaces(S32 dx, S32 dy);
VOID ReadMainSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* surface);
VOID ReleaseDirectX(VOID);
VOID ReleaseRendererSurface(VOID);
VOID RestoreDisplayMode(VOID);
VOID SetPixelColorMasks(U32 r, U32 g, U32 b);
VOID ShadeMainSurfaceRhomb(S32 x, S32 y, S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3);
VOID UnlockRendererSurface(VOID);
VOID WriteBackSurfaceMainSurfaceRectangle(S32 x, S32 y, S32 width, S32 height);
VOID WriteMainSurfaceRendererSurfaceWithFogOfWar(S32 sx, S32 sy, S32 ex, S32 ey);
VOID WriteSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 sstr, PIXEL* input, S32 dx, S32 dy, S32 dstr, PIXEL* output, S32 width, S32 height);
