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

#include "Module.hxx"
#include "Renderer.hxx"

#include <Mathematics.Basic.hxx>

#define DEFAULT_FONT_ASSET_SPACING      2

#define PIXEL_COLOR_BIT_MASK            0x8000
#define STENCIL_PIXEL_COLOR_SHIFT       5
#define STENCIL_PIXEL_COLOR_VALUE       32
#define STENCIL_PIXEL_MASK_VALUE        0xFFFB

// NOTE: The original game copies values as if they were 32-bit long, while they are 16-bit long.
// Because of this, it ignores the last odd pixel (if any), due to left shift operation (width >> 1) == (width / 2).
// The macro is used to reach the same buggy result as in the original game.
#define ACQUIREWIDTH(x) (2 * (x / 2))

RENDERERSTATECONTAINER RendererState;

using namespace Mathematics;

VOID MoveStencilSurface(S32 x, S32 y, S32 width, S32 height, S32 offset);

// 0x10001000
VOID Initialize()
{
    ModuleState.Surface.Offset = 0;

    ModuleState.Surface.Y = MAX_RENDERER_HEIGHT;
    ModuleState.Surface.Width = MAX_RENDERER_WIDTH;
    ModuleState.Surface.Height = MAX_RENDERER_HEIGHT;

    ModuleState.Surface.Stride = MAX_RENDERER_WIDTH * sizeof(PIXEL);

    ModuleState.Window.X = 0;
    ModuleState.Window.Y = 0;
    ModuleState.Window.Width = MAX_RENDERER_WIDTH - 1;
    ModuleState.Window.Height = MAX_RENDERER_HEIGHT - 1;
}

// 0x10001050
BOOL InitializeDirectX(HWND hwnd, BOOL fullscreen)
{
    RestoreDisplayMode();

#if ACTIVATE_MODERN_MODE
    if (FAILED(DirectDrawCreateEx(NULL, (LPVOID*)&ModuleState.DirectX.Instance, IID_IDirectDraw7, NULL))) { return FALSE; }
#else
    if (FAILED(DirectDrawCreate(NULL, &ModuleState.DirectX.Instance, NULL))) { return FALSE; }
#endif

    if (FAILED(ModuleState.DirectX.Instance->SetCooperativeLevel(hwnd,
        fullscreen ? (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) : DDSCL_NORMAL))) { return FALSE; }

    ModuleState.IsFullScreen = fullscreen;
    ModuleState.HWND = hwnd;

    return TRUE;
}

// 0x100010b0
VOID ReleaseRendererSurface()
{
    DIRECTDRAWRELEASE(ModuleState.DirectX.Surface);
}

// 0x100010d0
VOID RestoreDisplayMode()
{
    ReleaseRendererSurface();

    if (ModuleState.DirectX.Instance != NULL)
    {
        if (ModuleState.IsFullScreen) { ModuleState.DirectX.Instance->RestoreDisplayMode(); }

        DIRECTDRAWRELEASE(ModuleState.DirectX.Instance);
    }
}

// 0x10001110
VOID ReleaseDirectX()
{
    ReleaseRendererSurface();

    DIRECTDRAWRELEASE(ModuleState.DirectX.Instance);
}

// 0x10001130
VOID SetPixelColorMasks(U32 r, U32 g, U32 b)
{
    ModuleState.ActualRedMask = r;
    ModuleState.InitialRedMask = r;
    ModuleState.ActualGreenMask = g;
    ModuleState.InitialGreenMask = g;
    ModuleState.ActualBlueMask = b;
    ModuleState.InitialBlueMask = b;

    CONST U32 rm = r & DEFAULT_SCREEN_COLOR_MASK;
    CONST U32 gm = g & DEFAULT_SCREEN_COLOR_MASK;
    CONST U32 bm = b & DEFAULT_SCREEN_COLOR_MASK;

    // R
    {
        U32 x = 0;
        U32 mask = PIXEL_COLOR_BIT_MASK;

        for (; x < GRAPHICS_BITS_PER_PIXEL_16; x++)
        {
            if (mask & rm) { ModuleState.ActualColorMask = ModuleState.ActualColorMask | mask; break; }

            mask = mask >> 1;
        }

        ModuleState.RedOffset = x;
    }

    // G
    {
        U32 x = 0;
        U32 mask = PIXEL_COLOR_BIT_MASK;

        for (; x < GRAPHICS_BITS_PER_PIXEL_16; x++)
        {
            if (mask & gm) { ModuleState.ActualColorMask = ModuleState.ActualColorMask | mask; break; }

            mask = mask >> 1;
        }

        ModuleState.GreenOffset = x;
    }

    // B
    {
        U32 x = 0;
        U32 mask = PIXEL_COLOR_BIT_MASK;

        for (; x < GRAPHICS_BITS_PER_PIXEL_16; x++)
        {
            if (mask & bm) { ModuleState.ActualColorMask = ModuleState.ActualColorMask | mask; break; }

            mask = mask >> 1;
        }

        ModuleState.BlueOffset = x;
    }

    // R
    {
        U32 mask = 1;

        for (U32 x = 0; x < GRAPHICS_BITS_PER_PIXEL_16; x++)
        {
            if (mask & rm) { ModuleState.ActualColorBits = ModuleState.ActualColorBits | mask; break; }

            mask = mask << 1;
        }
    }

    // G
    {
        U32 mask = 1;

        for (U32 x = 0; x < GRAPHICS_BITS_PER_PIXEL_16; x++)
        {
            if (mask & gm) { ModuleState.ActualColorBits = ModuleState.ActualColorBits | mask; break; }

            mask = mask << 1;
        }
    }

    // B
    {
        U32 mask = 1;

        for (U32 x = 0; x < GRAPHICS_BITS_PER_PIXEL_16; x++)
        {
            if (mask & bm) { ModuleState.ActualColorBits = ModuleState.ActualColorBits | mask; break; }

            mask = mask << 1;
        }
    }

    ModuleState.InitialColorMask = ModuleState.ActualColorMask;

    ModuleState.Unk23 = ~ModuleState.ActualColorMask;
    ModuleState.Unk24 = ~ModuleState.ActualColorMask;

    ModuleState.Unk18 = ModuleState.ActualColorBits;
    ModuleState.ShadeColorMask = ~ModuleState.ActualColorBits;
    ModuleState.Unk22 = ~ModuleState.ActualColorBits;

    ModuleState.BackSurfaceShadePixel =
        ((5 << (11 - ModuleState.BlueOffset)) + (2 << (11 - ModuleState.GreenOffset))) & DEFAULT_SCREEN_COLOR_MASK;
    ModuleState.BackSurfaceShadePixel = (ModuleState.BackSurfaceShadePixel << 16) | ModuleState.BackSurfaceShadePixel;

    if (ModuleState.GreenOffset < ModuleState.RedOffset)
    {
        if (ModuleState.BlueOffset < ModuleState.GreenOffset)
        {
            ModuleState.Unk25 = (gm << 16) | bm | rm;
        }
        else if (ModuleState.BlueOffset <= ModuleState.RedOffset)
        {
            ModuleState.Unk25 = (bm << 16) | gm | rm;
        }
        else { ModuleState.Unk25 = (rm << 16) | bm | gm; }
    }
    else
    {
        if (ModuleState.GreenOffset <= ModuleState.BlueOffset)
        {
            ModuleState.Unk25 = (gm << 16) | bm | rm;
        }
        else if (ModuleState.RedOffset < ModuleState.BlueOffset)
        {
            ModuleState.Unk25 = (bm << 16) | gm | rm;
        }
        else { ModuleState.Unk25 = (rm << 16) | bm | gm; }
    }

    ModuleState.Unk27 = ((ModuleState.ActualColorMask << 16) | ModuleState.ActualColorMask) & ModuleState.Unk25;
}

