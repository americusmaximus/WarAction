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
#define STENCIL_PIXEL_OFFSET            0x440
#define STENCIL_PIXEL_MASK_VALUE        0xFFFB

RENDERERSTATECONTAINER RendererState;

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

#if ACTIVATE_MODERN_GRAPHICS_MODE
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

    ModuleState.InvertedActualColorMask = ~ModuleState.ActualColorMask;
    ModuleState.InvertedActualColorMaskCopy = ~ModuleState.ActualColorMask;

    ModuleState.ActualColorBitsCopy = ModuleState.ActualColorBits;

    ModuleState.ShadeColorMask = ~ModuleState.ActualColorBits;
    ModuleState.ShadeColorMaskCopy = ~ModuleState.ActualColorBits;

    ModuleState.ShadePixel =
        ((5 << (11 - ModuleState.BlueOffset)) + (2 << (11 - ModuleState.GreenOffset))) & DEFAULT_SCREEN_COLOR_MASK;
    ModuleState.ShadePixel = (ModuleState.ShadePixel << 16) | ModuleState.ShadePixel;

    if (ModuleState.GreenOffset < ModuleState.RedOffset)
    {
        if (ModuleState.BlueOffset < ModuleState.GreenOffset)
        {
            ModuleState.InitialRGBMask = (gm << 16) | bm | rm;
        }
        else if (ModuleState.BlueOffset <= ModuleState.RedOffset)
        {
            ModuleState.InitialRGBMask = (bm << 16) | gm | rm;
        }
        else { ModuleState.InitialRGBMask = (rm << 16) | bm | gm; }
    }
    else
    {
        if (ModuleState.GreenOffset <= ModuleState.BlueOffset)
        {
            ModuleState.InitialRGBMask = (gm << 16) | bm | rm;
        }
        else if (ModuleState.RedOffset < ModuleState.BlueOffset)
        {
            ModuleState.InitialRGBMask = (bm << 16) | gm | rm;
        }
        else { ModuleState.InitialRGBMask = (rm << 16) | bm | gm; }
    }

    ModuleState.ActualRGBMask = ((ModuleState.ActualColorMask << 16) | ModuleState.ActualColorMask) & ModuleState.InitialRGBMask;
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

#if ACTIVATE_MODERN_GRAPHICS_MODE
        if (FAILED(ModuleState.DirectX.Instance->SetDisplayMode(width, height, bits, 0, 0))) { return FALSE; }
#else
        if (FAILED(ModuleState.DirectX.Instance->SetDisplayMode(width, height, bits))) { return FALSE; }
#endif

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

#if ACTIVATE_MODERN_GRAPHICS_MODE
    DDSURFACEDESC2 desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

    desc.dwSize = sizeof(DDSURFACEDESC2);
#else
    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);
#endif

    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if (FAILED(ModuleState.DirectX.Instance->CreateSurface(&desc, &ModuleState.DirectX.Surface, NULL))) { return FALSE; }

    if (RendererState.IsTrueColor)
    {
        // Report RGB565 in case the game runs on 32-bit pixel depth system.
        SetPixelColorMasks(0xF800, 0x07E0, 0x001F);
    }
    else
    {
        if (FAILED(ModuleState.DirectX.Surface->GetSurfaceDesc(&desc))) { return FALSE; }

        SetPixelColorMasks(desc.ddpfPixelFormat.dwRBitMask,
            desc.ddpfPixelFormat.dwGBitMask, desc.ddpfPixelFormat.dwBBitMask);
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
    S32 max_x = x + length - 1;
    S32 new_x = x;

    if (y >= ModuleState.Window.Y && y <= ModuleState.Window.Height)
    {
        if (new_x < ModuleState.Window.X)
            new_x = ModuleState.Window.X;

        if (ModuleState.Window.Width < max_x)
            max_x = ModuleState.Window.Width;

        if (new_x <= max_x)
        {
            PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main
                + ModuleState.Surface.Offset + (y * MAX_RENDERER_WIDTH + new_x) * sizeof(PIXEL));

            if (ModuleState.Surface.Y <= y)
                pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

            for (S32 xx = 0; xx < max_x - new_x + 1; xx++) { pixels[xx] = pixel; }
        }
    }
}

// 0x100014b0
VOID DrawMainSurfaceVerticalColorLine(S32 x, S32 y, S32 height, PIXEL pixel)
{
    S32 max_y = height + y - 1;
    S32 new_y = y;

    if (x < ModuleState.Window.X || ModuleState.Window.Width < x)
        return;

    if (y < ModuleState.Window.Y)
        new_y = ModuleState.Window.Y;

    if (max_y > ModuleState.Window.Height)
        max_y = ModuleState.Window.Height;

    max_y += 1 - new_y;

    if (max_y < 1)
        return;

    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + ModuleState.Surface.Offset + (ADDR)(new_y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = new_y + max_y - ModuleState.Surface.Y;

        if (delta < 1)
        {
            for (S32 xx = 0; xx < max_y; ++xx)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 xx = 0; xx < max_y - delta; ++xx)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

            for (S32 xx = 0; xx < delta; ++xx)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

        for (S32 xx = 0; xx < max_y; ++xx)
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
        width += x - ModuleState.Window.X;
        x = ModuleState.Window.X;
    }

    if (y < ModuleState.Window.Y)
    {
        height += y - ModuleState.Window.Y;
        y = ModuleState.Window.Y;
    }

    if ((width + x - 1) > ModuleState.Window.Width)
        width = ModuleState.Window.Width - x + 1;

    if ((height + y - 1) > ModuleState.Window.Height)
        height = ModuleState.Window.Height - y + 1;

    if (width < 1 || height < 1)
        return;

    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main + ModuleState.Surface.Offset + (ADDR)(y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if (y + height < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; ++yy)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; ++yy)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

            for (S32 yy = 0; yy < delta; ++yy)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

        for (S32 yy = 0; yy < height; ++yy)
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

        if (width < 1)
            return;
    }

    if (y < ModuleState.Window.Y)
    {
        height = height + y - ModuleState.Window.Y;
        y = ModuleState.Window.Y;

        if (height < 1)
            return;
    }

    if ((width + x - 1) > ModuleState.Window.Width)
        width = ModuleState.Window.Width - x + 1;

    if ((height + y - 1) > ModuleState.Window.Height)
        height = ModuleState.Window.Height - y + 1;

    if (width < 1 || height < 1)
        return;

    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Main + ModuleState.Surface.Offset + (ADDR)(y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL));

    CONST PIXEL color = SHADEPIXEL(pixel, ModuleState.ShadeColorMask);

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx)
                {
                    pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color;
                }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx)
                {
                    pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color;
                }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

            for (S32 yy = 0; yy < delta; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx)
                {
                    pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color;
                }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

        for (S32 yy = 0; yy < height; ++yy)
        {
            for (S32 xx = 0; xx < width; ++xx)
            {
                pixels[xx] = SHADEPIXEL(pixels[xx], ModuleState.ShadeColorMask) + color;
            }

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x100017e0
VOID DrawMainSurfaceColorPoint(S32 x, S32 y, PIXEL pixel)
{
    if (x > ModuleState.Window.X
        && y > ModuleState.Window.Y
        && x <= ModuleState.Window.Width
        && y <= ModuleState.Window.Height)
    {
        S32 offset = ModuleState.Surface.Offset / sizeof(PIXEL) + y * MAX_RENDERER_WIDTH + x;

        if (ModuleState.Surface.Y <= y)
            offset -= SCREEN_SIZE_IN_PIXELS;

        RendererState.Surfaces.Main[offset] = pixel;
    }
}

// 0x10001840
VOID DrawBackSurfaceColorPoint(S32 x, S32 y, PIXEL pixel)
{
    if (x > ModuleState.Window.X
        && y > ModuleState.Window.Y
        && x <= ModuleState.Window.Width
        && y <= ModuleState.Window.Height)
    {
        S32 offset = ModuleState.Surface.Offset / sizeof(PIXEL) + y * MAX_RENDERER_WIDTH + x;

        if (ModuleState.Surface.Y <= y)
            offset -= SCREEN_SIZE_IN_PIXELS;

        RendererState.Surfaces.Back[offset] = pixel;
    }
}

// 0x100018a0
VOID ReadMainSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* surface)
{
    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Main + ModuleState.Surface.Offset + (sy * MAX_RENDERER_WIDTH + sx) * sizeof(PIXEL));
    PIXEL* dst = (PIXEL*)((ADDR)surface + (stride * dy + dx) * sizeof(PIXEL));

    if (sy < ModuleState.Surface.Y)
    {
        CONST S32 delta = sy + height - ModuleState.Surface.Y;

        if (delta <= 0)
        {
            for (S32 yy = 0; yy < height; ++yy)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; ++yy)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
            }

            src = (PIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);

            for (S32 yy = 0; yy < delta; ++yy)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        src = (PIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);

        for (S32 yy = 0; yy < height; ++yy)
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
VOID OffsetSurfaces(S32 dx, S32 dy)
{
    PIXEL* src;
    PIXEL* dst;

    S32 offset = ModuleState.Surface.Offset / sizeof(PIXEL) + dy * MAX_RENDERER_WIDTH + dx;
    if (offset < 0)
    {
        do
        {
            offset += SCREEN_SIZE_IN_PIXELS;
        } while (offset < 0);
    }
    else
    {
        while (offset >= SCREEN_SIZE_IN_PIXELS)
            offset -= SCREEN_SIZE_IN_PIXELS;
    }

    S32 x_max = dx + (ModuleState.Surface.Offset / sizeof(PIXEL)) % MAX_RENDERER_WIDTH;
    do
    {
        if (x_max >= 0)
        {
            if (x_max < MAX_RENDERER_WIDTH)
                break;

            src = ModuleState.Surface.Back + SCREEN_SIZE_IN_PIXELS;
            dst = ModuleState.Surface.Back;

            for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++) { dst[xx] = src[xx]; }

            src = ModuleState.Surface.Main + SCREEN_SIZE_IN_PIXELS;
            dst = ModuleState.Surface.Main;

            for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++) { dst[xx] = src[xx]; }

            src = ModuleState.Surface.Stencil + SCREEN_SIZE_IN_PIXELS;
            dst = ModuleState.Surface.Stencil;
        }
        else
        {
            src = ModuleState.Surface.Back;
            dst = ModuleState.Surface.Back + SCREEN_SIZE_IN_PIXELS;

            for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++) { dst[xx] = src[xx]; }

            src = ModuleState.Surface.Main;
            dst = ModuleState.Surface.Main + SCREEN_SIZE_IN_PIXELS;

            for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++) { dst[xx] = src[xx]; }

            src = ModuleState.Surface.Stencil;
            dst = ModuleState.Surface.Stencil + SCREEN_SIZE_IN_PIXELS;
        }

        for (U32 xx = 0; xx < MAX_RENDERER_WIDTH; xx++) { dst[xx] = src[xx]; }
    } while (FALSE);

    ModuleState.Surface.Offset = offset * sizeof(PIXEL);
    ModuleState.Surface.Y = MAX_RENDERER_HEIGHT - offset / MAX_RENDERER_WIDTH;

    if (dy <= 0)
    {
        if (dy < 0)
        {
            if (dx <= 0)
                MoveStencilSurface(-dx, -dy, dx + MAX_RENDERER_WIDTH, dy + MAX_RENDERER_HEIGHT, -dy);
            else
                MoveStencilSurface(0, -dy, MAX_RENDERER_WIDTH - dx, dy + MAX_RENDERER_HEIGHT, -dy);
        }
    }
    else
    {
        if (dx <= 0)
            MoveStencilSurface(-dx, 0, dx + MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT - dy, -dy);
        else
            MoveStencilSurface(0, 0, MAX_RENDERER_WIDTH - dx, MAX_RENDERER_HEIGHT - dy, -dy);
    }
}

// 0x10001e90
VOID DrawBackSurfaceRhomb(S32 tx, S32 ty, S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, IMAGEPALETTETILEPTR tile)
{
    DrawSurfaceRhomb(angle_0, angle_1, angle_2, angle_3, tx, ty,
        ModuleState.Surface.Width * sizeof(PIXEL), tile, RendererState.Surfaces.Back);
}

// 0x10001ed0
VOID ShadeMainSurfaceRhomb(S32 x, S32 y, S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3)
{
    ShadeSurfaceRhomb(angle_0, angle_1, angle_2, angle_3, x, y,
        ModuleState.Surface.Width * sizeof(PIXEL), RendererState.Surfaces.Main);
}

// 0x10001f10
VOID DrawMainSurfaceMaskRhomb(S32 x, S32 y, S32 color)
{
    DrawSurfaceMaskRhomb(x, y, ModuleState.Surface.Width * sizeof(PIXEL), color, RendererState.Surfaces.Main);
}

// 0x10001f40
VOID CleanMainSurfaceRhomb(S32 x, S32 y, S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, IMAGEPALETTETILEPTR tile)
{
    CleanSurfaceRhomb(angle_0, angle_1, angle_2, angle_3, x, y,
        ModuleState.Surface.Width * sizeof(PIXEL), tile, RendererState.Surfaces.Main);
}