// 0x10001330
BOOL InitializeWindow(S32 width, S32 height)
{
    ReleaseRendererSurface();

#if ACTIVE_TRUE_COLOR_MODE
    HDC hdc = GetDC(ModuleState.HWND);

    RendererState.IsTrueColor = GetDeviceCaps(hdc, BITSPIXEL) == GRAPHICS_BITS_PER_PIXEL_32;

    ReleaseDC(ModuleState.HWND, hdc);
#endif

    if (ModuleState.IsFullScreen)
    {
        CONST U32 bits = RendererState.IsTrueColor ? GRAPHICS_BITS_PER_PIXEL_32 : GRAPHICS_BITS_PER_PIXEL_16;

        if (FAILED(ModuleState.DirectX.Instance->SetDisplayMode(width, height, bits))) { return FALSE; }

        SetWindowPos(ModuleState.HWND, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }
    else
    {
        HDC hdc = GetDC(ModuleState.HWND);

        CONST U32 sw = GetDeviceCaps(hdc, HORZRES);
        CONST U32 sh = GetDeviceCaps(hdc, VERTRES);

        ReleaseDC(ModuleState.HWND, hdc);

        SetWindowLongA(ModuleState.HWND, GWL_STYLE, WS_CAPTION);

        RECT rect;
        ZeroMemory(&rect, sizeof(RECT));
        AdjustWindowRect(&rect, WS_CAPTION, FALSE);

        width = width + (rect.right - rect.left);
        height = height + (rect.bottom - rect.top);

        SetWindowPos(ModuleState.HWND, NULL, (sw - width) / 2, (sh - height) / 2, width, height, SWP_SHOWWINDOW);
    }

    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if (FAILED(ModuleState.DirectX.Instance->CreateSurface(&desc, &ModuleState.DirectX.Surface, NULL))) { return FALSE; }

    if (RendererState.IsTrueColor)
    {
        SetPixelColorMasks(0xF800, 0x07E0, 0x001F); // Report RGB565 in case the game runs on 32-bit pixel depth system.
    }
    else
    {
        if (FAILED(ModuleState.DirectX.Surface->GetSurfaceDesc(&desc))) { return FALSE; }

        SetPixelColorMasks(desc.ddpfPixelFormat.dwRBitMask, desc.ddpfPixelFormat.dwGBitMask, desc.ddpfPixelFormat.dwBBitMask);
    }

    if (!ModuleState.IsFullScreen)
    {
        LPDIRECTDRAWCLIPPER clipper = NULL;

        if (FAILED(ModuleState.DirectX.Instance->CreateClipper(0, &clipper, NULL))) { DIRECTDRAWRELEASE(ModuleState.DirectX.Surface); return FALSE; }

        if (FAILED(clipper->SetHWnd(0, ModuleState.HWND))) { DIRECTDRAWRELEASE(clipper); DIRECTDRAWRELEASE(ModuleState.DirectX.Surface); return FALSE; }

        if (FAILED(ModuleState.DirectX.Surface->SetClipper(clipper))) { DIRECTDRAWRELEASE(clipper); DIRECTDRAWRELEASE(ModuleState.DirectX.Surface); return FALSE; }

        DIRECTDRAWRELEASE(clipper);
    }

    ModuleState.Actions.Initialize();

    ModuleState.Window.X = 0;
    ModuleState.Window.Y = 0;
    ModuleState.Window.Width = width - 1;
    ModuleState.Window.Height = height - 1;

    return TRUE;
}

// 0x10001420
VOID DrawMainSurfaceHorizontalColorLine(S32 x, S32 y, S32 length, PIXEL pixel)
{
    S32 max = x + length - 1;

    if ((ModuleState.Window.Y <= y) && (y <= ModuleState.Window.Height))
    {
        if (x < ModuleState.Window.X) { x = ModuleState.Window.X; }
        if (ModuleState.Window.Width < max) { max = ModuleState.Window.Width; }

        if (x <= max)
        {
            PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main
                + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

            if (ModuleState.Surface.Y <= y)
            {
                pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            }

            for (S32 xx = 0; xx < max - x + 1; xx++) { pixels[xx] = pixel; }
        }
    }
}

// 0x100014b0
VOID DrawMainSurfaceVerticalColorLine(S32 x, S32 y, S32 height, PIXEL pixel)
{
    S32 max = height + y - 1;

    if (x < ModuleState.Window.X) { return; }
    if (ModuleState.Window.Width < x) { return; }

    if (y < ModuleState.Window.Y) { y = ModuleState.Window.Y; }
    if (ModuleState.Window.Height < max) { max = ModuleState.Window.Height; }

    max = max + 1 - y;

    if (max < 1) { return; }

    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + max - ModuleState.Surface.Y;

        if (delta < 1)
        {
            for (S32 xx = 0; xx < max; xx++)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 xx = 0; xx < max - delta; xx++)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 xx = 0; xx < delta; xx++)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 xx = 0; xx < max; xx++)
        {
            pixels[0] = pixel;

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x10001570
VOID DrawMainSurfaceColorBox(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    DrawMainSurfaceHorizontalColorLine(x, y, width, pixel);
    DrawMainSurfaceHorizontalColorLine(x, y + height - 1, width, pixel);
    DrawMainSurfaceVerticalColorLine(x, y, height, pixel);
    DrawMainSurfaceVerticalColorLine(x + width - 1, y, height, pixel);
}

// 0x100015d0
VOID DrawMainSurfaceColorRectangle(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    if (x < ModuleState.Window.X)
    {
        width = width + x - ModuleState.Window.X;
        x = ModuleState.Window.X;
    }

    if (y < ModuleState.Window.Y)
    {
        height = height + y - ModuleState.Window.Y;
        y = ModuleState.Window.Y;
    }

    if (ModuleState.Window.Width < (x + width - 1))
    {
        width = ModuleState.Window.Width - x + 1;
    }

    if (ModuleState.Window.Height < (y + height - 1))
    {
        height = ModuleState.Window.Height - y + 1;
    }

    if (width < 1 || height < 1) { return; }

    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x100016c0
VOID DrawMainSurfaceColorShadeRectangle(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    if (x < ModuleState.Window.X)
    {
        width = width + x - ModuleState.Window.X;
        x = ModuleState.Window.X;

        if (width < 1) { return; }
    }

    if (y < ModuleState.Window.Y)
    {
        height = height + y - ModuleState.Window.Y;
        y = ModuleState.Window.Y;

        if (height < 1) { return; }
    }

    if (ModuleState.Window.Width < (x + width - 1))
    {
        width = ModuleState.Window.Width - x + 1;
    }

    if (ModuleState.Window.Height < (y + height - 1))
    {
        height = ModuleState.Window.Height - y + 1;
    }

    if (width < 1 || height < 1) { return; }

    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    CONST PIXEL color = SHADEPIXEL(pixel, ModuleState.ShadeColorMask);

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color; }

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x100017e0
VOID DrawMainSurfaceColorPoint(S32 x, S32 y, PIXEL pixel)
{
    if (ModuleState.Window.X <= x && ModuleState.Window.Y <= y
        && x <= ModuleState.Window.Width && y <= ModuleState.Window.Height)
    {
        S32 indx = ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x;

        if (ModuleState.Surface.Y <= y) { indx = indx - MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; }

        RendererState.Surfaces.Main[indx] = pixel;
    }
}

// 0x10001840
VOID DrawBackSurfaceColorPoint(S32 x, S32 y, PIXEL pixel)
{
    if (ModuleState.Window.X <= x && ModuleState.Window.Y <= y
        && x <= ModuleState.Window.Width && y <= ModuleState.Window.Height)
    {
        S32 indx = ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x;

        if (ModuleState.Surface.Y <= y) { indx = indx - MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT; }

        RendererState.Surfaces.Back[indx] = pixel;
    }
}

// 0x100018a0
VOID ReadMainSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* surface)
{
    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)((ModuleState.Surface.Offset + (sy * MAX_RENDERER_WIDTH + sx)) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((ADDR)surface + (ADDR)((stride * dy + dx) * sizeof(PIXEL)));

    if (sy < ModuleState.Surface.Y)
    {
        CONST S32 delta = sy + height - ModuleState.Surface.Y;

        if ((sy + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
            }

            src = (PIXEL*)((ADDR)src - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        src = (PIXEL*)((ADDR)src - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

            src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
        }
    }
}

// 0x10001be0
VOID ConvertVisibleColors(PIXEL* input, PIXEL* output, S32 count)
{
    for (S32 x = 0; x < count; x++)
    {
        CONST PIXEL pixel = input[x];

        if (pixel == MAGENTA_PIXEL) { output[x] = MAGENTA_PIXEL; continue; }

        CONST PIXEL r = (((pixel & 0xF800) << 0) >> (ModuleState.RedOffset & 0x1F)) & ModuleState.ActualRedMask;
        CONST PIXEL g = (((pixel & 0x07E0) << 5) >> (ModuleState.GreenOffset & 0x1F)) & ModuleState.ActualGreenMask;
        CONST PIXEL b = (((pixel & 0x001F) << 11) >> (ModuleState.BlueOffset & 0x1F)) & ModuleState.ActualBlueMask;

        output[x] = (PIXEL)(r | g | b);
    }
}

// 0x10001c80
VOID ConvertAllColors(PIXEL* input, PIXEL* output, S32 count)
{
    for (S32 x = 0; x < count; x++)
    {
        CONST PIXEL pixel = input[x];

        CONST PIXEL r = (((pixel & 0xF800) << 0) >> (ModuleState.RedOffset & 0x1F)) & ModuleState.ActualRedMask;
        CONST PIXEL g = (((pixel & 0x07E0) << 5) >> (ModuleState.GreenOffset & 0x1F)) & ModuleState.ActualGreenMask;
        CONST PIXEL b = (((pixel & 0x001F) << 11) >> (ModuleState.BlueOffset & 0x1F)) & ModuleState.ActualBlueMask;

        output[x] = (PIXEL)(r | g | b);
    }
}

// 0x10001d00
VOID OffsetSurfaces(S32 x, S32 y)
{
    S32 offset = ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x;

    // Normalize offset so it is within the expected range.
    if (offset < 0)
    {
        while (offset < 0)
        {
            offset = offset + (MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT);
        }
    }
    else
    {
        while (offset >= MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT)
        {
            offset = offset - (MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT);
        }
    }

    S32 length = ModuleState.Surface.Offset & (0x80000000 | (MAX_RENDERER_WIDTH - 1));

    if (length < 0) { length = ((length - 1) | (-MAX_RENDERER_WIDTH)) + 1; }

    if (length + x < 0)
    {
        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            ModuleState.Surface.Back[MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT + x] = ModuleState.Surface.Back[x];
        }

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            ModuleState.Surface.Main[MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT + x] = ModuleState.Surface.Main[x];
        }

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            ModuleState.Surface.Stencil[MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT + x] = ModuleState.Surface.Stencil[x];
        }
    }
    else if (MAX_RENDERER_WIDTH <= length + x)
    {
        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            ModuleState.Surface.Back[x] = ModuleState.Surface.Back[MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT + x];
        }

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            ModuleState.Surface.Main[x] = ModuleState.Surface.Main[MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT + x];
        }

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++)
        {
            ModuleState.Surface.Stencil[x] = ModuleState.Surface.Stencil[MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT + x];
        }
    }

    CONST S32 lines = (offset + ((offset >> 0x1f) & (MAX_RENDERER_WIDTH - 1))) / MAX_RENDERER_WIDTH;

    ModuleState.Surface.Y = MAX_RENDERER_HEIGHT - lines;
    ModuleState.Surface.Offset = offset;

    if (y < 1)
    {
        if (y < 0) // Negative Y.
        {
            if (0 < x) // Positive X, Negative Y.
            {
                MoveStencilSurface(0, -y, MAX_RENDERER_WIDTH - x, y + MAX_RENDERER_HEIGHT, -y);
            }
            else // Negative X, Negative Y.
            {
                MoveStencilSurface(-x, -y, x + MAX_RENDERER_WIDTH, y + MAX_RENDERER_HEIGHT, -y);
            }
        }
    }
    else if (x < 1) // Negative X, Positive Y.
    {
        MoveStencilSurface(-x, 0, x + MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT - y, -y);
    }
    else // Positive X. Positive Y.
    {
        MoveStencilSurface(0, 0, MAX_RENDERER_WIDTH - x, MAX_RENDERER_HEIGHT - y, -y);
    }
}

// 0x10001e90
VOID FUN_10001e90(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001ed0
VOID FUN_10001ed0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f10
VOID FUN_10001f10(S32 param_1, S32 param_2, S32 param_3)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f40
VOID FUN_10001f40(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f80
VOID WriteBackSurfaceMainSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Back + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((ADDR)RendererState.Surfaces.Main + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            src = (PIXEL*)((ADDR)src - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            dst = (PIXEL*)((ADDR)dst - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        src = (PIXEL*)((ADDR)src - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        dst = (PIXEL*)((ADDR)dst - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

            src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x10002020
VOID DrawMainSurfaceColorEllipse(S32 x, S32 y, S32 size, PIXEL pixel, S32 step)
{
    S32 yy = 0;
    S32 current = 1 - size;
    S32 distance = step;
    S32 xx = size;
    S32 end = 0;
    S32 start = -size;
    S32 offset = start;

    size = size * 2;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
            start = offset;
        }
        if (-1 < current) {
            size = size + -2;
            xx = xx + -1;
            start = start + 1;
            distance = distance + 0x3989;
            current = current - size;
            offset = start;
        }
        end = end + -4;
        yy = yy + 1;
        distance = distance + 0x10000;
        current = current + 1 + yy * 8;
    } while (start <= end);

    size = yy * 8;
    current = -current;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
        }
        if (-1 < current) {
            size = size + 8;
            yy = yy + 1;
            distance = distance + 0x3989;
            current = current - size;
        }
        xx = xx + -1;
        distance = distance + 0x10000;
        current = current + -1 + xx * 2;
    } while (-1 < xx);

    offset = xx * 2;
    start = yy * -4;
    size = yy * 8;
    end = start;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
            start = end;
        }
        if (-1 < current) {
            size = size + -8;
            start = start + 4;
            yy = yy + -1;
            distance = distance + 0x3989;
            current = current - size;
            end = start;
        }
        offset = offset + -2;
        xx = xx + -1;
        distance = distance + 0x10000;
        current = current + (1 - offset);
    } while (start <= xx);

    current = -current;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
        }
        if (-1 < current) {
            xx = xx + -1;
            distance = distance + 0x3989;
            current = current + xx * 2;
        }
        yy = yy + -1;
        distance = distance + 0x10000;
        current = current + 1 + yy * 8;
    } while (-1 < yy);

    size = yy * 4;
    end = yy * 8;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
        }
        if (-1 < current) {
            xx = xx + 1;
            distance = distance + 0x3989;
            current = current + xx * 2;
        }
        size = size + -4;
        end = end + -8;
        yy = yy + -1;
        distance = distance + 0x10000;
        current = current + (-1 - end);
    } while (xx <= size);

    current = -current;
    start = -xx;
    size = xx * 2;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
        }
        if (-1 < current) {
            yy = yy + -1;
            distance = distance + 0x3989;
            current = current + yy * 8;
        }
        size = size + 2;
        xx = xx + 1;
        start = start + -1;
        distance = distance + 0x10000;
        current = current + (1 - size);
    } while (-1 < start);

    size = yy * 4;
    start = -xx;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx + x, yy + y, pixel);
        }
        if (-1 < current) {
            yy = yy + 1;
            size = size + 4;
            distance = distance + 0x3989;
            current = current + yy * 8;
        }
        xx = xx + 1;
        start = start + -1;
        distance = distance + 0x10000;
        current = current + -1 + xx * 2;
    } while (size <= start);

    end = yy * 8;
    current = -current;
    size = xx * 2;
    xx = xx + x;
    start = yy * -4;
    yy = yy + y;

    do {
        if (step < distance) {
            distance = distance - step;
            DrawMainSurfaceColorPoint(xx, yy, pixel);
        }
        if (-1 < current) {
            size = size + 2;
            xx = xx + 1;
            distance = distance + 0x3989;
            current = current - size;
        }
        end = end + 8;
        start = start + -4;
        current = current + (-1 - end);
        yy = yy + 1;
        distance = distance + 0x10000;
    } while (-1 < start);
}