// 0x10001f80
VOID WriteBackSurfaceMainSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    CONST ADDR offset = (ADDR)(x + y * MAX_RENDERER_WIDTH);
    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Back + ModuleState.Surface.Offset + offset * sizeof(PIXEL));
    PIXEL* dst = (PIXEL*)((ADDR)RendererState.Surfaces.Main + ModuleState.Surface.Offset + offset * sizeof(PIXEL));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;
        if (delta <= 0)
        {
            S32 lines = height;

            while (lines--)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            S32 lines = height - delta;

            while (lines--)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            src = (PIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);
            dst = (PIXEL*)((ADDR)dst - SCREEN_SIZE_IN_BYTES);

            lines = delta;

            while (lines--)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((ADDR)dst + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        src = (PIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);
        dst = (PIXEL*)((ADDR)dst - SCREEN_SIZE_IN_BYTES);

        S32 lines = height;

        while (lines--)
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
    CONST S32 offset = (ModuleState.Surface.Offset / sizeof(PIXEL)) % MAX_RENDERER_WIDTH;

    PIXEL* src = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + (ADDR)((offset + SCREEN_SIZE_IN_PIXELS) * sizeof(PIXEL)));

    RendererState.Outline.Width = ModuleState.Window.Width - ModuleState.Window.X;
    RendererState.Outline.Height = ModuleState.Window.Height + 1 - ModuleState.Window.Y;
    RendererState.Outline.Options = OUTLINEDRAWOPTION_NONE;

    x = x - ModuleState.Window.X;
    y = y - ModuleState.Window.Y;

    if (y < 0)
    {
        height = height + y;

        if (height <= 0)
            return;

        y = 0;
        RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_TOP);
    }

    if (y >= RendererState.Outline.Height)
    {
        height = height + y - (RendererState.Outline.Height - 1);

        if (height >= 0)
            return;

        y = RendererState.Outline.Height - 1;
        RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_TOP);
    }

    {
        CONST S32 max = y + 1 + height;

        if (y + 1 + height < 0 != max < 0)
        {
            height = height - max - 1;
            RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_BOTTOM);
        }
    }

    {
        CONST S32 max = y - 1 + height;

        if (max >= RendererState.Outline.Height)
        {
            height = height + RendererState.Outline.Height - max;
            RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_BOTTOM);
        }
    }

    if (x < 1)
    {
        width = width + x;

        if (width < 2)
            return;

        x = 1;
        width = width - 1;

        RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_LEFT);
    }

    if (x >= RendererState.Outline.Width + 2)
    {
        width = width + x + 1 - (RendererState.Outline.Width + 2);
        if (width >= 0)
            return;
        x = RendererState.Outline.Width + 1;
        RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_LEFT);
    }

    if (x + width <= 0 != x + width < 0)
    {
        width = width - x - width;

        RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_RIGHT);
    }

    {
        S32 max = x - 2 + width;

        if (max > RendererState.Outline.Width)
        {
            width = width + RendererState.Outline.Width - max;
            RendererState.Outline.Options = (OutlintDrawOption)(RendererState.Outline.Options | OUTLINEDRAWOPTION_RIGHT);
        }
    }

    // Offset in bytes to the next changed pixel. Since we use Pixel*, I changed it to 1
    RendererState.Outline.HorizontalStride = 1;
    if (width < 0)
    {
        RendererState.Outline.HorizontalStride = -RendererState.Outline.HorizontalStride;
        width = -width;
    }

    RendererState.Outline.VerticalStride = 1;
    RendererState.Outline.Stride = ModuleState.Surface.Stride;
    if (height < 0)
    {
        RendererState.Outline.Stride = -ModuleState.Surface.Stride;
        height = -height;
        RendererState.Outline.VerticalStride = -RendererState.Outline.VerticalStride;
    }

    PIXEL* dst = (PIXEL*)((ADDR)RendererState.Surfaces.Main
        + ModuleState.Surface.Offset + y * ModuleState.Surface.Stride + x * sizeof(PIXEL))
        + (ADDR)(ModuleState.Window.Y * ModuleState.Surface.Stride + ModuleState.Window.X * sizeof(PIXEL));

    if ((RendererState.Outline.Options & OUTLINEDRAWOPTION_TOP) == OUTLINEDRAWOPTION_NONE)
    {
        height = height - 1;

        PIXEL* pixels = dst;

        if (src <= dst)
        {
            pixels = (PIXEL*)((ADDR)dst - SCREEN_SIZE_IN_BYTES);
        }

        for (S32 xx = 0; xx < width; ++xx)
        {
            pixels[RendererState.Outline.HorizontalStride * xx] = pixel;
        }

        dst = (PIXEL*)((ADDR)dst + (ADDR)RendererState.Outline.Stride);
    }

    if ((RendererState.Outline.Options & OUTLINEDRAWOPTION_RIGHT) == OUTLINEDRAWOPTION_NONE)
    {
        S32 off = (width - 1) * sizeof(PIXEL);
        if (RendererState.Outline.HorizontalStride < 0)
        {
            off = -off;
        }

        PIXEL* pixels = (PIXEL*)((ADDR)dst + (ADDR)off);

        for (S32 yy = 0; yy < height - 1; ++yy)
        {
            if (src <= pixels)
            {
                pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);
            }

            pixels[0] = pixel;

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)RendererState.Outline.Stride);
        }
    }

    if ((RendererState.Outline.Options & OUTLINEDRAWOPTION_LEFT) == OUTLINEDRAWOPTION_NONE)
    {
        for (S32 yy = 0; yy < height - 1; ++yy)
        {
            if (src <= dst)
            {
                dst = (PIXEL*)((ADDR)dst - SCREEN_SIZE_IN_BYTES);
            }

            dst[0] = pixel;

            dst = (PIXEL*)((ADDR)dst + (ADDR)RendererState.Outline.Stride);
        }
    }
    else
    {
        dst = (PIXEL*)((ADDR)dst + (ADDR)(RendererState.Outline.Stride * (height - 1)));
    }

    if (height != 0 && (RendererState.Outline.Options & OUTLINEDRAWOPTION_BOTTOM) == OUTLINEDRAWOPTION_NONE)
    {
        if (src <= dst)
        {
            dst = (PIXEL*)((ADDR)dst - SCREEN_SIZE_IN_BYTES);
        }

        for (S32 xx = 0; xx < width; ++xx)
        {
            dst[RendererState.Outline.HorizontalStride * xx] = pixel;
        }
    }
}

// 0x100026e0
VOID DrawStencilSurfaceWindowRectangle()
{
    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Stencil
        + ModuleState.Surface.Offset + (ModuleState.Window.Y * MAX_RENDERER_WIDTH + ModuleState.Window.X) * sizeof(PIXEL));

    CONST S32 height = ModuleState.Window.Height - ModuleState.Window.Y + 1;
    CONST S32 width = ModuleState.Window.Width - ModuleState.Window.X + 1;
    CONST S32 stride = (MAX_RENDERER_WIDTH - width) * sizeof(PIXEL);

    PIXEL pixel = (PIXEL)(ModuleState.Window.Y * STENCIL_PIXEL_COLOR_VALUE);

    if (ModuleState.Window.Y < ModuleState.Surface.Y)
    {
        CONST S32 delta = ModuleState.Window.Y + height - ModuleState.Surface.Y;

        if ((ModuleState.Window.Y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }

            pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

            for (S32 yy = 0; yy < delta; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

        for (S32 yy = 0; yy < height; ++yy)
        {
            for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixel; }

            pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
        }
    }
}

// 0x10002780
VOID MaskStencilSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    PIXEL* pixels = (PIXEL*)((ADDR)RendererState.Surfaces.Stencil
        + ModuleState.Surface.Offset + (y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL));

    CONST S32 stride = (MAX_RENDERER_WIDTH - width) * sizeof(PIXEL);

    CONST PIXEL pixel = (PIXEL)STENCIL_PIXEL_MASK_VALUE;

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }

            pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

            for (S32 yy = 0; yy < delta; ++yy)
            {
                for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((ADDR)pixels - SCREEN_SIZE_IN_BYTES);

        for (S32 yy = 0; yy < height; ++yy)
        {
            for (S32 xx = 0; xx < width; ++xx) { pixels[xx] = pixels[xx] & pixel; }

            pixels = (PIXEL*)((ADDR)pixels + (ADDR)(width * sizeof(PIXEL) + stride));
        }
    }
}

// 0x10002810
VOID MoveStencilSurface(S32 x, S32 y, S32 width, S32 height, S32 offset)
{
    DOUBLEPIXEL* pixels = (DOUBLEPIXEL*)((ADDR)RendererState.Surfaces.Stencil
        + ModuleState.Surface.Offset + (MAX_RENDERER_WIDTH * y + x) * sizeof(PIXEL));
    CONST S32 stride = sizeof(PIXEL) * (MAX_RENDERER_WIDTH - width);

    CONST BOOL addOp = offset >= 0;
    CONST PIXEL pixel = (PIXEL)((addOp ? offset : -offset) << STENCIL_PIXEL_COLOR_SHIFT);

    DOUBLEPIXEL pix = ((DOUBLEPIXEL)(pixel) << GRAPHICS_BITS_PER_PIXEL_16) | (DOUBLEPIXEL)pixel;

    // Immitate "pixels[i] - pix"
    if (addOp == FALSE)
        pix = (DOUBLEPIXEL)(-(S32)pix);

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;
        if (delta <= 0)
        {
            // Entire region is before the surface.y
            for (S32 i = 0; i < height; ++i)
            {
                for (S32 j = 0; j < (width >> 1); ++j)
                    *pixels++ += pix;

                if (width & 1)
                    *(PIXEL*)pixels += (PIXEL)pix;

                pixels = (DOUBLEPIXEL*)((ADDR)pixels + stride);
            }
        }
        else
        {
            // First process rows before the surface.y
            for (S32 i = 0; i < height - delta; ++i)
            {
                for (S32 j = 0; j < (width >> 1); ++j)
                    *pixels++ += pix;

                if (width & 1)
                    *(PIXEL*)pixels += (PIXEL)pix;

                pixels = (DOUBLEPIXEL*)((ADDR)pixels + stride);
            }

            // Update remaining height and pixels pointer
            pixels = (DOUBLEPIXEL*)((ADDR)pixels - (ADDR)SCREEN_SIZE_IN_BYTES);

            for (S32 i = 0; i < delta; ++i)
            {
                for (S32 j = 0; j < (width >> 1); ++j)
                    *pixels++ += pix;

                if (width & 1)
                    *(PIXEL*)pixels += (PIXEL)pix;

                pixels = (DOUBLEPIXEL*)((ADDR)pixels + stride);
            }
        }
    }
    else
    {
        // Entire region is after the surface.y, so moving to the array beginning
        pixels = (DOUBLEPIXEL*)((ADDR)pixels - (ADDR)SCREEN_SIZE_IN_BYTES);

        for (S32 i = 0; i < height; ++i)
        {
            for (S32 j = 0; j < (width >> 1); ++j)
                *pixels++ += pix;

            if (width & 1)
                *(PIXEL*)pixels += (PIXEL)pix;

            pixels = (DOUBLEPIXEL*)((ADDR)pixels + stride);
        }
    }
}

// 0x100028f0
BOOL LockRendererSurface()
{
#if ACTIVATE_MODERN_GRAPHICS_MODE
    DDSURFACEDESC2 desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

    desc.dwSize = sizeof(DDSURFACEDESC2);
#else
    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);