// 0x100023e0
VOID DrawMainSurfaceColorOutline(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    S32 offset = ModuleState.Surface.Offset & (0x80000000 | (MAX_RENDERER_WIDTH - 1));

    if (offset < 0) { offset = ((offset - 1) | (-MAX_RENDERER_WIDTH)) + 1; }

    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)((offset + MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT) * sizeof(PIXEL)));

    RendererState.Outline.Width = ModuleState.Window.Width - ModuleState.Window.X;
    RendererState.Outline.Height = ModuleState.Window.Height + 1 - ModuleState.Window.Y;

    x = x - ModuleState.Window.X;
    y = y - ModuleState.Window.Y;

    RendererState.Outline.Options = OUTLINESKIPOPTIONS_NONE;

    if (y < 0)
    {
        height = height + y;

        if (height < 1) { RendererState.Outline.Options = OUTLINESKIPOPTIONS_NONE; return; }

        y = 0;
        RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_TOP);
    }

    if (RendererState.Outline.Height <= y)
    {
        height = height + y;
        y = RendererState.Outline.Height - 1;
        height = height - y;

        if (-1 < height) { return; }

        RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_TOP);
    }

    {
        CONST S32 max = y + 1 + height;

        if (y + 1 + height <= 0 != max < 0)
        {
            height = height - max - 1;
            RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_BOTTOM);
        }
    }

    {
        CONST S32 max = y - 1 + height;

        if (RendererState.Outline.Height <= max)
        {
            height = height + RendererState.Outline.Height - max;
            RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_BOTTOM);
        }
    }

    if (x < 1)
    {
        width = width + x;

        if (width < 2) { return; }

        x = 1;
        width = width - 1;

        RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_LEFT);
    }

    if (RendererState.Outline.Width + 2 <= x)
    {
        width = width + x + 1 - RendererState.Outline.Width - 2;
        x = RendererState.Outline.Width + 1;

        if (-1 < width) { return; }

        RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_LEFT);
    }

    if (x + width <= 0 != x + width < 0)
    {
        width = width - x - width;

        RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_RIGHT);
    }

    {
        S32 max = x - 2 + width;

        if (RendererState.Outline.Width < max)
        {
            width = width + RendererState.Outline.Width - max;

            RendererState.Outline.Options = (OUTLINESKIPOPTIONS)(RendererState.Outline.Options | OUTLINESKIPOPTIONS_RIGHT);
        }
    }

    RendererState.Outline.HorizontalDirection = 1;

    if (width < 0)
    {
        RendererState.Outline.HorizontalDirection = -RendererState.Outline.HorizontalDirection;
        width = -width;
    }

    RendererState.Outline.VerticalDirection = 1;
    RendererState.Outline.Stride = ModuleState.Surface.Stride;

    if (height < 0)
    {
        RendererState.Outline.Stride = -ModuleState.Surface.Stride;
        height = -height;
        RendererState.Outline.VerticalDirection = -RendererState.Outline.VerticalDirection;
    }

    PIXEL* dst = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL) + y * ModuleState.Surface.Stride + x * sizeof(PIXEL))
        + (ADDR)(ModuleState.Window.Y * ModuleState.Surface.Stride + ModuleState.Window.X * sizeof(PIXEL)));

    if ((RendererState.Outline.Options & OUTLINESKIPOPTIONS_TOP) == OUTLINESKIPOPTIONS_NONE)
    {
        height = height - 1;

        PIXEL* pixels = dst;

        if (src <= dst) { pixels = (PIXEL*)((ADDR)dst - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

        for (S32 xx = 0; xx < width; xx++)
        {
            pixels[RendererState.Outline.HorizontalDirection * xx] = pixel;
        }

        dst = (PIXEL*)((ADDR)dst + (ADDR)RendererState.Outline.Stride);
    }

    if ((RendererState.Outline.Options & OUTLINESKIPOPTIONS_RIGHT) == OUTLINESKIPOPTIONS_NONE)
    {
        S32 off = (width - 1) * sizeof(PIXEL);
        if (RendererState.Outline.HorizontalDirection < 0) { off = -off; }

        PIXEL* pixels = (PIXEL*)((ADDR)dst + (ADDR)off);

        for (S32 yy = 0; yy < height - 1; yy++)
        {
            if (src <= pixels) { pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

            pixels[0] = pixel;

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)RendererState.Outline.Stride);
        }
    }

    if ((RendererState.Outline.Options & OUTLINESKIPOPTIONS_LEFT) == OUTLINESKIPOPTIONS_NONE)
    {
        for (S32 yy = 0; yy < height - 1; yy++)
        {
            if (src <= dst) { dst = (PIXEL*)((ADDR)dst - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

            dst[0] = pixel;

            dst = (PIXEL*)((ADDR)dst + (ADDR)RendererState.Outline.Stride);
        }
    }
    else
    {
        dst = (PIXEL*)((ADDR)dst + (ADDR)(RendererState.Outline.Stride * (height - 1)));
    }

    if (height != 0 && (RendererState.Outline.Options & OUTLINESKIPOPTIONS_BOTTOM) == OUTLINESKIPOPTIONS_NONE)
    {
        if (src <= dst) { dst = (PIXEL*)((ADDR)dst - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

        for (S32 xx = 0; xx < width; xx++)
        {
            dst[RendererState.Outline.HorizontalDirection * xx] = pixel;
        }
    }
}

// 0x100026e0
VOID DrawStencilSurfaceWindowRectangle()
{
    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Stencil +
        (ADDR)(ModuleState.Surface.Offset + ModuleState.Window.Y * MAX_RENDERER_WIDTH + ModuleState.Window.X) * (ADDR)sizeof(PIXEL));

    CONST S32 height = ModuleState.Window.Height - ModuleState.Window.Y + 1;
    CONST S32 width = ModuleState.Window.Width - ModuleState.Window.X + 1;
    CONST S32 stride = (MAX_RENDERER_WIDTH - width) * sizeof(PIXEL);

    PIXEL pixel = (PIXEL)(ModuleState.Window.Y * STENCIL_PIXEL_COLOR_VALUE);

    if (ModuleState.Window.Y < ModuleState.Surface.Y)
    {
        CONST S32 delta = ModuleState.Window.Y + height - ModuleState.Surface.Y;

        if ((ModuleState.Window.Y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }

            pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

            pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
        }
    }
}

// 0x10002780
VOID MaskStencilSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Stencil
        + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    CONST S32 stride = (MAX_RENDERER_WIDTH - width) * sizeof(PIXEL);

    CONST PIXEL pixel = (PIXEL)STENCIL_PIXEL_MASK_VALUE;

    CONST S32 length = ACQUIREWIDTH(width);

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
            }

            pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
        }
    }
}

// 0x10002810
VOID MoveStencilSurface(S32 x, S32 y, S32 width, S32 height, S32 offset)
{
    CONST S32 stride = (MAX_RENDERER_WIDTH - width) * sizeof(PIXEL);

    DOUBLEPIXEL* pixels = (DOUBLEPIXEL*)((ADDR)RendererState.Surfaces.Stencil
        + (ADDR)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    CONST S32 length = ACQUIREWIDTH(width);

    if (-1 < offset)
    {
        CONST PIXEL pixel = (PIXEL)(offset << STENCIL_PIXEL_COLOR_SHIFT);
        CONST DOUBLEPIXEL pix = ((DOUBLEPIXEL)(pixel) << GRAPHICS_BITS_PER_PIXEL_16) | (DOUBLEPIXEL)pixel;

        if (y < ModuleState.Surface.Y)
        {
            CONST S32 delta = y + height - ModuleState.Surface.Y;

            if ((y + height) < ModuleState.Surface.Y || delta == 0)
            {
                for (S32 yy = 0; yy < height; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                    pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
                }
            }
            else
            {
                for (S32 yy = 0; yy < height - delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                    pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
                }

                pixels = (DOUBLEPIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

                for (S32 yy = 0; yy < delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                    pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
                }
            }
        }
        else
        {
            pixels = (DOUBLEPIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        CONST PIXEL pixel = (PIXEL)(-offset << STENCIL_PIXEL_COLOR_SHIFT);
        CONST DOUBLEPIXEL pix = ((DOUBLEPIXEL)(pixel) << GRAPHICS_BITS_PER_PIXEL_16) | (DOUBLEPIXEL)pixel;

        if (y < ModuleState.Surface.Y)
        {
            CONST S32 delta = y + height - ModuleState.Surface.Y;

            if ((y + height) < ModuleState.Surface.Y || delta == 0)
            {
                for (S32 yy = 0; yy < height; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                    pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
                }
            }
            else
            {
                for (S32 yy = 0; yy < height - delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                    pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
                }

                pixels = (DOUBLEPIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

                for (S32 yy = 0; yy < delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                    pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
                }
            }
        }
        else
        {
            pixels = (DOUBLEPIXEL*)((ADDR)pixels - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                pixels = (DOUBLEPIXEL*)((ADDR)pixels + (ADDR)(length * sizeof(PIXEL) + stride));
            }
        }
    }
}

// 0x100028f0
BOOL LockRendererSurface()
{
    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);

    HRESULT result = ModuleState.DirectX.Surface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

    while (TRUE)
    {
        if (SUCCEEDED(result))
        {
            ModuleState.Pitch = desc.lPitch;

            U32 offset = 0;

            if (!ModuleState.IsFullScreen)
            {
                RECT rect;
                ZeroMemory(&rect, sizeof(RECT));
                GetClientRect(ModuleState.HWND, &rect);

                POINT point;
                ZeroMemory(&point, sizeof(POINT));
                ClientToScreen(ModuleState.HWND, &point);

                OffsetRect(&rect, point.x, point.y);

                offset = desc.lPitch * rect.top + rect.left * (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL));
            }

            ModuleState.Surface.Renderer = (LPVOID)((ADDR)desc.lpSurface + (ADDR)offset);

            return TRUE;
        }

        if (result != DDERR_SURFACEBUSY && result != DDERR_SURFACELOST)
        {
            if (FAILED(ModuleState.DirectX.Surface->Restore())) { break; }
        }

        result = ModuleState.DirectX.Surface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);
    }

    return FALSE;
}

// 0x10002970
VOID UnlockRendererSurface()
{
    ModuleState.DirectX.Surface->Unlock(NULL);

    ModuleState.Surface.Renderer = NULL;
}

// 0x10002990
BOOL WriteRendererSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* pixels)
{
    BOOL locked = FALSE;

    if (ModuleState.Surface.Renderer == NULL)
    {
        if (!LockRendererSurface()) { return FALSE; }

        locked = TRUE;
    }

    PIXEL* src = (PIXEL*)((ADDR)pixels + (ADDR)((stride * sy + sx) * sizeof(PIXEL)));
    LPVOID dst = (LPVOID)((ADDR)ModuleState.Surface.Renderer
        + (ADDR)(ModuleState.Pitch * dy + dx * (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL))));

    CONST S32 length = ACQUIREWIDTH(width);

    for (S32 yy = 0; yy < height; yy++)
    {
        for (S32 xx = 0; xx < length; xx++)
        {
            if (RendererState.IsTrueColor) { ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]); }
            else { ((PIXEL*)dst)[xx] = src[xx]; }
        }

        src = (PIXEL*)((ADDR)src + (ADDR)(stride * sizeof(PIXEL)));
        dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
    }

    if (locked) { UnlockRendererSurface(); }

    return locked;
}