#endif

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

                offset = desc.lPitch * rect.top
                    + rect.left * (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL));
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

    for (S32 yy = 0; yy < height; yy++)
    {
        for (S32 xx = 0; xx < width; xx++)
        {
            if (RendererState.IsTrueColor)
            {
                ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]);
            }
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

    for (S32 yy = 0; yy < height; yy++)
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
        + (ADDR)(ModuleState.Surface.Offset + (y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));
    LPVOID dst = (LPVOID)((ADDR)ModuleState.Surface.Renderer
        + (ADDR)(ModuleState.Pitch * y + x * (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL))));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if (delta <= 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++)
                {
                    if (RendererState.IsTrueColor)
                    {
                        ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]);
                    }
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
                    if (RendererState.IsTrueColor)
                    {
                        ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]);
                    }
                    else { ((PIXEL*)dst)[xx] = src[xx]; }
                }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
            }

            src = (PIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++)
                {
                    if (RendererState.IsTrueColor)
                    {
                        ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]);
                    }
                    else { ((PIXEL*)dst)[xx] = src[xx]; }
                }

                src = (PIXEL*)((ADDR)src + (ADDR)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (LPVOID)((ADDR)dst + (ADDR)ModuleState.Pitch);
            }
        }
    }
    else
    {
        src = (PIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++)
            {
                if (RendererState.IsTrueColor)
                {
                    ((DOUBLEPIXEL*)dst)[xx] = RGB565_TO_RGB888(src[xx]);
                }
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
VOID WriteMainSurfaceRendererSurfaceWithFogOfWar(S32 sx, S32 sy, S32 ex, S32 ey)
{
    BOOL locked = FALSE;

    if (ModuleState.Surface.Renderer == NULL)
    {
        if (!LockRendererSurface()) { return; }

        locked = TRUE;
    }

    CONST U32 blocksNumber = 8;

    CONST S32 srcBlockSize = 8 * sizeof(DOUBLEPIXEL); // 32 bytes
    CONST S32 dstBlockSize =
        RendererState.IsTrueColor ? (2 * 8 * sizeof(DOUBLEPIXEL)) : (8 * sizeof(DOUBLEPIXEL)); // 32 or 64 bytes

    S32 delta = (ey + 1) - sy;
    RendererState.FogBlockParams2.Unk04 = 0;
    RendererState.FogRenderParams.ActualRGBMask = ModuleState.InitialRGBMask;
    RendererState.FogRenderParams.DstRowStride = -8 * ModuleState.Pitch + dstBlockSize;
    RendererState.FogRenderParams.LineStep = -(S32)MAX_RENDERER_WIDTH * 16 + srcBlockSize;

    U8* fogSrc = &ModuleState.Fog[(sy >> 3) + 8].Unk[(sx >> 4) + 8];
    DOUBLEPIXEL* src = (DOUBLEPIXEL*)((ADDR)RendererState.Surfaces.Main + ModuleState.Surface.Offset + (MAX_RENDERER_WIDTH * sy + sx) * sizeof(PIXEL));
    DOUBLEPIXEL* dst = (DOUBLEPIXEL*)((ADDR)ModuleState.Surface.Renderer
        + sx * (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL)) + sy * ModuleState.Pitch);

    if (sy >= ModuleState.Surface.Y)
        src = (DOUBLEPIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);

    if (sy >= ModuleState.Surface.Y || sy + delta <= ModuleState.Surface.Y)
    {
        RendererState.FogBlockParams.ValidRowsBlockCount = delta >> 3;
        RendererState.FogBlockParams.TempBlocksCount = 0;
        RendererState.FogBlockParams2.ExcessRowsBlockCount = 0;

        RendererState.FogRenderParams.BlocksCount = blocksNumber;
    }
    else
    {
        RendererState.FogBlockParams.ValidRowsBlockCount = (ModuleState.Surface.Y - sy) >> 3;
        RendererState.FogBlockParams2.ExcessRowsBlockCount = (delta + sy - ModuleState.Surface.Y) >> 3;
        U32 v7 = ((U8)ModuleState.Surface.Y - (U8)sy) & 7;
        v7 = v7 | ((8 - v7) << 8);      // v7 now is 0000 ' 0000 ' 8 - v7 ' v7, so the summ of all its bytes is always 8    
        RendererState.FogBlockParams.TempBlocksCount = v7;

        if (RendererState.FogBlockParams.ValidRowsBlockCount == 0)
        {
            RendererState.FogRenderParams.LineStep += SCREEN_SIZE_IN_BYTES;
            RendererState.FogRenderParams.BlocksCount = v7;
            RendererState.FogBlockParams.ValidRowsBlockCount = 1;
            RendererState.FogBlockParams.TempBlocksCount = 0;
        }
        else
            RendererState.FogRenderParams.BlocksCount = blocksNumber;
    }

    S32 remainingExcessRows;
    DOUBLEPIXEL* srcTemp;
    DOUBLEPIXEL* dstTemp;
    do
    {
        while (TRUE)
        {
            do {
                srcTemp = src;
                dstTemp = dst;

                U8* someFog = fogSrc;
                fogSrc = someFog + sizeof(FOGSPRITE);

                RendererState.FogRenderParams.Pointer = someFog;
                RendererState.FogBlockParams.Unk04 = ((ex + 1) - sx) >> 4;

                do {
                    DOUBLEPIXEL fogPixel = *(DOUBLEPIXEL*)((ADDR)RendererState.FogRenderParams.Pointer - 2);
                    fogPixel = (fogPixel & 0xFFFF'0000) | (*(PIXEL*)((ADDR)RendererState.FogRenderParams.Pointer + sizeof(FOGSPRITE)));

                    if (fogPixel)
                    {
                        if (fogPixel == 0x80808080)
                        {
                            U32 j = RendererState.FogRenderParams.BlocksCount;
                            ++RendererState.FogRenderParams.Pointer;
                            while (TRUE)
                            {
                                do
                                {
                                    if (RendererState.IsTrueColor)
                                    {
                                        dst[0] = RGB565_TO_RGB888((src[0] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[1] = RGB565_TO_RGB888((src[0] >> 16));

                                        dst[2] = RGB565_TO_RGB888((src[1] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[3] = RGB565_TO_RGB888((src[1] >> 16));

                                        dst[4] = RGB565_TO_RGB888((src[2] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[5] = RGB565_TO_RGB888((src[2] >> 16));

                                        dst[6] = RGB565_TO_RGB888((src[3] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[7] = RGB565_TO_RGB888((src[3] >> 16));

                                        dst[8] = RGB565_TO_RGB888((src[4] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[9] = RGB565_TO_RGB888((src[4] >> 16));

                                        dst[10] = RGB565_TO_RGB888((src[5] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[11] = RGB565_TO_RGB888((src[5] >> 16));

                                        dst[12] = RGB565_TO_RGB888((src[6] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[13] = RGB565_TO_RGB888((src[6] >> 16));

                                        dst[14] = RGB565_TO_RGB888((src[7] & DEFAULT_SCREEN_COLOR_MASK));
                                        dst[15] = RGB565_TO_RGB888((src[7] >> 16));
                                    }
                                    else
                                    {
                                        dst[0] = src[0];
                                        dst[1] = src[1];
                                        dst[2] = src[2];
                                        dst[3] = src[3];
                                        dst[4] = src[4];
                                        dst[5] = src[5];
                                        dst[6] = src[6];
                                        dst[7] = src[7];
                                    }

                                    src = (DOUBLEPIXEL*)((ADDR)src + (ADDR)MAX_RENDERER_WIDTH * sizeof(PIXEL));
                                    dst = (DOUBLEPIXEL*)((ADDR)dst + (ADDR)ModuleState.Pitch);

                                    --j;
                                } while (j & 0xFF);
                                j >>= 8;
                                if (j == 0)
                                    break;
                                src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
                            }
                        }
                        else
                        {
                            // Break fogPixel into bytes
                            U8 fogPixelLowByte = fogPixel & 0xFF;          // Byte 0
                            U8 fogValueMidByte = (fogPixel >> 8) & 0xFF;   // Byte 1
                            U8 fogPixelHighByte = (fogPixel >> 16) & 0xFF; // Byte 2
                            U8 fogPixelTopByte = (fogPixel >> 24) & 0xFF;  // Byte 3

                            // Step 1: fogValueMidByte -= fogPixelTopByte, track borrow
                            BOOL borrow1 = fogValueMidByte < fogPixelTopByte;
                            fogValueMidByte -= fogPixelTopByte;

                            // Step 2: fogValueMidByte -= fogPixelLowByte, track another borrow
                            BOOL borrow2 = fogValueMidByte < fogPixelLowByte;
                            fogValueMidByte -= fogPixelLowByte;

                            // Step 3: Compute carry of (fogPixelHighByte + fogValueMidByte)
                            BOOL carry = ((U16)fogPixelHighByte + (U16)fogValueMidByte) > 0xFF;
                            fogValueMidByte += fogPixelHighByte;

                            // Final v27 computation
                            S32 v27 = carry + (-(S32)borrow1) - (S32)borrow2;
                            v27 = (v27 & 0xFFFF'FF00) | fogValueMidByte;
                            RendererState.FogBlockParams.Unk01 = v27 >> 2;

                            // Compute v28 (difference between fogPixelTopByte and fogPixelHighByte)
                            S32 v28 = -(S32)(fogPixelTopByte < fogPixelHighByte);
                            fogPixelTopByte -= fogPixelHighByte;
                            v28 = (v28 & 0xFFFF'FF00) | fogPixelTopByte;
                            RendererState.FogBlockParams2.Unk01 = 2 * v28;

                            // Compute v30 (difference between fogPixelLowByte and fogPixelLow)
                            S32 v30 = -(S32)(fogPixelLowByte < fogPixelHighByte);
                            fogPixelLowByte -= fogPixelHighByte;
                            v30 = (v30 & 0xFFFF'FF00) | fogPixelLowByte;

                            S32 v31 = 4 * v30;
                            RendererState.FogBlockParams2.Unk02 = v31;

                            U32 fogOffset = ((S32)fogPixelHighByte + 0x7F) << 5;

                            CONST U32 mask = RendererState.FogRenderParams.ActualRGBMask;
                            U32 j = RendererState.FogRenderParams.BlocksCount;
                            ++RendererState.FogRenderParams.Pointer;

                            while (TRUE)
                            {
                                RendererState.FogBlockParams2.Unk04 = 0;
                                do {
                                    U8 k = 0x10;
                                    S32 v39 = fogOffset;
                                    do {
                                        while ((U8)(fogOffset >> 8) != 0x20)
                                        {
                                            CONST U32 srcPixel = (*src & 0xFFFF) | (*src << 16);
                                            CONST U32 v35 = RendererState.FogBlockParams2.Unk04 + (mask & srcPixel) * (U8)(fogOffset >> 8);
                                            fogOffset += RendererState.FogBlockParams2.Unk01;

                                            CONST U32 v37 = mask & (v35 >> 5);
                                            RendererState.FogBlockParams2.Unk04 = mask & v35;

                                            v31 = v37 >> 16;

                                            if (RendererState.IsTrueColor)
                                            {
                                                *(DOUBLEPIXEL*)dst = RGB565_TO_RGB888((PIXEL)(v31 | v37));
                                            }
                                            else { *(PIXEL*)dst = (PIXEL)(v31 | v37); }

                                            src = (DOUBLEPIXEL*)((ADDR)src + sizeof(PIXEL));
                                            dst = (DOUBLEPIXEL*)((ADDR)dst
                                                + (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL)));

                                            --k;
                                            if (k == 0)
                                                break;
                                        }
                                        if (k == 0)
                                            break;

                                        fogOffset += RendererState.FogBlockParams2.Unk01;

                                        if (RendererState.IsTrueColor)
                                        {
                                            *(DOUBLEPIXEL*)dst = RGB565_TO_RGB888((PIXEL)v31);
                                        }
                                        else { *(PIXEL*)dst = (PIXEL)v31; }

                                        src = (DOUBLEPIXEL*)((ADDR)src + sizeof(PIXEL));
                                        dst = (DOUBLEPIXEL*)((ADDR)dst
                                            + (RendererState.IsTrueColor ? sizeof(DOUBLEPIXEL) : sizeof(PIXEL)));

                                        --k;
                                    } while (k);

                                    fogOffset = RendererState.FogBlockParams2.Unk02 + v39;
                                    src = (DOUBLEPIXEL*)((ADDR)src + MAX_RENDERER_WIDTH * sizeof(PIXEL) - srcBlockSize);
                                    dst = (DOUBLEPIXEL*)((ADDR)dst + ModuleState.Pitch - dstBlockSize);

                                    RendererState.FogBlockParams2.Unk01 += RendererState.FogBlockParams.Unk01;

                                    --j;
                                } while (j & 0xFF);
                                j >>= 8;
                                if (j == 0)
                                    break;
                                src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
                            }
                        }
                    }
                    else
                    {
                        U32 j = RendererState.FogRenderParams.BlocksCount;
                        ++RendererState.FogRenderParams.Pointer;
                        CONST DOUBLEPIXEL mask = *(DOUBLEPIXEL*)&ModuleState.InvertedActualColorMask;
                        while (TRUE)
                        {
                            do
                            {
                                if (RendererState.IsTrueColor)
                                {
                                    dst[0] = RGB565_TO_RGB888(((mask & (src[0] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[1] = RGB565_TO_RGB888(((mask & (src[0] >> 1)) >> 16));

                                    dst[2] = RGB565_TO_RGB888(((mask & (src[1] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[3] = RGB565_TO_RGB888(((mask & (src[1] >> 1)) >> 16));

                                    dst[4] = RGB565_TO_RGB888(((mask & (src[2] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[5] = RGB565_TO_RGB888(((mask & (src[2] >> 1)) >> 16));

                                    dst[6] = RGB565_TO_RGB888(((mask & (src[3] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[7] = RGB565_TO_RGB888(((mask & (src[3] >> 1)) >> 16));

                                    dst[8] = RGB565_TO_RGB888(((mask & (src[4] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[9] = RGB565_TO_RGB888(((mask & (src[4] >> 1)) >> 16));

                                    dst[10] = RGB565_TO_RGB888(((mask & (src[5] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[11] = RGB565_TO_RGB888(((mask & (src[5] >> 1)) >> 16));

                                    dst[12] = RGB565_TO_RGB888(((mask & (src[6] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[13] = RGB565_TO_RGB888(((mask & (src[6] >> 1)) >> 16));

                                    dst[14] = RGB565_TO_RGB888(((mask & (src[7] >> 1)) & DEFAULT_SCREEN_COLOR_MASK));
                                    dst[15] = RGB565_TO_RGB888(((mask & (src[7] >> 1)) >> 16));
                                }
                                else
                                {
                                    dst[0] = mask & (src[0] >> 1);
                                    dst[1] = mask & (src[1] >> 1);
                                    dst[2] = mask & (src[2] >> 1);
                                    dst[3] = mask & (src[3] >> 1);
                                    dst[4] = mask & (src[4] >> 1);
                                    dst[5] = mask & (src[5] >> 1);
                                    dst[6] = mask & (src[6] >> 1);
                                    dst[7] = mask & (src[7] >> 1);
                                }

                                src = (DOUBLEPIXEL*)((ADDR)src + (ADDR)MAX_RENDERER_WIDTH * sizeof(PIXEL));
                                dst = (DOUBLEPIXEL*)((ADDR)dst + (ADDR)ModuleState.Pitch);

                                --j;
                            } while (j & 0xFF);
                            j >>= 8;
                            if (j == 0)
                                break;
                            src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
                        }
                    }

                    src = (DOUBLEPIXEL*)((ADDR)src + (ADDR)RendererState.FogRenderParams.LineStep);
                    dst = (DOUBLEPIXEL*)((ADDR)dst + (ADDR)RendererState.FogRenderParams.DstRowStride);
                    --RendererState.FogBlockParams.Unk04;
                } while (RendererState.FogBlockParams.Unk04);

                src = (DOUBLEPIXEL*)((ADDR)srcTemp + (ADDR)MAX_RENDERER_WIDTH * sizeof(PIXEL) * 8);
                dst = (DOUBLEPIXEL*)((ADDR)dstTemp + (ADDR)ModuleState.Pitch * 8);
                --RendererState.FogBlockParams.ValidRowsBlockCount;
            } while (RendererState.FogBlockParams.ValidRowsBlockCount);

            if ((RendererState.FogBlockParams.TempBlocksCount & 0xFF) == 0)
                break;

            RendererState.FogRenderParams.BlocksCount = RendererState.FogBlockParams.TempBlocksCount;
            RendererState.FogRenderParams.LineStep = SCREEN_SIZE_IN_BYTES - MAX_RENDERER_WIDTH * sizeof(PIXEL) * 8 + srcBlockSize;
            RendererState.FogBlockParams.ValidRowsBlockCount = 1;
            RendererState.FogBlockParams.TempBlocksCount = 0;
        }

        RendererState.FogRenderParams.LineStep = -(S32)MAX_RENDERER_WIDTH * 16 + srcBlockSize;
        remainingExcessRows = RendererState.FogBlockParams2.ExcessRowsBlockCount;
        RendererState.FogBlockParams.ValidRowsBlockCount = remainingExcessRows;
        RendererState.FogBlockParams2.ExcessRowsBlockCount = 0;
        RendererState.FogRenderParams.BlocksCount = blocksNumber;
        src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
    } while (remainingExcessRows);

    if (locked) { UnlockRendererSurface(); }
}

// 0x10002fb0
VOID BlendMainSurfaceWithFogOfWar(S32 sx, S32 sy, S32 ex, S32 ey)
{
    CONST S32 blockSize = 8 * sizeof(DOUBLEPIXEL);  // 32 bytes
    CONST U32 blocksNumber = 8;

    S32 delta = (ey + 1) - sy;
    RendererState.FogBlockParams2.Unk04 = 0;
    RendererState.FogRenderParams.ActualRGBMask = ModuleState.InitialRGBMask;
    RendererState.FogRenderParams.DstRowStride = -8 * ModuleState.Pitch + blockSize;
    RendererState.FogRenderParams.LineStep = -(S32)MAX_RENDERER_WIDTH * 16 + blockSize;

    U8* fogSrc = &ModuleState.Fog[(sy >> 3) + 8].Unk[(sx >> 4) + 8];
    DOUBLEPIXEL* src = (DOUBLEPIXEL*)((ADDR)RendererState.Surfaces.Main + ModuleState.Surface.Offset + (MAX_RENDERER_WIDTH * sy + sx) * sizeof(PIXEL));

    if (sy >= ModuleState.Surface.Y)
        src = (DOUBLEPIXEL*)((ADDR)src - SCREEN_SIZE_IN_BYTES);

    if (sy >= ModuleState.Surface.Y || sy + delta <= ModuleState.Surface.Y)
    {
        RendererState.FogBlockParams.ValidRowsBlockCount = delta >> 3;
        RendererState.FogBlockParams.TempBlocksCount = 0;
        RendererState.FogBlockParams2.ExcessRowsBlockCount = 0;

        RendererState.FogRenderParams.BlocksCount = blocksNumber;
    }
    else
    {
        RendererState.FogBlockParams.ValidRowsBlockCount = (ModuleState.Surface.Y - sy) >> 3;
        RendererState.FogBlockParams2.ExcessRowsBlockCount = (delta + sy - ModuleState.Surface.Y) >> 3;
        U32 v7 = ((U8)ModuleState.Surface.Y - (U8)sy) & 7;
        v7 = v7 | ((8 - v7) << 8);      // v7 now is 0000 ' 0000 ' 8 - v7 ' v7, so the summ of all its bytes is always 8    
        RendererState.FogBlockParams.TempBlocksCount = v7;

        if (RendererState.FogBlockParams.ValidRowsBlockCount == 0)
        {
            RendererState.FogRenderParams.LineStep += SCREEN_SIZE_IN_BYTES;
            RendererState.FogRenderParams.BlocksCount = v7;
            RendererState.FogBlockParams.ValidRowsBlockCount = 1;
            RendererState.FogBlockParams.TempBlocksCount = 0;
        }
        else
            RendererState.FogRenderParams.BlocksCount = blocksNumber;
    }

    S32 remainingExcessRows;
    DOUBLEPIXEL* srcTemp;
    do
    {
        while (TRUE)
        {
            do {
                srcTemp = src;

                U8* someFog = fogSrc;
                fogSrc = someFog + sizeof(FOGSPRITE);

                RendererState.FogRenderParams.Pointer = someFog;
                RendererState.FogBlockParams.Unk04 = ((ex + 1) - sx) >> 4;

                do {
                    DOUBLEPIXEL fogPixel = *(DOUBLEPIXEL*)((ADDR)RendererState.FogRenderParams.Pointer - 2);
                    fogPixel = (fogPixel & 0xFFFF'0000) | (*(PIXEL*)((ADDR)RendererState.FogRenderParams.Pointer + sizeof(FOGSPRITE)));

                    if (fogPixel)
                    {
                        if (fogPixel == 0x80808080)
                        {
                            ++RendererState.FogRenderParams.Pointer;
                            src = (DOUBLEPIXEL*)((ADDR)src + blockSize);
                        }
                        else
                        {
                            // Break fogPixel into bytes
                            U8 fogPixelLowByte = fogPixel & 0xFF;          // Byte 0
                            U8 fogValueMidByte = (fogPixel >> 8) & 0xFF;   // Byte 1
                            U8 fogPixelHighByte = (fogPixel >> 16) & 0xFF; // Byte 2
                            U8 fogPixelTopByte = (fogPixel >> 24) & 0xFF;  // Byte 3

                            // Step 1: fogValueMidByte -= fogPixelTopByte, track borrow
                            BOOL borrow1 = fogValueMidByte < fogPixelTopByte;
                            fogValueMidByte -= fogPixelTopByte;

                            // Step 2: fogValueMidByte -= fogPixelLowByte, track another borrow
                            BOOL borrow2 = fogValueMidByte < fogPixelLowByte;
                            fogValueMidByte -= fogPixelLowByte;

                            // Step 3: Compute carry of (fogPixelHighByte + fogValueMidByte)
                            BOOL carry = ((U16)fogPixelHighByte + (U16)fogValueMidByte) > 0xFF;
                            fogValueMidByte += fogPixelHighByte;

                            // Final v27 computation
                            S32 v27 = carry + (-(S32)borrow1) - (S32)borrow2;
                            v27 = (v27 & 0xFFFF'FF00) | fogValueMidByte;
                            RendererState.FogBlockParams.Unk01 = v27 >> 2;

                            // Compute v28 (difference between fogPixelTopByte and fogPixelHighByte)
                            S32 v28 = -(S32)(fogPixelTopByte < fogPixelHighByte);
                            fogPixelTopByte -= fogPixelHighByte;
                            v28 = (v28 & 0xFFFF'FF00) | fogPixelTopByte;
                            RendererState.FogBlockParams2.Unk01 = 2 * v28;

                            // Compute v30 (difference between fogPixelLowByte and fogPixelLow)
                            S32 v30 = -(S32)(fogPixelLowByte < fogPixelHighByte);
                            fogPixelLowByte -= fogPixelHighByte;
                            v30 = (v30 & 0xFFFF'FF00) | fogPixelLowByte;

                            CONST S32 v31 = 4 * v30;
                            RendererState.FogBlockParams2.Unk02 = v31;

                            U32 fogOffset = ((S32)fogPixelHighByte + 0x7F) << 5;

                            CONST U32 mask = RendererState.FogRenderParams.ActualRGBMask;
                            U32 j = RendererState.FogRenderParams.BlocksCount;
                            ++RendererState.FogRenderParams.Pointer;

                            while (TRUE)
                            {
                                RendererState.FogBlockParams2.Unk04 = 0;
                                do {
                                    U8 k = 0x10;
                                    S32 v39 = fogOffset;
                                    do {
                                        while ((U8)(fogOffset >> 8) != 0x20)
                                        {
                                            CONST U32 srcPixel = (*src & 0xFFFF) | (*src << 16);
                                            CONST U32 v35 = RendererState.FogBlockParams2.Unk04 + (mask & srcPixel) * (U8)(fogOffset >> 8);
                                            fogOffset += RendererState.FogBlockParams2.Unk01;

                                            CONST U32 v37 = mask & (v35 >> 5);
                                            RendererState.FogBlockParams2.Unk04 = mask & v35;

                                            *(PIXEL*)src = (PIXEL)((v37 >> 16) | v37);

                                            src = (DOUBLEPIXEL*)((ADDR)src + sizeof(PIXEL));
                                            --k;
                                            if (k == 0)
                                                break;
                                        }
                                        if (k == 0)
                                            break;

                                        fogOffset += RendererState.FogBlockParams2.Unk01;

                                        src = (DOUBLEPIXEL*)((ADDR)src + sizeof(PIXEL));
                                        --k;
                                    } while (k);

                                    fogOffset = RendererState.FogBlockParams2.Unk02 + v39;
                                    src = (DOUBLEPIXEL*)((ADDR)src + MAX_RENDERER_WIDTH * sizeof(PIXEL) - blockSize);
                                    RendererState.FogBlockParams2.Unk01 += RendererState.FogBlockParams.Unk01;

                                    --j;
                                } while (j & 0xFF);
                                j >>= 8;
                                if (j == 0)
                                    break;
                                src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
                            }
                            src = (DOUBLEPIXEL*)((ADDR)src + (ADDR)RendererState.FogRenderParams.LineStep);
                        }
                    }
                    else
                    {
                        U32 j = RendererState.FogRenderParams.BlocksCount;
                        ++RendererState.FogRenderParams.Pointer;
                        CONST DOUBLEPIXEL mask = (*(DOUBLEPIXEL*)&ModuleState.InvertedActualColorMask) & 0x7FFF7FFF;
                        while (TRUE)
                        {
                            do
                            {
                                src[0] = mask & (src[0] >> 1);
                                src[1] = mask & (src[1] >> 1);
                                src[2] = mask & (src[2] >> 1);
                                src[3] = mask & (src[3] >> 1);
                                src[4] = mask & (src[4] >> 1);
                                src[5] = mask & (src[5] >> 1);
                                src[6] = mask & (src[6] >> 1);
                                src[7] = mask & (src[7] >> 1);
                                src = (DOUBLEPIXEL*)((ADDR)src + (ADDR)MAX_RENDERER_WIDTH * sizeof(PIXEL));

                                --j;
                            } while (j & 0xFF);
                            j >>= 8;
                            if (j == 0)
                                break;
                            src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
                        }
                        src = (DOUBLEPIXEL*)((ADDR)src + (ADDR)RendererState.FogRenderParams.LineStep);
                    }

                    --RendererState.FogBlockParams.Unk04;
                } while (RendererState.FogBlockParams.Unk04);

                src = (DOUBLEPIXEL*)((ADDR)srcTemp + (ADDR)MAX_RENDERER_WIDTH * sizeof(PIXEL) * 8);
                --RendererState.FogBlockParams.ValidRowsBlockCount;
            } while (RendererState.FogBlockParams.ValidRowsBlockCount);

            if ((RendererState.FogBlockParams.TempBlocksCount & 0xFF) == 0)
                break;

            RendererState.FogRenderParams.BlocksCount = RendererState.FogBlockParams.TempBlocksCount;
            RendererState.FogRenderParams.LineStep = SCREEN_SIZE_IN_BYTES - MAX_RENDERER_WIDTH * sizeof(PIXEL) * 8 + blockSize;
            RendererState.FogBlockParams.ValidRowsBlockCount = 1;
            RendererState.FogBlockParams.TempBlocksCount = 0;
        }

        RendererState.FogRenderParams.LineStep = -(S32)MAX_RENDERER_WIDTH * 16 + blockSize;
        remainingExcessRows = RendererState.FogBlockParams2.ExcessRowsBlockCount;
        RendererState.FogBlockParams.ValidRowsBlockCount = remainingExcessRows;
        RendererState.FogBlockParams2.ExcessRowsBlockCount = 0;
        RendererState.FogRenderParams.BlocksCount = blocksNumber;
        src = (DOUBLEPIXEL*)((ADDR)src - (ADDR)SCREEN_SIZE_IN_BYTES);
    } while (remainingExcessRows);
}

// 0x10003320
S32 AcquireTextLength(LPCSTR text, BINASSETCOLLECTIONCONTENTPTR asset)
{
    S32 result = 0;

    for (U32 xx = 0; text[xx] != NULL; xx++)
    {
        CONST IMAGEPALETTESPRITEPTR image = (IMAGEPALETTESPRITEPTR)((ADDR)asset + (ADDR)asset->Items[xx]);

        result += DEFAULT_FONT_ASSET_SPACING + image->Width;
    }

    return result;
}

// 0x10003360
VOID DrawMainSurfaceText(S32 x, S32 y, LPCSTR text, BINASSETCOLLECTIONCONTENTPTR asset, PIXEL* palette)
{
    U32 offset = 0;

    for (U32 xx = 0; text[xx] != NULL; xx++)
    {
        CONST IMAGEPALETTESPRITEPTR image = (IMAGEPALETTESPRITEPTR)((ADDR)asset + (ADDR)asset->Items[text[xx]]);

        DrawMainSurfacePaletteSpriteCompact(x + offset, y, palette, image);

        offset = offset + DEFAULT_FONT_ASSET_SPACING + image->Width;
    }
}

// 0x100033c0
VOID DrawBackSurfaceText(S32 x, S32 y, LPCSTR text, BINASSETCOLLECTIONCONTENTPTR asset, PIXEL* palette)
{
    U32 offset = 0;

    for (U32 xx = 0; text[xx] != NULL; xx++)
    {
        CONST IMAGEPALETTESPRITEPTR image = (IMAGEPALETTESPRITEPTR)((ADDR)asset + (ADDR)asset->Items[text[xx]]);

        DrawBackSurfacePaletteShadeSprite(x + offset, y, y, palette, image);

        offset = offset + DEFAULT_FONT_ASSET_SPACING + image->Width;
    }
}

// 0x10003420
VOID DrawSurfaceRhomb(S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, S32 tx, S32 ty, S32 stride, IMAGEPALETTETILEPTR tile, PIXEL* output)
{
    // Tile height: 32
    // Tile width:  63

    RendererState.Tile.Stencil = (PIXEL*)((ADDR)output + ModuleState.Surface.Offset % (MAX_RENDERER_WIDTH * sizeof(PIXEL)) + SCREEN_SIZE_IN_BYTES);
    RendererState.Tile.Window.X = ModuleState.Window.X + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Y = ModuleState.Window.Y;
    RendererState.Tile.Window.Width = ModuleState.Window.Width + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Height = ModuleState.Window.Height;
    RendererState.Tile.DisplayedHalfs = 0;

    // Check visibility
    if (tx > RendererState.Tile.Window.Width + 1
        || tx < RendererState.Tile.Window.X - TILE_SIZE_WIDTH - 1
        || ty > RendererState.Tile.Window.Height + 1
        || ty < RendererState.Tile.Window.Y - TILE_SIZE_HEIGHT)
        return;

    S32 tileStartDrawLength;

    // Initialization
    CONST U8* srcInput = tile->Pixels;
    PIXEL* dst = (PIXEL*)((ADDR)output + ModuleState.Surface.Offset + stride * ty + tx * sizeof(PIXEL) - 2);
    PIXEL* dst2;
    S32 txDelta = tx + TILE_SIZE_HEIGHT;
    S32 diff = (angle_1 - angle_0) << 2;
    BOOL isUpperPart = (ty > (ModuleState.Window.Y - 16)) ? TRUE : FALSE; // 1 - the rest 0 - quartile

    if (!isUpperPart)
    {
        // Initialization of lower quartile renderer parameters
        RendererState.Tile.Diff = (angle_3 - angle_0) << 4;
        txDelta = (angle_0 << 8) + RendererState.Tile.Diff;
        dst2 = (PIXEL*)((ADDR)dst + (ADDR)(16 * stride - 29 * sizeof(PIXEL)));
        tx += 3; // Step between cut pyramids, say 32 + 3

        tileStartDrawLength = 61; // Starting length of the lower part of the tile
        srcInput += 528; // Skip upper tile part to render the lower part
        ty += 16; // -24 + 16 is likely the height outside the screen -8, probably removal of the upper part

        // Calculating height
        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - ty, 16);

        // Decrease the height of the tile if it is partially outside the boundaries of the screen
        CONST S32 overage = ModuleState.Window.Y - ty;
        if (overage >= 0)
        {
            ty = RendererState.Tile.Window.Y;
            RendererState.Tile.Height -= overage;

            for (S32 y = 0; y < overage; ++y)
            {
                srcInput += tileStartDrawLength;
                txDelta += RendererState.Tile.Diff;
                tileStartDrawLength -= 4; // For stepped rendering for each of the following pyramid -4 from it's apex
                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL))); // 4 is likely to be 2 pixels + and offset for stepped operations
                tx += 2;
            }
        }
    }
    else
    {
        tileStartDrawLength = 3;   // Starting length of upper half of the tile
        tx = txDelta;

        // Initialization of renderer parameters
        RendererState.Tile.Diff = (angle_0 - angle_2) << 4;
        txDelta = (angle_2 << 8) + RendererState.Tile.Diff + diff;

        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - ty, 16);

        S32 overage = ModuleState.Window.Y - ty;
        if (overage >= 0)
        {
            ty += overage;
            RendererState.Tile.Height -= overage;

            for (S32 y = 0; y < overage; ++y)
            {
                srcInput += tileStartDrawLength;
                txDelta += RendererState.Tile.Diff;
                tileStartDrawLength += 4;
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride - 2 * sizeof(PIXEL)));
                tx -= 2;
            }
        }

        // Drawing
        if (RendererState.Tile.Height > 0)
        {
            ty += RendererState.Tile.Height;
            S32 overflow = Mathematics::Max(ty - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            dst2 = dst;
            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            while (RendererState.Tile.Height > 0)
            {
                for (S32 yy = 0; yy < RendererState.Tile.Height; ++yy)
                {
                    RendererState.Tile.TempHeight = overflow;

                    S32 totalTxOffset = txDelta;

                    CONST S32 delta = (RendererState.Tile.Window.Width + 1) - tx;
                    S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                    CONST S32 delta3 = RendererState.Tile.Window.X - tx;
                    if (delta > 0 && delta2 > delta3)
                    {
                        CONST U8* srcTemp = srcInput;
                        PIXEL* dstTemp = dst2;

                        if (delta3 > 0)
                        {
                            srcTemp = (U8*)((ADDR)srcTemp + (ADDR)delta3);
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)delta3 * sizeof(PIXEL));

                            delta2 -= delta3;
                            totalTxOffset = txDelta + delta3 * diff;
                        }

                        // Glitch
                        if (RendererState.Tile.Stencil <= dstTemp)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp - (ADDR)SCREEN_SIZE_IN_BYTES);
                        }
                        if (dstTemp < output)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)SCREEN_SIZE_IN_BYTES);
                        }

                        U16 uVar5 = ((U16)((totalTxOffset >> 8) ^ RendererState.Tile.Unk08) << 8) | (totalTxOffset & 0xFF);
                        RendererState.Tile.Unk08 ^= 0x20;

                        for (S32 y = 0; y < delta2; ++y)
                        {
                            dstTemp[y] = ModuleState.Rhombs[(uVar5 & 0xFF00) | srcTemp[y]];
                            uVar5 = uVar5 + (U16)(diff) ^ 0x2000;
                        }
                    }

                    srcInput = (U8*)((ADDR)srcInput + tileStartDrawLength);
                    tileStartDrawLength += 4;

                    txDelta += RendererState.Tile.Diff;
                    overflow = RendererState.Tile.TempHeight;
                    tx -= 2;

                    dst2 = dst = (PIXEL*)((ADDR)dst2 + (ADDR)(stride - 4));
                }

                // Drawing the rest of the height of the tile
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        if (ty > RendererState.Tile.Window.Height + 1)
            return;

        RendererState.Tile.Unk08 ^= 0x20;
        tileStartDrawLength -= 6;                                   // Moving lower part by 3 pixels
        dst2 = (PIXEL*)((ADDR)dst + (ADDR)(3 * sizeof(PIXEL)));     // Offset of the lower part
        tx += 3;

        RendererState.Tile.Height = Mathematics::Min((RendererState.Tile.Window.Height + 1) - ty, 16);
        RendererState.Tile.Diff = (angle_3 - angle_0) << 4;
        txDelta = (angle_0 << 8) + RendererState.Tile.Diff;
    }

    // Drawing of the lower part
    if (RendererState.Tile.Height > 0)
    {
        S32 overflow = RendererState.Tile.TempHeight;

        if (RendererState.Tile.DisplayedHalfs < 2)
        {
            overflow = Mathematics::Max(RendererState.Tile.Height + ty - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;

                overflow = RendererState.Tile.TempHeight;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        while (RendererState.Tile.Height > 0)
        {
            for (U16 yy = 0; yy < RendererState.Tile.Height; ++yy)
            {
                RendererState.Tile.TempHeight = overflow;

                S32 totalTxOffset = txDelta;

                S32 delta = (RendererState.Tile.Window.Width + 1) - tx;
                S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                CONST S32 delta3 = RendererState.Tile.Window.X - tx;
                if (delta > 0 && delta2 > delta3)
                {
                    CONST U8* srcTemp = srcInput;
                    PIXEL* dstTemp = dst2;
                    if (delta3 > 0)
                    {
                        srcTemp = (U8*)((ADDR)srcTemp + delta3);
                        dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)(delta3 * sizeof(PIXEL)));

                        delta2 -= delta3;
                        totalTxOffset = txDelta + delta3 * diff;
                    }

                    U16 uVar5 = ((U16)((totalTxOffset >> 8) ^ RendererState.Tile.Unk08) << 8) | (totalTxOffset & 0xFF);
                    RendererState.Tile.Unk08 ^= 0x20;

                    for (S32 y = 0; y < delta2; y++)
                    {
                        dstTemp[y] = ModuleState.Rhombs[(uVar5 & 0xFF00) | srcTemp[y]];
                        uVar5 = uVar5 + (U16)(diff) ^ 0x2000;
                    }

                }

                srcInput = (U8*)((ADDR)srcInput + tileStartDrawLength);
                tileStartDrawLength -= 4;

                txDelta += RendererState.Tile.Diff;
                overflow = RendererState.Tile.TempHeight;
                tx += 2;

                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL)));
            }

            RendererState.Tile.Height = RendererState.Tile.TempHeight;
            RendererState.Tile.TempHeight = 0;

            overflow = RendererState.Tile.TempHeight;

            dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x1000381e
VOID ShadeSurfaceRhomb(S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, S32 tx, S32 ty, S32 stride, PIXEL* output)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Tile.Stencil = (PIXEL*)((ADDR)output + ModuleState.Surface.Offset % (MAX_RENDERER_WIDTH * sizeof(PIXEL)) + SCREEN_SIZE_IN_BYTES);
    RendererState.Tile.Window.X = ModuleState.Window.X + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Y = ModuleState.Window.Y;
    RendererState.Tile.Window.Width = ModuleState.Window.Width + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Height = ModuleState.Window.Height;
    RendererState.Tile.DisplayedHalfs = 0;

    if (tx > RendererState.Tile.Window.Width + 1
        || tx < RendererState.Tile.Window.X - TILE_SIZE_WIDTH - 1
        || ty > RendererState.Tile.Window.Height + 1
        || ty < RendererState.Tile.Window.Y - TILE_SIZE_HEIGHT)
        return;

    S32 tileStartDrawLength;

    PIXEL* dst = (PIXEL*)((ADDR)output + ModuleState.Surface.Offset + stride * ty + tx * sizeof(PIXEL) - 2);
    PIXEL* dst2;
    S32 txDelta = tx + TILE_SIZE_HEIGHT;
    S32 diff = (angle_1 - angle_0) << 2;

    if (ty <= ModuleState.Window.Y - 16)
    {
        RendererState.Tile.Diff = (angle_3 - angle_0) << 4;
        txDelta = (angle_0 << 8) + RendererState.Tile.Diff;
        dst2 = (PIXEL*)((ADDR)dst + (ADDR)(16 * stride - 29 * sizeof(PIXEL)));
        tx += 3;

        tileStartDrawLength = 61;
        ty += 16;

        // Calculating height
        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - ty, 16);

        // Decrease the height of the tile if it is partially outside the boundaries of the screen
        CONST S32 overage = ModuleState.Window.Y - ty;
        if (overage >= 0)
        {
            ty = RendererState.Tile.Window.Y;
            RendererState.Tile.Height -= overage;

            for (S32 y = 0; y < overage; ++y)
            {
                txDelta += RendererState.Tile.Diff;
                tileStartDrawLength -= 4; // For stepped rendering for each of the following pyramid -4 from it's apex
                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL))); // 4 is likely to be 2 pixels + and offset for stepped operations
                tx += 2;
            }
        }
    }
    else
    {
        tileStartDrawLength = 3;   // Starting length of upper half of the tile
        tx = txDelta;

        // Initialization of renderer parameters
        RendererState.Tile.Diff = (angle_0 - angle_2) << 4;
        txDelta = (angle_2 << 8) + RendererState.Tile.Diff + diff;

        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - ty, 16);

        S32 overage = ModuleState.Window.Y - ty;
        if (overage >= 0)
        {
            ty += overage;
            RendererState.Tile.Height -= overage;

            for (S32 y = 0; y < overage; ++y)
            {
                txDelta += RendererState.Tile.Diff;
                tileStartDrawLength += 4;
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride - 2 * sizeof(PIXEL)));
                tx -= 2;
            }
        }

        // Drawing
        if (RendererState.Tile.Height > 0)
        {
            ty += RendererState.Tile.Height;
            S32 overflow = Mathematics::Max(ty - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            dst2 = dst;
            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            while (RendererState.Tile.Height > 0)
            {
                for (S32 yy = 0; yy < RendererState.Tile.Height; ++yy)
                {
                    RendererState.Tile.TempHeight = overflow;

                    S32 totalTxOffset = txDelta;

                    CONST S32 delta = (RendererState.Tile.Window.Width + 1) - tx;
                    S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                    CONST S32 delta3 = RendererState.Tile.Window.X - tx;
                    if (delta > 0 && delta2 > delta3)
                    {
                        PIXEL* dstTemp = dst2;

                        if (delta3 > 0)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)delta3 * sizeof(PIXEL));

                            delta2 -= delta3;
                            totalTxOffset = txDelta + delta3 * diff;
                        }

                        // Glitch
                        if (RendererState.Tile.Stencil <= dstTemp)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp - (ADDR)SCREEN_SIZE_IN_BYTES);
                        }
                        if (dstTemp < output)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)SCREEN_SIZE_IN_BYTES);
                        }

                        for (S32 j = 0; j < delta2; ++j)
                        {
                            CONST U8 byte1 = (U8)((ADDR)txDelta >> 8);
                            if (byte1 >= 0x20)
                                continue;

                            PIXEL res = (PIXEL)byte1;
                            if (byte1)
                            {
                                CONST DOUBLEPIXEL val = (dstTemp[j] << 16) | dstTemp[j];
                                CONST DOUBLEPIXEL mask = colorMask & (((colorMask & val) * byte1) >> 5);
                                res = (PIXEL)((mask >> 16) | mask);
                            }
                            dstTemp[j] = res;
                        }
                    }

                    tileStartDrawLength += 4;

                    txDelta += RendererState.Tile.Diff;
                    overflow = RendererState.Tile.TempHeight;
                    tx -= 2;

                    dst2 = dst = (PIXEL*)((ADDR)dst2 + (ADDR)(stride - 4));
                }

                // Drawing the rest of the height of the tile
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        if (ty > RendererState.Tile.Window.Height + 1)
            return;

        tileStartDrawLength -= 6;                                   // Moving lower part by 3 pixels
        dst2 = (PIXEL*)((ADDR)dst + (ADDR)(3 * sizeof(PIXEL)));     // Offset of the lower part
        tx += 3;

        RendererState.Tile.Height = Mathematics::Min((RendererState.Tile.Window.Height + 1) - ty, 16);
        RendererState.Tile.Diff = (angle_3 - angle_0) << 4;
        txDelta = (angle_0 << 8) + RendererState.Tile.Diff;
    }

    // Drawing of the lower part
    if (RendererState.Tile.Height > 0)
    {
        S32 overflow = RendererState.Tile.TempHeight;

        if (RendererState.Tile.DisplayedHalfs < 2)
        {
            overflow = Mathematics::Max(RendererState.Tile.Height + ty - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;

                overflow = RendererState.Tile.TempHeight;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        while (RendererState.Tile.Height > 0)
        {
            for (U16 yy = 0; yy < RendererState.Tile.Height; ++yy)
            {
                RendererState.Tile.TempHeight = overflow;

                S32 totalTxOffset = txDelta;

                S32 delta = (RendererState.Tile.Window.Width + 1) - tx;
                S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                CONST S32 delta3 = RendererState.Tile.Window.X - tx;
                if (delta > 0 && delta2 > delta3)
                {
                    PIXEL* dstTemp = dst2;
                    if (delta3 > 0)
                    {
                        dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)(delta3 * sizeof(PIXEL)));

                        delta2 -= delta3;
                        totalTxOffset = txDelta + delta3 * diff;
                    }

                    for (S32 j = 0; j < delta2; j++)
                    {
                        CONST U8 byte1 = (U8)((ADDR)txDelta >> 8);
                        if (byte1 >= 0x20)
                            continue;

                        PIXEL res = (PIXEL)byte1;
                        if (byte1)
                        {
                            CONST DOUBLEPIXEL val = (dstTemp[j] << 16) | dstTemp[j];
                            CONST DOUBLEPIXEL mask = colorMask & (((colorMask & val) * byte1) >> 5);
                            res = (PIXEL)((mask >> 16) | mask);
                        }
                        dstTemp[j] = res;
                    }

                }

                tileStartDrawLength -= 4;

                txDelta += RendererState.Tile.Diff;
                overflow = RendererState.Tile.TempHeight;
                tx += 2;

                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL)));
            }

            RendererState.Tile.Height = RendererState.Tile.TempHeight;
            RendererState.Tile.TempHeight = 0;

            overflow = RendererState.Tile.TempHeight;

            dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10003c48
VOID CleanSurfaceRhomb(S32 angle_0, S32 angle_1, S32 angle_2, S32 angle_3, S32 tx, S32 ty, S32 stride, IMAGEPALETTETILEPTR tile, PIXEL* output)
{
    RendererState.Tile.Stencil = (PIXEL*)((ADDR)output + ModuleState.Surface.Offset % (MAX_RENDERER_WIDTH * sizeof(PIXEL)) + SCREEN_SIZE_IN_BYTES);
    RendererState.Tile.Window.X = ModuleState.Window.X + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Y = ModuleState.Window.Y;
    RendererState.Tile.Window.Width = ModuleState.Window.Width + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Height = ModuleState.Window.Height;
    RendererState.Tile.DisplayedHalfs = 0;

    if (tx > RendererState.Tile.Window.Width + 1
        || tx < RendererState.Tile.Window.X - TILE_SIZE_WIDTH - 1
        || ty > RendererState.Tile.Window.Height + 1
        || ty < RendererState.Tile.Window.Y - TILE_SIZE_HEIGHT)
        return;

    S32 tileStartDrawLength;

    CONST U8* srcInput = tile->Pixels;
    PIXEL* dst = (PIXEL*)((ADDR)output + ModuleState.Surface.Offset + stride * ty + tx * sizeof(PIXEL) - 2);
    PIXEL* dst2;

    if (ty <= ModuleState.Window.Y - 16)
    {
        tileStartDrawLength = 61;
        tx += 3;

        dst2 = (PIXEL*)((ADDR)dst + (ADDR)(16 * stride - 29 * sizeof(PIXEL)));
        srcInput += 528;
        ty += 16;

        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - ty, 16);

        CONST S32 overage = ModuleState.Window.Y - ty;
        if (overage >= 0)
        {
            ty = RendererState.Tile.Window.Y;
            RendererState.Tile.Height -= overage;

            for (S32 y = 0; y < overage; ++y)
            {
                srcInput += tileStartDrawLength;
                tileStartDrawLength -= 4;
                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL)));
                tx += 2;
            }
        }
    }
    else
    {
        tileStartDrawLength = 3;
        tx += TILE_SIZE_HEIGHT;

        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - ty, 16);

        S32 overage = ModuleState.Window.Y - ty;
        if (overage >= 0)
        {
            ty += overage;
            RendererState.Tile.Height -= overage;

            for (S32 y = 0; y < overage; ++y)
            {
                srcInput += tileStartDrawLength;
                tileStartDrawLength += 4;
                dst = (PIXEL*)((ADDR)dst + (ADDR)(stride - 2 * sizeof(PIXEL)));
                tx -= 2;
            }
        }

        if (RendererState.Tile.Height > 0)
        {
            ty += RendererState.Tile.Height;
            S32 overflow = Mathematics::Max(ty - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            dst2 = dst;
            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            while (RendererState.Tile.Height > 0)
            {
                for (S32 yy = 0; yy < RendererState.Tile.Height; ++yy)
                {
                    RendererState.Tile.TempHeight = overflow;

                    CONST S32 delta = (RendererState.Tile.Window.Width + 1) - tx;
                    S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                    CONST S32 delta3 = RendererState.Tile.Window.X - tx;
                    if (delta > 0 && delta2 > delta3)
                    {
                        CONST U8* srcTemp = srcInput;
                        PIXEL* dstTemp = dst2;

                        if (delta3 > 0)
                        {
                            srcTemp = (U8*)((ADDR)srcTemp + (ADDR)delta3);
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)delta3 * sizeof(PIXEL));

                            delta2 -= delta3;
                        }

                        if (RendererState.Tile.Stencil <= dstTemp)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp - (ADDR)SCREEN_SIZE_IN_BYTES);
                        }
                        if (dstTemp < output)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)SCREEN_SIZE_IN_BYTES);
                        }

                        for (S32 y = 0; y < delta2; ++y)
                        {
                            if (srcTemp[y])
                                dstTemp[y] = 0;
                        }
                    }

                    srcInput = (U8*)((ADDR)srcInput + tileStartDrawLength);
                    tileStartDrawLength += 4;

                    overflow = RendererState.Tile.TempHeight;
                    tx -= 2;

                    dst2 = dst = (PIXEL*)((ADDR)dst2 + (ADDR)(stride - 4));
                }

                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        if (ty > RendererState.Tile.Window.Height + 1)
            return;

        tileStartDrawLength -= 6;
        dst2 = (PIXEL*)((ADDR)dst + (ADDR)(3 * sizeof(PIXEL)));
        tx += 3;

        RendererState.Tile.Height = Mathematics::Min((RendererState.Tile.Window.Height + 1) - ty, 16);
    }

    if (RendererState.Tile.Height > 0)
    {
        S32 overflow = RendererState.Tile.TempHeight;

        if (RendererState.Tile.DisplayedHalfs < 2)
        {
            overflow = Mathematics::Max(RendererState.Tile.Height + ty - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;

                overflow = RendererState.Tile.TempHeight;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        while (RendererState.Tile.Height > 0)
        {
            for (U16 yy = 0; yy < RendererState.Tile.Height; ++yy)
            {
                RendererState.Tile.TempHeight = overflow;

                S32 delta = (RendererState.Tile.Window.Width + 1) - tx;
                S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                CONST S32 delta3 = RendererState.Tile.Window.X - tx;
                if (delta > 0 && delta2 > delta3)
                {
                    CONST U8* srcTemp = srcInput;
                    PIXEL* dstTemp = dst2;
                    if (delta3 > 0)
                    {
                        srcTemp = (U8*)((ADDR)srcTemp + delta3);
                        dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)(delta3 * sizeof(PIXEL)));

                        delta2 -= delta3;
                    }

                    for (S32 y = 0; y < delta2; y++)
                    {
                        if (srcTemp[y])
                            dstTemp[y] = 0;
                    }

                }

                srcInput = (U8*)((ADDR)srcInput + tileStartDrawLength);
                tileStartDrawLength -= 4;

                overflow = RendererState.Tile.TempHeight;
                tx += 2;

                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL)));
            }

            RendererState.Tile.Height = RendererState.Tile.TempHeight;
            RendererState.Tile.TempHeight = 0;

            overflow = RendererState.Tile.TempHeight;

            dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10004016
VOID DrawSurfaceMaskRhomb(S32 x, S32 y, S32 stride, S32 mask, PIXEL* pixels)
{
    RendererState.Tile.Stencil = (PIXEL*)((ADDR)pixels + ModuleState.Surface.Offset % (MAX_RENDERER_WIDTH * sizeof(PIXEL)) + SCREEN_SIZE_IN_BYTES);
    RendererState.Tile.Window.X = ModuleState.Window.X + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Y = ModuleState.Window.Y;
    RendererState.Tile.Window.Width = ModuleState.Window.Width + TILE_SIZE_HEIGHT + 1;
    RendererState.Tile.Window.Height = ModuleState.Window.Height;
    RendererState.Tile.DisplayedHalfs = 0;

    if (x > RendererState.Tile.Window.Width + 1
        || x < RendererState.Tile.Window.X - TILE_SIZE_WIDTH - 1
        || y > RendererState.Tile.Window.Height + 1
        || y < RendererState.Tile.Window.Y - TILE_SIZE_HEIGHT)
        return;

    S32 tileStartDrawLength;
    PIXEL* dst = (PIXEL*)((ADDR)pixels + x * sizeof(PIXEL) + stride * y + ModuleState.Surface.Offset - 2);
    PIXEL* dst2;

    if (RendererState.Tile.Window.Y - 16 > y)
    {
        dst2 = dst + 8 * stride - 29;
        x += TILE_SIZE_HEIGHT - 29;
        y += 16;
        tileStartDrawLength = 61;
        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - y, 16);

        S32 overage = ModuleState.Window.Y - y;
        if (overage > 0)
        {
            RendererState.Tile.Height -= overage;
            y = RendererState.Tile.Window.Y;
            do
            {
                tileStartDrawLength -= 4;
                dst2 = (PIXEL*)((ADDR)dst2 + stride + 4);
                x += 2;
            } while (--overage);
        }
    }
    else
    {
        x += TILE_SIZE_HEIGHT;
        tileStartDrawLength = 3;
        RendererState.Tile.Height = Mathematics::Min((ModuleState.Window.Height + 1) - y, 16);

        S32 overage = RendererState.Tile.Window.Y - y;
        if (overage > 0)
        {
            RendererState.Tile.Height -= overage;
            y = RendererState.Tile.Window.Y;
            do
            {
                tileStartDrawLength += 4;
                x -= 2;
                dst = (PIXEL*)((ADDR)dst + (stride - 4));
            } while (--overage);
        }


        if (RendererState.Tile.Height > 0)
        {
            y += RendererState.Tile.Height;
            S32 overflow = Mathematics::Max(y - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            dst2 = dst;
            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            while (RendererState.Tile.Height > 0)
            {
                for (S32 i = 0; i < RendererState.Tile.Height; ++i)
                {
                    RendererState.Tile.TempHeight = overflow;

                    CONST S32 delta = (RendererState.Tile.Window.Width + 1) - x;
                    S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                    CONST S32 delta3 = RendererState.Tile.Window.X - x;
                    if (delta > 0 && delta2 > delta3)
                    {
                        PIXEL* dstTemp = dst2;

                        if (delta3 > 0)
                        {
                            dstTemp += delta3;
                            delta2 -= delta3;
                        }

                        if (dstTemp >= RendererState.Tile.Stencil)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp - (ADDR)SCREEN_SIZE_IN_BYTES);
                        }
                        if (dstTemp < pixels)
                        {
                            dstTemp = (PIXEL*)((ADDR)dstTemp + (ADDR)SCREEN_SIZE_IN_BYTES);
                        }

                        for (S32 j = 0; j < delta2; ++j)
                        {
                            dstTemp[j] = (PIXEL)mask + ((ModuleState.ShadeColorMask & dstTemp[j]) >> 1);
                        }
                    }

                    tileStartDrawLength += 4;

                    overflow = RendererState.Tile.TempHeight;
                    x -= 2;

                    dst2 = dst = (PIXEL*)((ADDR)dst2 + (stride - 4));
                }

                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;
                RendererState.Tile.DisplayedHalfs++;

                overflow = 0;

                dst2 = (PIXEL*)((ADDR)dst - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        if (y > RendererState.Tile.Window.Height + 1)
            return;

        tileStartDrawLength -= 3 * sizeof(PIXEL);
        dst2 = dst + 3;
        x += 3;

        RendererState.Tile.Height = Mathematics::Min((RendererState.Tile.Window.Height + 1) - y, 16);
    }

    if (RendererState.Tile.Height > 0)
    {
        S32 overflow = RendererState.Tile.TempHeight;

        if (RendererState.Tile.DisplayedHalfs < 2)
        {
            overflow = Mathematics::Max(RendererState.Tile.Height + y - ModuleState.Surface.Y, 0);

            RendererState.Tile.TempHeight = RendererState.Tile.Height;
            RendererState.Tile.Height -= overflow;

            if (RendererState.Tile.Height <= 0)
            {
                RendererState.Tile.Height = RendererState.Tile.TempHeight;
                RendererState.Tile.TempHeight = 0;

                overflow = RendererState.Tile.TempHeight;

                dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
            }
        }

        while (RendererState.Tile.Height > 0)
        {
            for (U16 yy = 0; yy < RendererState.Tile.Height; ++yy)
            {
                RendererState.Tile.TempHeight = overflow;

                S32 delta = (RendererState.Tile.Window.Width + 1) - x;
                S32 delta2 = Mathematics::Min(delta, tileStartDrawLength);
                CONST S32 delta3 = RendererState.Tile.Window.X - x;
                if (delta > 0 && delta2 > delta3)
                {
                    PIXEL* dstTemp = dst2;
                    if (delta3 > 0)
                    {
                        dstTemp += delta3;
                        delta2 -= delta3;
                    }

                    for (S32 j = 0; j < delta2; ++j)
                    {
                        dstTemp[j] = (PIXEL)mask + ((ModuleState.ShadeColorMask & dstTemp[j]) >> 1);
                    }

                }

                tileStartDrawLength -= 4;

                overflow = RendererState.Tile.TempHeight;
                x += 2;

                dst2 = (PIXEL*)((ADDR)dst2 + (ADDR)(stride + 2 * sizeof(PIXEL)));
            }

            RendererState.Tile.Height = RendererState.Tile.TempHeight;
            RendererState.Tile.TempHeight = 0;

            overflow = RendererState.Tile.TempHeight;

            dst2 = (PIXEL*)((ADDR)dst2 - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10004390
VOID DrawBackSurfaceRhombsPaletteSpriteA(S32 x, S32 y, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = ModuleState.Rhombs[indx + 0x4100];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            PIXEL pixel = ModuleState.Rhombs[indx + 0x4100];
                            pixel = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = ModuleState.Rhombs[indx + 0x4100];

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x100046b6
VOID DrawBackSurfaceRhombsPaletteSpriteB(S32 x, S32 y, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = ModuleState.Rhombs[indx + 0x4200];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            PIXEL pixel = ModuleState.Rhombs[indx + 0x4200];
                            pixel = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = ModuleState.Rhombs[indx + 0x4200];

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x100049e6
VOID DrawBackSurfaceRhombsPaletteShadedSprite(S32 x, S32 y, U16 level, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    CONST ptrdiff_t offset = sx - surfaceOffset;
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + offset;

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        PIXEL pixel = ModuleState.Rhombs[indx + 0x4000];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST PIXEL sten = level | (stencil[i] & 3);
                            if (sten & 2)
                                pixel = (PIXEL)(ModuleState.ShadePixel + (((DOUBLEPIXEL)ModuleState.ShadeColorMask & pixel) >> 1));

                            stencil[i] = sten;
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            PIXEL pixel = ModuleState.Rhombs[indx + 0x4000];
                            pixel = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;

                            CONST PIXEL sten = level | (stencil[i] & 3);
                            if (sten & 2)
                                pixel = (PIXEL)(ModuleState.ShadePixel + (((DOUBLEPIXEL)ModuleState.ShadeColorMask & pixel) >> 1));

                            stencil[i - 1] = sten;
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            PIXEL pixel = ModuleState.Rhombs[indx + 0x4000];

                            CONST PIXEL sten = level | (stencil[i] & 3);
                            if (sten & 2)
                                pixel = (PIXEL)(ModuleState.ShadePixel + (((DOUBLEPIXEL)ModuleState.ShadeColorMask & pixel) >> 1));

                            stencil[i] = sten;
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10004db0
VOID DrawMainSurfacePaletteSpriteStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;
    CONST U32 stencilLevel = (level << 16) | level;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    CONST ptrdiff_t offset = sx - surfaceOffset;
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + offset;

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (stencilLevel <= *(DOUBLEPIXEL*)((ADDR)stencil + (i - 1) * sizeof(PIXEL)))
                                continue;

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (stencilLevel <= *(DOUBLEPIXEL*)((ADDR)stencil + (i - 1) * sizeof(PIXEL)))
                                continue;

                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = palette[indx];

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x100050df
VOID DrawMainSurfacePaletteSpriteCompact(S32 x, S32 y, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);

        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                PIXEL* sx = RendererState.Sprite.X;
                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];

                        if (indx != 0)
                        {
                            CONST PIXEL pixel = palette[indx];

                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];

                            sx[i] = palette[indx];
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x100053c3
VOID DrawMainSurfaceVanishingPaletteSprite(S32 x, S32 y, S32 vanishOffset, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.VanishOffset = vanishOffset;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    RendererState.Sprite.Height = sprite->Height;

    x += sprite->X;
    y += sprite->Y;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height - (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                PIXEL* sx = RendererState.Sprite.X;
                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST U8 indx = pixels->Pixels[0];

                        if (indx != 0)
                        {
                            CONST PIXEL pixel = palette[indx];

                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                U32 tempDoublePixel = ((U32)sx[i] << 16) | sx[i];
                                U32 tempDoublePixel2 = (RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & tempDoublePixel) * RendererState.Sprite.VanishOffset) >> 5))
                                    | ((RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & tempDoublePixel) * RendererState.Sprite.VanishOffset) >> 5)) >> 16);
                                U32 tempDoublePixel3 = ((U32)pixel << 16) | pixel;
                                U32 tempDoublePixel4 = RendererState.Sprite.ColorMask & (((31 - RendererState.Sprite.VanishOffset) * (RendererState.Sprite.ColorMask & tempDoublePixel3)) >> 5);
                                U32 tempDoublePixel5 = tempDoublePixel4 | (tempDoublePixel4 >> 16);

                                sx[i] = (U16)(tempDoublePixel2 + tempDoublePixel5);
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];

                            CONST PIXEL pixel = palette[indx];

                            U32 tempDoublePixel = ((U32)sx[i] << 16) | sx[i];
                            U32 tempDoublePixel2 = (RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & tempDoublePixel) * RendererState.Sprite.VanishOffset) >> 5))
                                | ((RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & tempDoublePixel) * RendererState.Sprite.VanishOffset) >> 5)) >> 16);
                            U32 tempDoublePixel3 = ((U32)pixel << 16) | pixel;
                            U32 tempDoublePixel4 = RendererState.Sprite.ColorMask & (((31 - RendererState.Sprite.VanishOffset) * (RendererState.Sprite.ColorMask & tempDoublePixel3)) >> 5);
                            U32 tempDoublePixel5 = tempDoublePixel4 | (tempDoublePixel4 >> 16);

                            sx[i] = (U16)(tempDoublePixel2 + tempDoublePixel5);
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x1000579c
VOID DrawBackSurfacePaletteSprite(S32 x, S32 y, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];

                        if (indx != 0)
                        {
                            CONST PIXEL pixel = palette[indx];

                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];

                            CONST PIXEL pixel = palette[indx];
                            sx[i] = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];

                            CONST PIXEL pixel = palette[indx];
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10005ac6
VOID DrawBackSurfacePaletteSpriteAndStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Back);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            sx[i] = pixel;
                            stencil[i] = level;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = palette[indx];

                            sx[i] = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;
                            stencil[i] = level;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = palette[indx];

                            sx[i] = pixel;
                            stencil[i] = level;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10005e31
VOID DrawBackSurfacePaletteShadeSprite(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x = x + sprite->X;
    y = y + sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height - (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Back);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> count and one pixel
                        CONST U8 indx = pixels->Pixels[0];

                        if (indx != 0)
                        {
                            PIXEL pixel = palette[indx];

                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                CONST DOUBLEPIXEL sten = stencil[i];
                                stencil[i] = level | (sten & 3);

                                if (sten & 2)
                                    pixel = (PIXEL)(ModuleState.ShadePixel + (((DOUBLEPIXEL)ModuleState.ShadeColorMask & pixel) >> 1));

                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];

                            CONST PIXEL sten = stencil[i];
                            stencil[i] = level | (sten & 3);

                            PIXEL pixel = palette[indx];
                            if (sten & 2)
                                pixel = (PIXEL)(ModuleState.ShadePixel + (((DOUBLEPIXEL)ModuleState.ShadeColorMask & pixel) >> 1));

                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x1000618d
VOID DrawMainSurfacePaletteBlendSprite(S32 x, S32 y, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = palette[indx];
                            sx[i] = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST PIXEL pixel = palette[indx];
                            sx[i] = pixel;
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x100064b6
VOID DrawMainSurfaceSprite(S32 x, S32 y, IMAGESPRITEPTR sprite)
{
    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x = x + sprite->X;
    y = y + sprite->Y;

    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height - (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGESPRITEPIXELPTR pixels = (IMAGESPRITEPIXELPTR)content;

                PIXEL* sx = RendererState.Sprite.X;
                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGESPRITEPIXEL) + (count - 1) * sizeof(PIXEL));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST PIXEL pixel = pixels->Pixels[0];

                        if (pixel != MAGENTA_PIXEL)
                        {
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST PIXEL pixel = pixels->Pixels[skip + i];

                            sx[i] = pixel;
                        }

                        pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGESPRITEPIXEL) + (count - 1) * sizeof(PIXEL));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x100067ad
VOID DrawMainSurfaceAnimationSprite(S32 x, S32 y, ANIMATIONPIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST U8 indx = pixels->Pixels[0];
                        CONST ANIMATIONPIXEL pixel = palette[indx];

                        CONST DOUBLEPIXEL pix = pixel >> 19;

                        if ((pix & 0xFF) != 0x1F)
                        {
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                CONST DOUBLEPIXEL res = ((DOUBLEPIXEL)sx[i] << 16) | sx[i];
                                CONST DOUBLEPIXEL value = RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & res) * pix) >> 5);

                                sx[i] = (PIXEL)pixel + (PIXEL)((value >> 16) | value);
                            }
                        }
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST ANIMATIONPIXEL pixel = palette[indx];

                            CONST DOUBLEPIXEL pix = pixel >> 19;

                            if ((pix & 0xFF) != 0x1F)
                            {
                                CONST DOUBLEPIXEL res = ((DOUBLEPIXEL)sx[i] << 16) | sx[i];
                                CONST DOUBLEPIXEL value = RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & res) * pix) >> 5);

                                sx[i] = (PIXEL)pixel + (PIXEL)((value >> 16) | value);
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + ((count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10006b21
VOID DrawMainSurfaceAnimationSpriteStencil(S32 x, S32 y, U16 level, ANIMATIONPIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;
    CONST DOUBLEPIXEL stencilLevel = (level << 16) | level;

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x = x + sprite->X;
    y = y + sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height = RendererState.Sprite.Height - (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                // There was a bug, that 'pixels' was bigger than 'next'. According to IDA, there is a separate loop "pixels < next"
                // Also, there was a bug that 'i < count - skip', but count has been reduced by skip
                // And also, there was a bug that sx[i] >= maxX
                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Main);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST U8 indx = pixels->Pixels[0];
                        CONST ANIMATIONPIXEL pixel = palette[indx];

                        CONST DOUBLEPIXEL pix = pixel >> 19;

                        if ((pix & 0xFF) != 0x1F)
                        {
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                if (stencilLevel <= *(DOUBLEPIXEL*)(stencil + i - 1))
                                    continue;

                                CONST DOUBLEPIXEL value =
                                    (pix * (((sx[i] << 16) | sx[i]) & RendererState.Sprite.ColorMask) >> 5) & RendererState.Sprite.ColorMask;

                                sx[i] = (PIXEL)((value >> 16) | value) + (PIXEL)pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[skip + i];
                            CONST ANIMATIONPIXEL pixel = palette[indx];

                            CONST DOUBLEPIXEL pix = pixel >> 19;

                            if ((pix & 0xFF) != 0x1F)
                            {
                                if (stencilLevel <= *(DOUBLEPIXEL*)(stencil + i - 1))
                                    continue;

                                CONST DOUBLEPIXEL value =
                                    (pix * (((sx[i] << 16) | sx[i]) & RendererState.Sprite.ColorMask) >> 5) & RendererState.Sprite.ColorMask;

                                sx[i] = (PIXEL)((value >> 16) | value) + (PIXEL)pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + (ADDR)((count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10006ef8
VOID DrawMainSurfacePaletteSpriteFrontStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;
    CONST DOUBLEPIXEL stencilLevel = (level << 16) | level;

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Main);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];

                                sx[i] = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];

                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10007292
VOID DrawMainSurfacePaletteSpriteBackStencil(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;
    CONST DOUBLEPIXEL stencilLevel = (level << 16) | level;

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;
    S32 chessPixel = y;    // Used to display units behind objects in a checkerboard pattern

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        chessPixel += ModuleState.Window.Y - y;
        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = (ADDR)(ModuleState.Surface.Stride * y);

        RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + ModuleState.Surface.Offset + linesStride + (ADDR)(x * sizeof(PIXEL)));
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + ModuleState.Surface.Offset + linesStride + (ADDR)(ModuleState.Window.X * sizeof(PIXEL)));
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Surfaces.Main
            + ModuleState.Surface.Offset + linesStride + (ADDR)((ModuleState.Window.Width + 1) * sizeof(PIXEL)));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                chessPixel ^= 1;
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Main);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if ((((ADDR)(sx + i) / 2 ^ chessPixel) & 1) != 0
                                || *(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                    {
                        // Mask 0x40 -> repeat and blend pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if ((((ADDR)(sx + i) / 2 ^ chessPixel) & 1) != 0
                                || *(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];

                                CONST PIXEL pixel = palette[indx];
                                sx[i] = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                    {
                        // Mask 0x00 -> draw pixels
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if ((((ADDR)(sx + i) / 2 ^ chessPixel) & 1) != 0
                                || *(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];
                                sx[i] = pixel;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10007662
VOID DrawMainSurfaceShadowSprite(S32 x, S32 y, DOUBLEPIXEL color, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                // This function uses 0x80 RLE mask to skip pixels, not to draw one pixel few times.
                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Main);

                    if ((pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK) == 0)
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST DOUBLEPIXEL sten = *(DOUBLEPIXEL*)(stencil + i);
                            if ((sten & 0x8007) == 0)
                            {
                                *(DOUBLEPIXEL*)(stencil + i) = color | sten;

                                CONST PIXEL pixel = (PIXEL)(ModuleState.ShadePixel + SHADEPIXEL(*(DOUBLEPIXEL*)(sx + i), *(DOUBLEPIXEL*)&ModuleState.ShadeColorMask));
                                sx[i] = pixel;
                            }
                        }
                    }

                    pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10007928
VOID DrawBackSurfaceShadowSprite(S32 x, S32 y, DOUBLEPIXEL color, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Back;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                // This function uses 0x80 RLE mask to skip pixels, not to draw one pixel few times.
                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Back);

                    if ((pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK) == 0)
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST DOUBLEPIXEL sten = *(DOUBLEPIXEL*)(stencil + i);
                            if ((sten & 0x8007) == 0)
                            {
                                *(DOUBLEPIXEL*)(stencil + i) = color | sten;

                                CONST PIXEL pixel = (PIXEL)(ModuleState.ShadePixel + SHADEPIXEL(*(DOUBLEPIXEL*)(sx + i), *(DOUBLEPIXEL*)&ModuleState.ShadeColorMask));
                                sx[i] = pixel;
                            }
                        }
                    }

                    pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10007be8
VOID DrawMainSurfaceAdjustedSprite(S32 x, S32 y, U16 level, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;
    CONST DOUBLEPIXEL stencilLevel = (level << 16) | level;

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        // Count 0 -> no pixels to process
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Main);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                DOUBLEPIXEL pixel = RendererState.Sprite.ColorMask & (sx[i] | ((DOUBLEPIXEL)sx[i] << 16));
                                pixel = RendererState.Sprite.AdjustedColorMask & ((pixel * (pixels->Pixels[0] & IMAGESPRITE_ITEM_SMALL_PIXEL_MASK)) >> 4);
                                pixel = RendererState.Sprite.ColorMask &
                                    (((RendererState.Sprite.ColorMask - pixel) >> 5) | pixel);

                                sx[i] = (PIXEL)((pixel >> 16) | pixel);
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                DOUBLEPIXEL pixel = RendererState.Sprite.ColorMask & (sx[i] | ((DOUBLEPIXEL)sx[i] << 16));
                                pixel = RendererState.Sprite.AdjustedColorMask & ((pixel * (pixels->Pixels[skip + i] & IMAGESPRITE_ITEM_SMALL_PIXEL_MASK)) >> 4);
                                pixel = RendererState.Sprite.ColorMask &
                                    (((RendererState.Sprite.ColorMask - pixel) >> 5) | pixel);

                                sx[i] = (PIXEL)((pixel >> 16) | pixel);
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10007fbc
VOID DrawMainSurfaceActualSprite(S32 x, S32 y, U16 level, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    level = (level + STENCIL_PIXEL_OFFSET) * STENCIL_PIXEL_COLOR_VALUE;
    CONST DOUBLEPIXEL stencilLevel = (level << 16) | level;

    RendererState.Sprite.Height = sprite->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    x += sprite->X;
    y += sprite->Y;

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < ModuleState.Window.Y)
    {
        RendererState.Sprite.Height -= (ModuleState.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < ModuleState.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = ModuleState.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (ModuleState.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = ModuleState.Surface.Stride * y;
        CONST PIXEL* surfaceOffset = RendererState.Surfaces.Main;

        RendererState.Sprite.X = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * ModuleState.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)((ADDR)surfaceOffset + ModuleState.Surface.Offset + linesStride + sizeof(PIXEL) * (ModuleState.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < ModuleState.Surface.Y
            ? 0 : y + RendererState.Sprite.Height - ModuleState.Surface.Y;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the pixels to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> only count
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> count and one pixel
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            // Mask 0x40 and 0x00 -> count and few pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);
                    PIXEL* CONST stencil = RendererState.Surfaces.Stencil + (sx - RendererState.Surfaces.Main);

                    if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                    {
                        // Mask 0xC0 -> skip pixels
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                    }
                    else if ((pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        // Mask 0x80 -> repeat one pixel
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                CONST PIXEL summ = pixel + sx[i];
                                CONST PIXEL colorPixel = ModuleState.ActualColorMask & ((summ ^ pixel ^ sx[i]) >> 1);
                                PIXEL out = summ - colorPixel;
                                if (colorPixel & ModuleState.ActualRedMask)
                                    out |= ModuleState.ActualRedMask;
                                if (colorPixel & ModuleState.ActualGreenMask)
                                    out |= ModuleState.ActualGreenMask;
                                if (colorPixel & ModuleState.ActualBlueMask)
                                    out |= ModuleState.ActualGreenMask;
                                sx[i] = out;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            if (*(DOUBLEPIXEL*)(stencil + i - 1) < stencilLevel)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];

                                CONST PIXEL summ = pixel + sx[i];
                                CONST PIXEL colorPixel = ModuleState.ActualColorMask & ((summ ^ pixel ^ sx[i]) >> 1);
                                PIXEL out = summ - colorPixel;
                                if (colorPixel & ModuleState.ActualRedMask)
                                    out |= ModuleState.ActualRedMask;
                                if (colorPixel & ModuleState.ActualGreenMask)
                                    out |= ModuleState.ActualGreenMask;
                                if (colorPixel & ModuleState.ActualBlueMask)
                                    out |= ModuleState.ActualGreenMask;
                                sx[i] = out;
                            }
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)ModuleState.Surface.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)ModuleState.Surface.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}

// 0x10008ecd
VOID DrawUISprite(S32 x, S32 y, IMAGEPALETTESPRITEPTR sprite, LPVOID pal, IMAGESPRITEUIPTR output)
{
    // Early exit just in case
    switch (sprite->Type)
    {
    case SPRITETYPE_STATIC:
    case SPRITETYPE_DYNAMIC:
    case SPRITETYPE_ANIMATION:
    case SPRITETYPE_ALPHA:
        break;
    default:
        return;
    }

    RendererState.UI.Offset = output->Offset;
    RendererState.UI.Stride = output->Stride * 2;
    RendererState.UI.Window.X = output->X;
    RendererState.UI.Window.Y = output->Y;
    RendererState.UI.Window.Width = output->Width;
    RendererState.UI.Window.Height = output->Height;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    y += sprite->Y;
    x += sprite->X;

    RendererState.Sprite.Height = sprite->Height;

    if (y < RendererState.UI.Window.Y)
    {
        RendererState.Sprite.Height -= (RendererState.UI.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < RendererState.UI.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = RendererState.UI.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (RendererState.UI.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        ADDR offset = RendererState.UI.Offset;
        CONST ADDR linesStride = RendererState.UI.Stride * y;

        RendererState.Sprite.X = (PIXEL*)(offset + linesStride + x * sizeof(PIXEL));
        RendererState.Sprite.MinX = (PIXEL*)(offset + linesStride + RendererState.UI.Window.X * sizeof(PIXEL));
        RendererState.Sprite.MaxX = (PIXEL*)(offset + linesStride + (RendererState.UI.Window.Width + 1) * sizeof(PIXEL));

        CONST S32 overage = y + RendererState.Sprite.Height < MAX_RENDERER_HEIGHT ? 0 : y + RendererState.Sprite.Height - MAX_RENDERER_HEIGHT;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        switch (sprite->Type)
        {
        case SPRITETYPE_STATIC:
        {
            PIXEL* palette = (PIXEL*)pal;

            while (RendererState.Sprite.Height > 0)
            {
                while (RendererState.Sprite.Height > 0)
                {
                    ptrdiff_t skip = 0;
                    // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                    IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                    PIXEL* sx = RendererState.Sprite.X;
                    while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                        CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                        if (count <= need)
                        {
                            if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                            {
                                // Mask 0x80 -> count and one pixel
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                            }
                            else
                            {
                                // Mask 0x40 and 0x00 -> count and few pixels
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                            }
                        }

                        skip = count == need ? 0 : Mathematics::Min(count, need);
                        sx += Mathematics::Min(count, need);
                    }

                    while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                        if (count == 0)
                        {
                            // Count 0 -> no pixels to process
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                            return;
                        }

                        CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> repeat one pixel
                            CONST U8 indx = pixels->Pixels[0];

                            if (indx != 0)
                            {
                                CONST PIXEL pixel = palette[indx];

                                for (ptrdiff_t i = 0; i < availCount; ++i)
                                {
                                    sx[i] = pixel;
                                }
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];

                                sx[i] = pixel;
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                        }

                        sx += availCount;

                        skip = 0;
                    }

                    content = (LPVOID)((ADDR)next + sizeof(U16));
                    next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                    --RendererState.Sprite.Height;

                    RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)RendererState.UI.Stride);
                }

                // Wrap around vertically, and draw the overage
                // in case the sprite has more content that can fit into the allowed drawing rectangle.
                RendererState.Sprite.Height = RendererState.Sprite.Overage;
                RendererState.Sprite.Overage = 0;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            break;
        }
        case SPRITETYPE_DYNAMIC:
        {
            PIXEL* palette = (PIXEL*)pal;

            while (RendererState.Sprite.Height > 0)
            {
                while (RendererState.Sprite.Height > 0)
                {
                    ptrdiff_t skip = 0;
                    // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                    IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                    PIXEL* sx = RendererState.Sprite.X;
                    while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                        CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                        if (count <= need)
                        {
                            if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                            {
                                // Mask 0x80 -> count and one pixel
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                            }
                            else
                            {
                                // Mask 0x40 and 0x00 -> count and few pixels
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                            }
                        }

                        skip = count == need ? 0 : Mathematics::Min(count, need);
                        sx += Mathematics::Min(count, need);
                    }

                    while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK);

                        if (count == 0)
                        {
                            // Count 0 -> no pixels to process
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                            continue;
                        }

                        CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                        if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                        {
                            // Mask 0xC0 -> skip pixels
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> repeat one pixel
                            CONST U8 indx = pixels->Pixels[0];
                            CONST PIXEL pixel = palette[indx];

                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                sx[i] = pixel;
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_SHORT_COMPACT_MASK) == IMAGESPRITE_ITEM_SHORT_COMPACT_MASK)
                        {
                            // Mask 0x40 -> repeat and blend pixels
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];

                                sx[i] = (sx[i] + pixel - (ModuleState.ActualColorBits & (sx[i] ^ pixel))) >> 1;
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                        }
                        else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == 0)
                        {
                            // Mask 0x00 -> draw pixels
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST PIXEL pixel = palette[indx];

                                sx[i] = pixel;
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                        }

                        sx += availCount;

                        skip = 0;
                    }

                    content = (LPVOID)((ADDR)next + sizeof(U16));
                    next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                    --RendererState.Sprite.Height;

                    RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)RendererState.UI.Stride);
                }

                // Wrap around vertically, and draw the overage
                // in case the sprite has more content that can fit into the allowed drawing rectangle.
                RendererState.Sprite.Height = RendererState.Sprite.Overage;
                RendererState.Sprite.Overage = 0;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            break;
        }
        case SPRITETYPE_ALPHA:
        {
            while (RendererState.Sprite.Height > 0)
            {
                while (RendererState.Sprite.Height > 0)
                {
                    ptrdiff_t skip = 0;
                    // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                    IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                    PIXEL* sx = RendererState.Sprite.X;
                    while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                        CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                        if (count <= need)
                        {
                            if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                            {
                                // Mask 0x80 -> count and one pixel
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                            }
                            else
                            {
                                // Mask 0x40 and 0x00 -> count and few pixels
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                            }
                        }

                        skip = count == need ? 0 : Mathematics::Min(count, need);
                        sx += Mathematics::Min(count, need);
                    }

                    while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                    {
                        ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                        if (count == 0)
                        {
                            // Count 0 -> no pixels to process
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                            continue;
                        }

                        CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                        if ((pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK) == 0)
                        {
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                if ((sx[i] & 0x8007) == 0)
                                {
                                    CONST PIXEL pixel = (PIXEL)(ModuleState.ShadePixel + SHADEPIXEL(*(DOUBLEPIXEL*)(sx + i), *(DOUBLEPIXEL*)&ModuleState.ShadeColorMask));

                                    sx[i] = pixel;
                                }
                            }
                        }
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        sx += availCount;

                        skip = 0;
                    }

                    content = (LPVOID)((ADDR)next + sizeof(U16));
                    next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                    --RendererState.Sprite.Height;

                    RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)RendererState.UI.Stride);
                }

                // Wrap around vertically, and draw the overage
                // in case the sprite has more content that can fit into the allowed drawing rectangle.
                RendererState.Sprite.Height = RendererState.Sprite.Overage;
                RendererState.Sprite.Overage = 0;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            break;
        }
        case SPRITETYPE_ANIMATION:
        {
            ANIMATIONPIXEL* palette = (ANIMATIONPIXEL*)pal;

            CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
            RendererState.Sprite.ColorMask = colorMask;
            RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

            while (RendererState.Sprite.Height > 0)
            {
                while (RendererState.Sprite.Height > 0)
                {
                    ptrdiff_t skip = 0;
                    // How many pixels we should skip if pixels->count was bigger than diff between minX and sx
                    IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                    // Skip the pixels to the left of the sprite drawing area
                    // in case the sprite starts to the left of allowed drawing rectangle.
                    PIXEL* sx = RendererState.Sprite.X;

                    while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                        CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_SHORT_COUNT_MASK;

                        if (count <= need)
                        {
                            if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_EXTENDED_MASK)
                            {
                                // Mask 0xC0 -> only count
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                            }
                            else if ((pixels->Count & IMAGESPRITE_ITEM_EXTENDED_MASK) == IMAGESPRITE_ITEM_COMPACT_MASK)
                            {
                                // Mask 0x80 -> count and one pixel
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                            }
                            else
                            {
                                // Mask 0x40 and 0x00 -> count and few pixels
                                pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                            }
                        }

                        skip = count == need ? 0 : Mathematics::Min(count, need);
                        sx += Mathematics::Min(count, need);
                    }

                    while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                    {
                        CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                        if (count == 0)
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                            continue;
                        }

                        CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            // Mask 0x80 -> repeat one pixel
                            CONST U8 indx = pixels->Pixels[0];
                            CONST ANIMATIONPIXEL pixel = palette[indx];
                            CONST DOUBLEPIXEL pix = pixel >> 19;

                            if ((pix & 0xFF) != 0x1F)
                            {
                                for (ptrdiff_t i = 0; i < availCount; ++i)
                                {
                                    CONST DOUBLEPIXEL value =
                                        (pix * (((sx[i] << 16) | sx[i]) & RendererState.Sprite.ColorMask) >> 5) & RendererState.Sprite.ColorMask;

                                    sx[i] = (PIXEL)((value >> 16) | value) + (PIXEL)pixel;
                                }
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            for (ptrdiff_t i = 0; i < availCount; ++i)
                            {
                                CONST U8 indx = pixels->Pixels[skip + i];
                                CONST ANIMATIONPIXEL pixel = palette[indx];
                                CONST DOUBLEPIXEL pix = pixel >> 19;

                                if ((pix & 0xFF) != 0x1F)
                                {
                                    CONST DOUBLEPIXEL value = (pix * (((sx[i] << 16) | sx[i]) & RendererState.Sprite.ColorMask) >> 5) & RendererState.Sprite.ColorMask;

                                    sx[i] = (PIXEL)((value >> 16) | value) + (PIXEL)pixel;
                                }
                            }

                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                        }

                        sx += availCount;

                        skip = 0;
                    }

                    content = (LPVOID)((ADDR)next + sizeof(U16));
                    next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                    --RendererState.Sprite.Height;

                    RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + (ADDR)RendererState.UI.Stride);
                    RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + (ADDR)RendererState.UI.Stride);
                }

                // Wrap around vertically, and draw the overage
                // in case the sprite has more content that can fit into the allowed drawing rectangle.
                RendererState.Sprite.Height = RendererState.Sprite.Overage;
                RendererState.Sprite.Overage = 0;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
            }

            break;
        }
        }
    }
}