// 0x10002a30
VOID WriteSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 sstr, PIXEL* input, S32 dx, S32 dy, S32 dstr, PIXEL* output, S32 width, S32 height)
{
    PIXEL* src = (PIXEL*)((ADDR)input + (ADDR)((sstr * sy + sx) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((ADDR)output + (ADDR)((dstr * dy + dx) * sizeof(PIXEL)));

    for (s32 yy = 0; yy < height; yy++)
    {
        for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

        src = (PIXEL*)((ADDR)src + (ADDR)(sstr * sizeof(PIXEL)));
        dst = (PIXEL*)((ADDR)dst + (ADDR)(dstr * sizeof(PIXEL)));
    }
}

// 0x10002a90
BOOL WriteMainSurfaceRendererSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    BOOL locked = FALSE;

    if (ModuleState.Surface.Renderer == NULL)
    {
        if (!LockRendererSurface()) { return FALSE; }

        locked = TRUE;
    }

    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)(ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL));
    LPVOID dst = (LPVOID)((ADDR)ModuleState.Surface.Renderer
        + (ADDR)(ModuleState.Pitch * y + x * (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL))));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++)
                {
                    if (RendererState.IsTrueColor) { ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]); }
                    else { ((PIXEL*)dst)[xx] = src[xx]; }
                }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++)
                {
                    if (RendererState.IsTrueColor) { ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]); }
                    else { ((PIXEL*)dst)[xx] = src[xx]; }
                }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
            }

            src = (PIXEL*)((ADDR)src - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++)
                {
                    if (RendererState.IsTrueColor) { ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]); }
                    else { ((PIXEL*)dst)[xx] = src[xx]; }
                }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
            }
        }
    }
    else
    {
        src = (PIXEL*)((ADDR)src - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++)
            {
                if (RendererState.IsTrueColor) { ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]); }
                else { ((PIXEL*)dst)[xx] = src[xx]; }
            }

            src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
        }
    }

    if (locked) { UnlockRendererSurface(); }

    return locked;
}

// 0x10002b90
BOOL FUN_10002b90(S32 x, S32 y, S32 width, S32 height)
{
    // TODO NOT IMPLEMENTED

    return TRUE;
}

// 0x10002fb0
VOID FUN_10002fb0(S32 x, S32 y, S32 width, S32 height)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10003320
S32 AcquireTextLength(LPCSTR text, BINASSETCONTENTPTR asset)
{
    S32 result = 0;

    for (U32 xx = 0; text[xx] != NULL; xx++)
    {
        result = result + DEFAULT_FONT_ASSET_SPACING + ACQUIRETEXTWIDTH(asset, text[xx]);
    }

    return result;
}

// 0x10003360
VOID DrawMainSurfaceText(S32 x, S32 y, LPCSTR text, BINASSETCONTENTPTR asset, PIXEL* palette)
{
    U32 offset = 0;

    for (U32 xx = 0; text[xx] != NULL; xx++)
    {
        CONST IMAGEPALETTESPRITEPTR image = (IMAGEPALETTESPRITEPTR)((ADDR)asset + (ADDR)asset->Offset[text[xx]]);

        DrawMainSurfacePaletteSprite(x + offset, y, palette, image);

        offset = offset + DEFAULT_FONT_ASSET_SPACING + ACQUIRETEXTWIDTH(asset, text[xx]);
    }
}

// 0x100033c0
VOID DrawBackSurfaceText(S32 x, S32 y, LPCSTR text, BINASSETCONTENTPTR asset, PIXEL* palette)
{
    U32 offset = 0;

    for (U32 xx = 0; text[xx] != NULL; xx++)
    {
        CONST IMAGEPALETTESPRITEPTR image = (IMAGEPALETTESPRITEPTR)((ADDR)asset + (ADDR)asset->Offset[text[xx]]);

        DrawBackSurfacePaletteShadeSprite(x + offset, y, y, palette, image);

        offset = offset + DEFAULT_FONT_ASSET_SPACING + ACQUIRETEXTWIDTH(asset, text[xx]);
    }
}

// 0x10004390
VOID FUN_10004390(S32 param_1, S32 param_2, LPVOID param_3)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100046b6
VOID FUN_100046b6(S32 param_1, S32 param_2, LPVOID param_3)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100049e6
VOID FUN_100049e6(S32 param_1, S32 param_2, U16 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10004db0
VOID DrawMainSurfaceAnimationSpriteVersion0(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100050df
VOID DrawMainSurfacePaletteSprite(S32 x, S32 y, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Window.X = ModuleState.Window.X;
    RendererState.Sprite.Window.Y = ModuleState.Window.Y;
    RendererState.Sprite.Window.Width = ModuleState.Window.Width;
    RendererState.Sprite.Window.Height = ModuleState.Window.Height;

    RendererState.Sprite.Height = sprite->Height;
    RendererState.Sprite.Width = sprite->Width + 1;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    y = y + sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height + y - ModuleState.Window.Y;

        if (RendererState.Sprite.Height <= 0 || RendererState.Sprite.Height == ModuleState.Window.Y - y) { return; }

        for (S32 x = 0; x < ModuleState.Window.Y - y; x++)
        {
            content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
            next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = RendererState.Sprite.Height + y - ModuleState.Window.Height - 1;

    BOOL draw = overflow == 0 || (RendererState.Sprite.Height + y < ModuleState.Window.Height + 1);

    if (!draw)
    {
        CONST S32 height = RendererState.Sprite.Height;

        RendererState.Sprite.Height = RendererState.Sprite.Height - overflow;

        draw = RendererState.Sprite.Height != 0 && overflow <= height;
    }

    if (draw)
    {
        RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((x + sprite->X) * sizeof(PIXEL)));
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)(ModuleState.Window.X * sizeof(PIXEL)));
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((ModuleState.Window.Width + 1) * sizeof(PIXEL)));

        CONST S32 overage = RendererState.Sprite.Height + y < ModuleState.Surface.Y
            ? 0 : (RendererState.Sprite.Height + y - ModuleState.Surface.Y);

        RendererState.Sprite.Overage = overage;
        RendererState.Sprite.Height = RendererState.Sprite.Height - overage;

        if (RendererState.Sprite.Height == 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                U32 skip = 0;
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX)
                {
                    CONST U32 need = (U32)((ADDR)RendererState.Sprite.MinX - (ADDR)sx) / sizeof(PIXEL);
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        if (count <= need) { pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL)); }
                    }
                    else
                    {
                        if (count <= need) { pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)); }
                    }

                    skip = count == need ? 0 : Min(count, need);
                    sx = (PIXEL*)((ADDR)sx + (ADDR)(Min(count, need) * sizeof(PIXEL)));
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST U8 indx = pixels->Pixels[0];

                        if (indx != 0)
                        {
                            CONST PIXEL pixel = palette[indx];

                            for (U32 x = 0; x < count - skip; x++)
                            {
                                if (((ADDR)sx + (ADDR)(x * sizeof(PIXEL))) < (ADDR)RendererState.Sprite.MaxX) { sx[x] = pixel; }
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (U32 x = 0; x < count - skip; x++)
                        {
                            CONST U8 indx = pixels->Pixels[skip + x];

                            if (indx != 0)
                            {
                                if (((ADDR)sx + (ADDR)(x * sizeof(PIXEL))) < (ADDR)RendererState.Sprite.MaxX) { sx[x] = palette[indx]; }
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)((count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)));
                    }

                    sx = (PIXEL*)((ADDR)sx + (ADDR)((count - skip) * sizeof(PIXEL)));

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
                next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));

                RendererState.Sprite.Height = RendererState.Sprite.Height - 1;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }
    }
}