// 0x10009eb3
VOID MarkUISprite(S32 x, S32 y, IMAGEPALETTESPRITEPTR sprite, U8 type, IMAGESPRITEUIPTR output, U8* offset)
{
    RendererState.UI.Stride = output->Stride;
    RendererState.UI.Window.X = output->X;
    RendererState.UI.Window.Y = output->Y;
    RendererState.UI.Window.Width = output->Width;
    RendererState.UI.Window.Height = output->Height;

    RendererState.Sprite.Height = sprite->Height;

    x += sprite->X;
    y += sprite->Y;

    LPVOID content = &sprite->Pixels[0];
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < RendererState.UI.Window.Y)
    {
        RendererState.Sprite.Height -= (RendererState.UI.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < RendererState.UI.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = RendererState.UI.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (RendererState.UI.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        CONST ADDR linesStride = (ADDR)(RendererState.UI.Stride * y);

        RendererState.Sprite.X = (PIXEL*)(offset + linesStride + x);
        RendererState.Sprite.MinX = (PIXEL*)(offset + linesStride + RendererState.UI.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)(offset + linesStride + RendererState.UI.Window.Width + 1);


        CONST S32 overage = y + RendererState.Sprite.Height < MAX_RENDERER_HEIGHT
            ? 0 : y + RendererState.Sprite.Height - MAX_RENDERER_HEIGHT;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_PIXELS);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_PIXELS);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many elements we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the elements to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                U8* sx = (U8*)RendererState.Sprite.X;

                while (sx < (U8*)RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = (U8*)RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(U8));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < (U8*)RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, (U8*)RendererState.Sprite.MaxX - sx);

                    for (ptrdiff_t i = 0; i < availCount; i++) { sx[i] = type; }

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + ((count - 1) * sizeof(U8) + sizeof(IMAGEPALETTESPRITEPIXEL)));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + RendererState.UI.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + RendererState.UI.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + RendererState.UI.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_PIXELS);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_PIXELS);
        }
    }
}