// 0x100053c3
VOID FUN_100053c3(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x1000579c
VOID FUN_1000579c(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10005ac6
VOID FUN_10005ac6(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10005e31
VOID DrawBackSurfacePaletteShadeSprite(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Window.X = ModuleState.Window.X;
    RendererState.Sprite.Window.Y = ModuleState.Window.Y;
    RendererState.Sprite.Window.Width = ModuleState.Window.Width;
    RendererState.Sprite.Window.Height = ModuleState.Window.Height;

    level = (level + 0x440) * 0x20;

    RendererState.Sprite.Height = sprite->Height;
    RendererState.Sprite.Width = sprite->Width + 1;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    y = y + sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height + y - ModuleState.Window.Y;

        if (RendererState.Sprite.Height <= 0 || RendererState.Sprite.Height == ModuleState.Window.Y - y) { return; }

        for (S32 x = 0; x < ModuleState.Window.Y - y; x++)
        {
            content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
            next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = RendererState.Sprite.Height + y - ModuleState.Window.Height - 1;

    BOOL draw = overflow == 0 || (RendererState.Sprite.Height + y < ModuleState.Window.Height + 1);

    if (!draw)
    {
        CONST S32 height = RendererState.Sprite.Height;

        RendererState.Sprite.Height = RendererState.Sprite.Height - overflow;

        draw = RendererState.Sprite.Height != 0 && overflow <= height;
    }

    if (draw)
    {
        RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Surfaces.Back
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((x + sprite->X) * sizeof(PIXEL)));
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Surfaces.Back
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)(ModuleState.Window.X * sizeof(PIXEL)));
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Surfaces.Back
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((ModuleState.Window.Width + 1) * sizeof(PIXEL)));

        CONST S32 overage = RendererState.Sprite.Height + y < ModuleState.Surface.Y
            ? 0 : (RendererState.Sprite.Height + y - ModuleState.Surface.Y);

        RendererState.Sprite.Overage = overage;
        RendererState.Sprite.Height = RendererState.Sprite.Height - overage;

        if (RendererState.Sprite.Height == 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                U32 skip = 0;
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX)
                {
                    CONST U32 need = (U32)((ADDR)RendererState.Sprite.MinX - (ADDR)sx) / sizeof(PIXEL);
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        if (count <= need) { pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL)); }
                    }
                    else
                    {
                        if (count <= need) { pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)); }
                    }

                    skip = count == need ? 0 : Min(count, need);
                    sx = (PIXEL*)((ADDR)sx + (ADDR)(Min(count, need) * sizeof(PIXEL)));
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST U8 indx = pixels->Pixels[0];

                        if (indx != 0)
                        {
                            PIXEL pixel = palette[indx];

                            CONST ADDR offset = (ADDR)sx - (ADDR)RendererState.Surfaces.Back;
                            PIXEL* stencil = (PIXEL*)((ADDR)RendererState.Surfaces.Stencil + offset);

                            for (U32 x = 0; x < count - skip; x++)
                            {
                                CONST DOUBLEPIXEL spix = stencil[x];
                                stencil[x] = (spix & 3) | level;

                                if (spix & 2)
                                {
                                    pixel = SHADEPIXEL(pixel, ModuleState.ShadeColorMask) + ModuleState.BackSurfaceShadePixel;
                                }

                                sx[x] = (PIXEL)pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (U32 x = 0; x < count - skip; x++)
                        {
                            CONST U8 indx = pixels->Pixels[skip + x];

                            if (indx != 0)
                            {
                                PIXEL pixel = palette[indx];

                                CONST ADDR offset = (ADDR)sx - (ADDR)RendererState.Surfaces.Back;
                                PIXEL* stencil = (PIXEL*)((ADDR)RendererState.Surfaces.Stencil + offset);

                                CONST PIXEL spix = stencil[skip + x];
                                stencil[skip + x] = (spix & 3) | level;

                                if (spix & 2)
                                {
                                    pixel = SHADEPIXEL(pixel, ModuleState.ShadeColorMask) + ModuleState.BackSurfaceShadePixel;
                                }

                                sx[x] = (PIXEL)pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)((count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)));
                    }

                    sx = (PIXEL*)((ADDR)sx + (ADDR)((count - skip) * sizeof(PIXEL)));

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
                next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));

                RendererState.Sprite.Height = RendererState.Sprite.Height - 1;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }
    }
}

// 0x1000618d
VOID FUN_1000618d(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100064b6
VOID DrawMainSurfaceSprite(S32 x, S32 y, IMAGESPRITEPTR sprite)
{
    RendererState.Sprite.Window.X = ModuleState.Window.X;
    RendererState.Sprite.Window.Y = ModuleState.Window.Y;
    RendererState.Sprite.Window.Width = ModuleState.Window.Width;
    RendererState.Sprite.Window.Height = ModuleState.Window.Height;

    RendererState.Sprite.Height = sprite->Height;
    RendererState.Sprite.Width = sprite->Width + 1;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    y = y + sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height + y - ModuleState.Window.Y;

        if (RendererState.Sprite.Height <= 0 || RendererState.Sprite.Height == ModuleState.Window.Y - y) { return; }

        for (S32 x = 0; x < ModuleState.Window.Y - y; x++)
        {
            content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
            next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = RendererState.Sprite.Height + y - ModuleState.Window.Height - 1;

    BOOL draw = overflow == 0 || (RendererState.Sprite.Height + y < ModuleState.Window.Height + 1);

    if (!draw)
    {
        CONST S32 height = RendererState.Sprite.Height;

        RendererState.Sprite.Height = RendererState.Sprite.Height - overflow;

        draw = RendererState.Sprite.Height != 0 && overflow <= height;
    }

    if (draw)
    {
        RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((x + sprite->X) * sizeof(PIXEL)));
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)(ModuleState.Window.X * sizeof(PIXEL)));
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((ModuleState.Window.Width + 1) * sizeof(PIXEL)));

        CONST S32 overage = RendererState.Sprite.Height + y < ModuleState.Surface.Y
            ? 0 : (RendererState.Sprite.Height + y - ModuleState.Surface.Y);

        RendererState.Sprite.Overage = overage;
        RendererState.Sprite.Height = RendererState.Sprite.Height - overage;

        if (RendererState.Sprite.Height == 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                U32 skip = 0;
                IMAGESPRITEPIXELPTR pixels = (IMAGESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX)
                {
                    CONST U32 need = (U32)((ADDR)RendererState.Sprite.MinX - (ADDR)sx) / sizeof(PIXEL);
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        if (count <= need) { pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGESPRITEPIXEL)); }
                    }
                    else
                    {
                        if (count <= need) { pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + (count - 1) * sizeof(PIXEL) + sizeof(IMAGESPRITEPIXEL)); }
                    }

                    skip = count == need ? 0 : Min(count, need);
                    sx = (PIXEL*)((ADDR)sx + (ADDR)(Min(count, need) * sizeof(PIXEL)));
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count == 0)
                    {
                        pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGESPRITEPIXEL));
                    }
                    else if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST PIXEL pixel = pixels->Pixels[0];

                        if (pixel != MAGENTA_PIXEL)
                        {
                            for (U32 x = 0; x < count - skip; x++)
                            {
                                if (((ADDR)sx + (ADDR)(x * sizeof(PIXEL))) < (ADDR)RendererState.Sprite.MaxX) { sx[x] = pixel; }
                            }
                        }

                        pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGESPRITEPIXEL));
                    }
                    else
                    {
                        for (U32 x = 0; x < count - skip; x++)
                        {
                            CONST PIXEL pixel = pixels->Pixels[skip + x];

                            if (((ADDR)sx + (ADDR)(x * sizeof(PIXEL))) < (ADDR)RendererState.Sprite.MaxX) { if (pixel != MAGENTA_PIXEL) { sx[x] = pixel; } }
                        }

                        pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + (ADDR)((count - 1) * sizeof(PIXEL) + sizeof(IMAGESPRITEPIXEL)));
                    }

                    sx = (PIXEL*)((ADDR)sx + (ADDR)((count - skip) * sizeof(PIXEL)));

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
                next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));

                RendererState.Sprite.Height = RendererState.Sprite.Height - 1;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }
    }
}

// 0x100067ad
VOID FUN_100067ad(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10006b21
VOID DrawMainSurfaceAnimationSpriteVersion2(S32 x, S32 y, U16 level, LPVOID pal, IMAGEPALETTESPRITEPTR sprite)
{
    ANIMATIONPIXEL* palette = (ANIMATIONPIXEL*)pal;

    RendererState.Sprite.ColorMask =
        (((U32)ModuleState.ActualGreenMask) << 16) | ModuleState.ActualRedMask | ModuleState.ActualBlueMask;
    RendererState.Sprite.AdjustedColorMask = (RendererState.Sprite.ColorMask << 1) | RendererState.Sprite.ColorMask;

    RendererState.Sprite.Window.X = ModuleState.Window.X;
    RendererState.Sprite.Window.Y = ModuleState.Window.Y;
    RendererState.Sprite.Window.Width = ModuleState.Window.Width;
    RendererState.Sprite.Window.Height = ModuleState.Window.Height;

    level = (level + 0x440) * 0x20;
    CONST DOUBLEPIXEL stencil = (level << 16) | level;

    RendererState.Sprite.Height = sprite->Height;
    RendererState.Sprite.Width = sprite->Width + 1;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    y = y + sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height + y - ModuleState.Window.Y;

        if (RendererState.Sprite.Height <= 0 || RendererState.Sprite.Height == ModuleState.Window.Y - y) { return; }

        for (S32 x = 0; x < ModuleState.Window.Y - y; x++)
        {
            content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
            next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = RendererState.Sprite.Height + y - ModuleState.Window.Height - 1;

    BOOL draw = overflow == 0 || (RendererState.Sprite.Height + y < ModuleState.Window.Height + 1);

    if (!draw)
    {
        CONST S32 height = RendererState.Sprite.Height;

        RendererState.Sprite.Height = RendererState.Sprite.Height - overflow;

        draw = RendererState.Sprite.Height != 0 && overflow <= height;
    }

    if (draw)
    {
        RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((x + sprite->X) * sizeof(PIXEL)));
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)(ModuleState.Window.X * sizeof(PIXEL)));
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + (ADDR)(ModuleState.Surface.Offset * sizeof(PIXEL)) + (ADDR)(ModuleState.Surface.Stride * y) + (ADDR)((ModuleState.Window.Width + 1) * sizeof(PIXEL)));

        CONST S32 overage = RendererState.Sprite.Height + y < ModuleState.Surface.Y
            ? 0 : (RendererState.Sprite.Height + y - ModuleState.Surface.Y);

        RendererState.Sprite.Overage = overage;
        RendererState.Sprite.Height = RendererState.Sprite.Height - overage;

        if (RendererState.Sprite.Height == 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                U32 skip = 0;
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX)
                {
                    CONST U32 need = (U32)((ADDR)RendererState.Sprite.MinX - (ADDR)sx) / sizeof(PIXEL);
                    CONST U32 count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        if (count <= need) { pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL)); }
                    }
                    else
                    {
                        if (count <= need) { pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)); }
                    }

                    skip = count == need ? 0 : Min(count, need);
                    sx = (PIXEL*)((ADDR)sx + (ADDR)(Min(count, need) * sizeof(PIXEL)));
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    U32 count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Skip pixels that are below required stencil value.
                        {
                            CONST ADDR offset = (ADDR)sx - (ADDR)RendererState.Surfaces.Main;
                            while (stencil <= *(DOUBLEPIXEL*)((ADDR)RendererState.Surfaces.Stencil + offset + (ADDR)(x + skip) * (ADDR)sizeof(PIXEL)))
                            {
                                count = count - 1;
                                skip = skip + 1;

                                if (count == 0) { break; }
                            }
                        }

                        // Blend and draw pixels.
                        if (count != 0)
                        {
                            CONST U8 indx = pixels->Pixels[0];
                            CONST ANIMATIONPIXEL pixel = palette[indx];

                            CONST DOUBLEPIXEL pix = pixel >> 19;

                            if ((pix & 0xFF) != 0x1F)
                            {
                                for (U32 x = 0; x < count - skip; x++)
                                {
                                    CONST DOUBLEPIXEL value =
                                        (pix * (((sx[x] << 16) | sx[x]) & RendererState.Sprite.ColorMask) >> 5) & RendererState.Sprite.ColorMask;

                                    sx[x] = (PIXEL)((value >> 16) | value) + (PIXEL)pixel;
                                }
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        // Skip pixels that are below required stencil value.
                        {
                            CONST ADDR offset = (ADDR)sx - (ADDR)RendererState.Surfaces.Main;
                            while (stencil <= *(DOUBLEPIXEL*)((ADDR)RendererState.Surfaces.Stencil + offset + (ADDR)(x + skip) * (ADDR)sizeof(PIXEL)))
                            {
                                count = count - 1;
                                skip = skip + 1;

                                if (count == 0) { break; }
                            }
                        }

                        // Blend and draw pixels.
                        if (count != 0)
                        {
                            for (U32 x = 0; x < count - skip; x++)
                            {
                                CONST U8 indx = pixels->Pixels[skip + x];
                                CONST ANIMATIONPIXEL pixel = palette[indx];

                                CONST DOUBLEPIXEL pix = pixel >> 19;

                                if ((pix & 0xFF) != 0x1F)
                                {
                                    CONST DOUBLEPIXEL value =
                                        (pix * (((sx[x] << 16) | sx[x]) & RendererState.Sprite.ColorMask) >> 5) & RendererState.Sprite.ColorMask;

                                    sx[x] = (PIXEL)((value >> 16) | value) + (PIXEL)pixel;
                                }
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)((count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)));
                    }

                    sx = (PIXEL*)((ADDR)sx + (ADDR)((count - skip) * sizeof(PIXEL)));

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + (ADDR)sizeof(U16));
                next = (LPVOID)((ADDR)next + (ADDR)(((U16*)next)[0] + sizeof(U16)));

                RendererState.Sprite.Height = RendererState.Sprite.Height - 1;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        }
    }
}

// 0x10006ef8
VOID DrawMainSurfaceAnimationSpriteVersion1A(S32 x, S32 y, U16 level, LPVOID param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007292
VOID DrawMainSurfaceAnimationSpriteVersion1B(S32 x, S32 y, U16 level, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007662
VOID DrawMainSurfaceAnimationSpriteVersion3(S32 x, S32 y, LPVOID pal, IMAGEPALETTESPRITEPTR sprite)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
    // TODO Use of 3rd data type is weird!
}

// 0x10007928
VOID FUN_10007928(S32 x, S32 y, S32 level, LPVOID sprite)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007be8
VOID FUN_10007be8(S32 x, S32 y, U16 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007fbc
VOID DrawMainSurfaceAnimationSpriteVersion4(S32 x, S32 y, U16 level, LPVOID pal, IMAGEPALETTESPRITEPTR sprite)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
    // TODO Use of 3rd data type is weird!
}

// 0x10008ecd
VOID FUN_10008ecd(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10009eb3
VOID FUN_10009eb3(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x1000a4f3
VOID FUN_1000a4f3(S32 param_1, S32 param_2, S32 param_3, S32 param_4, LPVOID param_5, LPVOID param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}