// 0x1000a4f3
VOID DrawVanishingUISprite(S32 x, S32 y, S32 vanishLevel, PIXEL* palette, IMAGEPALETTESPRITEPTR sprite, IMAGESPRITEUIPTR output)
{
    CONST U32 colorMask = ((U32)ModuleState.ActualGreenMask << 16) | ModuleState.ActualBlueMask | ModuleState.ActualRedMask;
    RendererState.Sprite.ColorMask = colorMask;
    RendererState.Sprite.AdjustedColorMask = colorMask | (colorMask << 1);

    RendererState.UI.Offset = output->Offset;
    RendererState.UI.Stride = output->Stride * sizeof(PIXEL);
    RendererState.UI.Window.X = output->X;
    RendererState.UI.Window.Y = output->Y;
    RendererState.UI.Window.Width = output->Width;
    RendererState.UI.Window.Height = output->Height;

    RendererState.Sprite.Height = sprite->Height;


    x += sprite->X;
    y += sprite->Y;

    LPVOID content = &sprite->Pixels;
    LPVOID next = (LPVOID)((ADDR)content + (ADDR)sprite->Next);

    // Skip the necessary number of rows from the top of the image
    // in case the sprite starts above the allowed drawing rectangle.
    if (y < RendererState.UI.Window.Y)
    {
        RendererState.Sprite.Height -= (RendererState.UI.Window.Y - y);
        if (RendererState.Sprite.Height <= 0)
        {
            return;
        }

        for (S32 i = 0; i < RendererState.UI.Window.Y - y; ++i)
        {
            content = (LPVOID)((ADDR)next + sizeof(U16));
            next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);
        }

        y = RendererState.UI.Window.Y;
    }

    CONST S32 overflow = y + RendererState.Sprite.Height - (RendererState.UI.Window.Height + 1);
    BOOL draw = overflow <= 0;

    if (!draw)
    {
        draw = !(RendererState.Sprite.Height <= overflow);
        RendererState.Sprite.Height -= overflow;
    }

    if (draw)
    {
        ADDR offset = RendererState.UI.Offset;
        CONST ADDR linesStride = (ADDR)(RendererState.UI.Stride * y);

        RendererState.Sprite.X = (PIXEL*)(offset + linesStride + sizeof(PIXEL) * x);
        RendererState.Sprite.MinX = (PIXEL*)(offset + linesStride + sizeof(PIXEL) * RendererState.UI.Window.X);
        RendererState.Sprite.MaxX = (PIXEL*)(offset + linesStride + sizeof(PIXEL) * (RendererState.UI.Window.Width + 1));


        CONST S32 overage = y + RendererState.Sprite.Height < MAX_RENDERER_HEIGHT
            ? 0 : y + RendererState.Sprite.Height - MAX_RENDERER_HEIGHT;

        RendererState.Sprite.Overage = RendererState.Sprite.Height;
        RendererState.Sprite.Height -= overage;

        if (RendererState.Sprite.Height <= 0)
        {
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
        else
            RendererState.Sprite.Overage = overage;

        while (RendererState.Sprite.Height > 0)
        {
            while (RendererState.Sprite.Height > 0)
            {
                ptrdiff_t skip = 0;
                // How many elements we should skip if pixels->count was bigger than diff between minX and sx
                IMAGEPALETTESPRITEPIXELPTR pixels = (IMAGEPALETTESPRITEPIXELPTR)content;

                // Skip the elements to the left of the sprite drawing area
                // in case the sprite starts to the left of allowed drawing rectangle.
                PIXEL* sx = RendererState.Sprite.X;

                while (sx < RendererState.Sprite.MinX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t need = RendererState.Sprite.MinX - sx;
                    CONST ptrdiff_t count = pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK;

                    if (count <= need)
                    {
                        if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                        }
                        else
                        {
                            pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                        }
                    }

                    skip = count == need ? 0 : Mathematics::Min(count, need);
                    sx += Mathematics::Min(count, need);
                }

                while (sx < RendererState.Sprite.MaxX && (ADDR)pixels < (ADDR)next)
                {
                    CONST ptrdiff_t count = (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

                    if (count == 0)
                    {
                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(U8));
                        continue;
                    }

                    CONST ptrdiff_t availCount = Mathematics::Min(count - skip, RendererState.Sprite.MaxX - sx);

                    if (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                    {
                        CONST U8 indx = pixels->Pixels[0];
                        CONST PIXEL pixel = palette[indx];

                        CONST DOUBLEPIXEL res2 = ((DOUBLEPIXEL)pixel << 16) | pixel;
                        CONST DOUBLEPIXEL mask2 = RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & res2) * (31 - vanishLevel)) >> 5);

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST DOUBLEPIXEL res = ((DOUBLEPIXEL)sx[i] << 16) | sx[i];
                            CONST DOUBLEPIXEL mask = RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & res) * vanishLevel) >> 5);

                            sx[i] = (PIXEL)((mask | (mask >> 16)) + (mask2 | (mask >> 16)));
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL));
                    }
                    else
                    {

                        for (ptrdiff_t i = 0; i < availCount; ++i)
                        {
                            CONST U8 indx = pixels->Pixels[0];
                            CONST PIXEL pixel = palette[indx];

                            CONST DOUBLEPIXEL res2 = ((DOUBLEPIXEL)pixel << 16) | pixel;
                            CONST DOUBLEPIXEL mask2 = RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & res2) * (31 - vanishLevel)) >> 5);

                            CONST DOUBLEPIXEL res = ((DOUBLEPIXEL)sx[i] << 16) | sx[i];
                            CONST DOUBLEPIXEL mask = RendererState.Sprite.ColorMask & (((RendererState.Sprite.ColorMask & res) * vanishLevel) >> 5);

                            sx[i] = (PIXEL)((mask | (mask >> 16)) + (mask2 | (mask >> 16)));
                        }

                        pixels = (IMAGEPALETTESPRITEPIXELPTR)((ADDR)pixels + sizeof(IMAGEPALETTESPRITEPIXEL) + (count - 1) * sizeof(pixels->Pixels));
                    }

                    sx += availCount;

                    skip = 0;
                }

                content = (LPVOID)((ADDR)next + sizeof(U16));
                next = (LPVOID)((ADDR)next + sizeof(U16) + ((U16*)next)[0]);

                --RendererState.Sprite.Height;

                RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X + RendererState.UI.Stride);
                RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX + RendererState.UI.Stride);
                RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX + RendererState.UI.Stride);
            }

            // Wrap around vertically, and draw the overage
            // in case the sprite has more content that can fit into the allowed drawing rectangle.
            RendererState.Sprite.Height = RendererState.Sprite.Overage;
            RendererState.Sprite.Overage = 0;

            RendererState.Sprite.X = (PIXEL*)((ADDR)RendererState.Sprite.X - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MinX = (PIXEL*)((ADDR)RendererState.Sprite.MinX - (ADDR)SCREEN_SIZE_IN_BYTES);
            RendererState.Sprite.MaxX = (PIXEL*)((ADDR)RendererState.Sprite.MaxX - (ADDR)SCREEN_SIZE_IN_BYTES);
        }
    }
}
