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

#include "Module.hxx"
#include "Renderer.hxx"

#define PIXEL_COLOR_BIT_MASK 0x8000
#define STENCIL_PIXEL_COLOR_SHIFT 5
#define STENCIL_PIXEL_COLOR_VALUE 32
#define STENCIL_PIXEL_MASK_VALUE 0xFFFB

// NOTE: The original game copies values as if they were 32-bit long, while they are 16-bit long.
// Because of this, it ignores the last odd pixel (if any), due to left shift operation (width >> 1) == (width / 2).
// The macro is used to reach the same buggy result as in the original game.
#define ACQUIREWIDTH(x) (2 * (x / 2))

RENDERERSTATECONTAINER RendererState;

VOID MoveStencilSurface(S32 x, S32 y, S32 width, S32 height, S32 offset);

// 0x10001000
VOID Initialize()
{
    ModuleState.Surface.Offset = 0;

    ModuleState.Surface.Y = MAX_RENDERER_HEIGHT;
    ModuleState.Surface.Width = MAX_RENDERER_WIDTH;
    ModuleState.Surface.Height = MAX_RENDERER_HEIGHT;

    ModuleState.Surface.Stride = MAX_RENDERER_WIDTH * DEFAULT_SCREEN_DEPTH_BYTES;

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
    if (ModuleState.DirectX.Surface != NULL)
    {
        ModuleState.DirectX.Surface->Release();
        ModuleState.DirectX.Surface = NULL;
    }
}

// 0x100010d0
VOID RestoreDisplayMode()
{
    ReleaseRendererSurface();

    if (ModuleState.DirectX.Instance != NULL)
    {
        if (ModuleState.IsFullScreen) { ModuleState.DirectX.Instance->RestoreDisplayMode(); }

        ModuleState.DirectX.Instance->Release();
        ModuleState.DirectX.Instance = NULL;
    }
}

// 0x10001110
VOID ReleaseDirectX()
{
    ReleaseRendererSurface();

    if (ModuleState.DirectX.Instance != NULL)
    {
        ModuleState.DirectX.Instance->Release();
        ModuleState.DirectX.Instance = NULL;
    }
}

// 0x10001130
// TODO: Implement 32-bit colors.
VOID SetPixelColorMasks(CONST U32 r, CONST U32 g, CONST U32 b)
{
    ModuleState.ActualRedMask = r;
    ModuleState.InitialRedMask = r;
    ModuleState.ActualGreenMask = g;
    ModuleState.InitialGreenMask = g;
    ModuleState.ActualBlueMask = b;
    ModuleState.InitialBlueMask = b;

    CONST U32 rm = r & DEFAULT_SCREEN_DEPTH_COLOR_MASK;
    CONST U32 gm = g & DEFAULT_SCREEN_DEPTH_COLOR_MASK;
    CONST U32 bm = b & DEFAULT_SCREEN_DEPTH_COLOR_MASK;

    // R
    {
        U32 x = 0;
        U32 mask = PIXEL_COLOR_BIT_MASK;

        for (; x < DEFAULT_SCREEN_DEPTH_BITS; x++)
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

        for (; x < DEFAULT_SCREEN_DEPTH_BITS; x++)
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

        for (; x < DEFAULT_SCREEN_DEPTH_BITS; x++)
        {
            if (mask & bm) { ModuleState.ActualColorMask = ModuleState.ActualColorMask | mask; break; }

            mask = mask >> 1;
        }

        ModuleState.BlueOffset = x;
    }

    // R
    {
        U32 mask = 1;

        for (U32 x = 0; x < DEFAULT_SCREEN_DEPTH_BITS; x++)
        {
            if (mask & rm) { ModuleState.ActualColorBits = ModuleState.ActualColorBits | mask; break; }

            mask = mask << 1;
        }
    }

    // G
    {
        U32 mask = 1;

        for (U32 x = 0; x < DEFAULT_SCREEN_DEPTH_BITS; x++)
        {
            if (mask & gm) { ModuleState.ActualColorBits = ModuleState.ActualColorBits | mask; break; }

            mask = mask << 1;
        }
    }

    // B
    {
        U32 mask = 1;

        for (U32 x = 0; x < DEFAULT_SCREEN_DEPTH_BITS; x++)
        {
            if (mask & bm) { ModuleState.ActualColorBits = ModuleState.ActualColorBits | mask; break; }

            mask = mask << 1;
        }
    }

    ModuleState.InitialColorMask = ModuleState.ActualColorMask;

    ModuleState.Unk23 = ~ModuleState.ActualColorMask;
    ModuleState.Unk24 = ~ModuleState.ActualColorMask;

    ModuleState.Unk18 = ModuleState.ActualColorBits;
    ModuleState.Unk21 = ~ModuleState.ActualColorBits;
    ModuleState.Unk22 = ~ModuleState.ActualColorBits;

    ModuleState.Unk29 = (5 << (11 - (ModuleState.BlueOffset & 0x1f))) + (2 << (11 - (ModuleState.GreenOffset & 0x1f)));
    ModuleState.Unk30 = ModuleState.Unk29;

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

    if (ModuleState.IsFullScreen)
    {
        if (FAILED(ModuleState.DirectX.Instance->SetDisplayMode(width, height, DEFAULT_SCREEN_DEPTH_BITS))) { return FALSE; }
    }

    SetWindowPos(ModuleState.HWND, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);

    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if (FAILED(ModuleState.DirectX.Instance->CreateSurface(&desc, &ModuleState.DirectX.Surface, NULL))) { return FALSE; }

    if (FAILED(ModuleState.DirectX.Surface->GetSurfaceDesc(&desc))) { return FALSE; }

    SetPixelColorMasks(desc.ddpfPixelFormat.dwRBitMask, desc.ddpfPixelFormat.dwGBitMask, desc.ddpfPixelFormat.dwBBitMask);

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
            PIXEL* pixels = (PIXEL*)((addr)RendererState.Surfaces.Main
                + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

            if (ModuleState.Surface.Y <= y)
            {
                pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            }

            for (U32 xx = 0; xx < max - x + 1; xx++) { pixels[xx] = pixel; }
        }
    }
}

// 0x100014c0
VOID DrawMainSurfaceVerticalColorLine(S32 x, S32 y, S32 length, PIXEL pixel)
{
    S32 max = length + y - 1;

    if (x < ModuleState.Window.X) { return; }
    if (ModuleState.Window.Width < x) { return; }

    if (y < ModuleState.Window.Y) { y = ModuleState.Window.Y; }
    if (ModuleState.Window.Height < max) { max = ModuleState.Window.Height; }

    max = max + 1 - y;

    if (max < 1) { return; }

    PIXEL* pixels = (PIXEL*)((addr)RendererState.Surfaces.Main
        + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + max - ModuleState.Surface.Y;

        if (delta < 1)
        {
            for (S32 xx = 0; xx < max; xx++)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 xx = 0; xx < max - delta; xx++)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 xx = 0; xx < delta; xx++)
            {
                pixels[0] = pixel;

                pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 xx = 0; xx < max; xx++)
        {
            pixels[0] = pixel;

            pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x10001580
VOID DrawMainSurfaceColorBox(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    DrawMainSurfaceHorizontalColorLine(x, y, width, pixel);
    DrawMainSurfaceHorizontalColorLine(x, y + height - 1, width, pixel);
    DrawMainSurfaceVerticalColorLine(x, y, height, pixel);
    DrawMainSurfaceVerticalColorLine(x + width - 1, y, height, pixel);
}

// 0x100015e0
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

    PIXEL* pixels = (PIXEL*)((addr)RendererState.Surfaces.Main
        + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

            pixels = (PIXEL*)((addr)pixels + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x100016d0
VOID DrawMainSurfaceRectangleColorOverlay(S32 x, S32 y, S32 width, S32 height, S32 param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100017f0
VOID FUN_100017f0(S32 x, S32 y, PIXEL pixel)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001850
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

// 0x100018b0
VOID ReadMainSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* surface)
{
    PIXEL* src = (PIXEL*)((addr)RendererState.Surfaces.Main
        + (addr)((ModuleState.Surface.Offset + (sy * MAX_RENDERER_WIDTH + sx)) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((addr)surface + (addr)((stride * dy + dx) * sizeof(PIXEL)));

    if (sy < ModuleState.Surface.Y)
    {
        CONST S32 delta = sy + height - ModuleState.Surface.Y;

        if ((sy + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)(stride * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)(stride * sizeof(PIXEL)));
            }

            src = (PIXEL*)((addr)src - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)(stride * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        src = (PIXEL*)((addr)src - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

            src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            dst = (PIXEL*)((addr)dst + (addr)(stride * sizeof(PIXEL)));
        }
    }
}

// 0x10001bf0
// TODO: Better name.
// TODO: Implement 32-bit colors.
VOID ConvertColorsExtra(PIXEL* input, PIXEL* output, S32 count)
{
    for (S32 x = 0; x < count; x++)
    {
        CONST PIXEL pixel = input[x];

        if (pixel == MAGENTA_PIXEL) { output[x] = MAGENTA_PIXEL; continue; }

        CONST PIXEL r = (((pixel & 0xF800) << 0) >> (ModuleState.RedOffset & 0x1f)) & ModuleState.ActualRedMask;
        CONST PIXEL g = (((pixel & 0x07E0) << 5) >> (ModuleState.GreenOffset & 0x1f)) & ModuleState.ActualGreenMask;
        CONST PIXEL b = (((pixel & 0x001F) << 11) >> (ModuleState.BlueOffset & 0x1f)) & ModuleState.ActualBlueMask;

        output[x] = (PIXEL)(r | g | b);
    }
}

// 0x10001c90
// TODO: Implement 32-bit colors.
VOID ConvertColors(PIXEL* input, PIXEL* output, S32 count)
{
    for (S32 x = 0; x < count; x++)
    {
        CONST PIXEL pixel = input[x];

        CONST PIXEL r = (((pixel & 0xF800) << 0) >> (ModuleState.RedOffset & 0x1f)) & ModuleState.ActualRedMask;
        CONST PIXEL g = (((pixel & 0x07E0) << 5) >> (ModuleState.GreenOffset & 0x1f)) & ModuleState.ActualGreenMask;
        CONST PIXEL b = (((pixel & 0x001F) << 11) >> (ModuleState.BlueOffset & 0x1f)) & ModuleState.ActualBlueMask;

        output[x] = (PIXEL)(r | g | b);
    }
}

// 0x10001d10
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

// 0x10001ea0
VOID FUN_10001ea0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001ee0
VOID FUN_10001ee0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f20
VOID FUN_10001f20(S32 param_1, S32 param_2, S32 param_3)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f50
VOID FUN_10001f50(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f90
VOID WriteBackSurfaceMainSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    PIXEL* src = (PIXEL*)((addr)RendererState.Surfaces.Back + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((addr)RendererState.Surfaces.Main + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }

            src = (PIXEL*)((addr)src - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
            dst = (PIXEL*)((addr)dst - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            }
        }
    }
    else
    {
        src = (PIXEL*)((addr)src - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));
        dst = (PIXEL*)((addr)dst - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

            src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            dst = (PIXEL*)((addr)dst + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
        }
    }
}

// 0x10002030
VOID FUN_10002030(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100023f0
VOID DrawMainSurfaceColorOutline(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    S32 offset = ModuleState.Surface.Offset & (0x80000000 | (MAX_RENDERER_WIDTH - 1));

    if (offset < 0) { offset = ((offset - 1) | (-MAX_RENDERER_WIDTH)) + 1; }

    PIXEL* src = (PIXEL*)((addr)RendererState.Surfaces.Main
        + (addr)((offset + MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT) * sizeof(PIXEL)));

    RendererState.Outline.Width = ModuleState.Window.Width - ModuleState.Window.X;
    RendererState.Outline.Height = ModuleState.Window.Height + 1 - ModuleState.Window.Y;

    x = x - ModuleState.Window.X;
    y = y - ModuleState.Window.Y;

    RendererState.Outline.Options = RENDERER_OUTLINE_OPTIONS_SKIP_NONE;

    if (y < 0)
    {
        height = height + y;

        if (height < 1) { RendererState.Outline.Options = RENDERER_OUTLINE_OPTIONS_SKIP_NONE; return; }

        y = 0;
        RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_TOP);
    }

    if (RendererState.Outline.Height <= y)
    {
        height = height + y;
        y = RendererState.Outline.Height - 1;
        height = height - y;

        if (-1 < height) { return; }

        RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_TOP);
    }

    {
        CONST S32 max = y + 1 + height;

        if (y + 1 + height <= 0 != max < 0)
        {
            height = height - max - 1;
            RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_BOTTOM);
        }
    }

    {
        CONST S32 max = y - 1 + height;

        if (RendererState.Outline.Height <= max)
        {
            height = height + RendererState.Outline.Height - max;
            RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_BOTTOM);
        }
    }

    if (x < 1)
    {
        width = width + x;

        if (width < 2) { return; }

        x = 1;
        width = width - 1;

        RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_LEFT);
    }

    if (RendererState.Outline.Width + 2 <= x)
    {
        width = width + x + 1 - RendererState.Outline.Width - 2;
        x = RendererState.Outline.Width + 1;

        if (-1 < width) { return; }

        RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_LEFT);
    }

    if (x + width <= 0 != x + width < 0)
    {
        width = width - x - width;

        RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_RIGHT);
    }

    {
        S32 max = x - 2 + width;

        if (RendererState.Outline.Width < max)
        {
            width = width + RendererState.Outline.Width - max;

            RendererState.Outline.Options = (RENDEREROUTLINESKIPOPTIONS)(RendererState.Outline.Options | RENDERER_OUTLINE_OPTIONS_SKIP_RIGHT);
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

    PIXEL* dst = (PIXEL*)((addr)RendererState.Surfaces.Main
        + (addr)(ModuleState.Surface.Offset * sizeof(PIXEL) + y * ModuleState.Surface.Stride + x * sizeof(PIXEL))
        + (addr)(ModuleState.Window.Y * ModuleState.Surface.Stride + ModuleState.Window.X * sizeof(PIXEL)));

    if ((RendererState.Outline.Options & RENDERER_OUTLINE_OPTIONS_SKIP_TOP) == RENDERER_OUTLINE_OPTIONS_SKIP_NONE)
    {
        height = height - 1;

        PIXEL* pixels = dst;

        if (src <= dst) { pixels = (PIXEL*)((addr)dst - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

        for (S32 xx = 0; xx < width; xx++)
        {
            pixels[RendererState.Outline.HorizontalDirection * xx] = pixel;
        }

        dst = (PIXEL*)((addr)dst + (addr)RendererState.Outline.Stride);
    }

    if ((RendererState.Outline.Options & RENDERER_OUTLINE_OPTIONS_SKIP_RIGHT) == RENDERER_OUTLINE_OPTIONS_SKIP_NONE)
    {
        S32 off = (width - 1) * sizeof(PIXEL);
        if (RendererState.Outline.HorizontalDirection < 0) { off = -off; }

        PIXEL* pixels = (PIXEL*)((addr)dst + (addr)off);

        for (S32 yy = 0; yy < height - 1; yy++)
        {
            if (src <= pixels) { pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

            pixels[0] = pixel;

            pixels = (PIXEL*)((addr)pixels + (addr)RendererState.Outline.Stride);
        }
    }

    if ((RendererState.Outline.Options & RENDERER_OUTLINE_OPTIONS_SKIP_LEFT) == RENDERER_OUTLINE_OPTIONS_SKIP_NONE)
    {
        for (S32 yy = 0; yy < height - 1; yy++)
        {
            if (src <= dst) { dst = (PIXEL*)((addr)dst - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

            dst[0] = pixel;

            dst = (PIXEL*)((addr)dst + (addr)RendererState.Outline.Stride);
        }
    }
    else
    {
        dst = (PIXEL*)((addr)dst + (addr)(RendererState.Outline.Stride * (height - 1)));
    }

    if (height != 0 && (RendererState.Outline.Options & RENDERER_OUTLINE_OPTIONS_SKIP_BOTTOM) == RENDERER_OUTLINE_OPTIONS_SKIP_NONE)
    {
        if (src <= dst) { dst = (PIXEL*)((addr)dst - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL))); }

        for (S32 xx = 0; xx < width; xx++)
        {
            dst[RendererState.Outline.HorizontalDirection * xx] = pixel;
        }
    }
}

// 0x100026f0
VOID DrawStencilSurfaceWindowRectangle()
{
    PIXEL* pixels = (PIXEL*)((addr)RendererState.Surfaces.Stencil +
        (addr)(ModuleState.Surface.Offset + ModuleState.Window.Y * MAX_RENDERER_WIDTH + ModuleState.Window.X) * (addr)sizeof(PIXEL));

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

                pixels = (PIXEL*)((addr)pixels + (addr)(width * sizeof(PIXEL) + stride));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((addr)pixels + (addr)(width * sizeof(PIXEL) + stride));
            }

            pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

                pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

                pixels = (PIXEL*)((addr)pixels + (addr)(width * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { pixels[xx] = pixel; }

            pixel = pixel + STENCIL_PIXEL_COLOR_VALUE;

            pixels = (PIXEL*)((addr)pixels + (addr)(width * sizeof(PIXEL) + stride));
        }
    }
}

// 0x10002790
VOID MaskStencilSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    PIXEL* pixels = (PIXEL*)((addr)RendererState.Surfaces.Stencil
        + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

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

                pixels = (PIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
            }

            pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

                pixels = (PIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        pixels = (PIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < length; xx++) { pixels[xx] = pixels[xx] & pixel; }

            pixels = (PIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
        }
    }
}

// 0x10002820
VOID MoveStencilSurface(S32 x, S32 y, S32 width, S32 height, S32 offset)
{
    CONST S32 stride = (MAX_RENDERER_WIDTH - width) * sizeof(PIXEL);

    DOUBLEPIXEL* pixels = (DOUBLEPIXEL*)((addr)RendererState.Surfaces.Stencil
        + (addr)((ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL)));

    CONST S32 length = ACQUIREWIDTH(width);

    if (-1 < offset)
    {
        CONST PIXEL pixel = (PIXEL)(offset << STENCIL_PIXEL_COLOR_SHIFT);
        CONST DOUBLEPIXEL pix = ((DOUBLEPIXEL)(pixel) << DEFAULT_SCREEN_DEPTH_BITS) | (DOUBLEPIXEL)pixel;

        if (y < ModuleState.Surface.Y)
        {
            CONST S32 delta = y + height - ModuleState.Surface.Y;

            if ((y + height) < ModuleState.Surface.Y || delta == 0)
            {
                for (S32 yy = 0; yy < height; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                    pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
                }
            }
            else
            {
                for (S32 yy = 0; yy < height - delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                    pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
                }

                pixels = (DOUBLEPIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

                for (S32 yy = 0; yy < delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                    pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
                }
            }
        }
        else
        {
            pixels = (DOUBLEPIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] + pix; }

                pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
            }
        }
    }
    else
    {
        CONST PIXEL pixel = (PIXEL)(-offset << STENCIL_PIXEL_COLOR_SHIFT);
        CONST DOUBLEPIXEL pix = ((DOUBLEPIXEL)(pixel) << DEFAULT_SCREEN_DEPTH_BITS) | (DOUBLEPIXEL)pixel;

        if (y < ModuleState.Surface.Y)
        {
            CONST S32 delta = y + height - ModuleState.Surface.Y;

            if ((y + height) < ModuleState.Surface.Y || delta == 0)
            {
                for (S32 yy = 0; yy < height; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                    pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
                }
            }
            else
            {
                for (S32 yy = 0; yy < height - delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                    pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
                }

                pixels = (DOUBLEPIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

                for (S32 yy = 0; yy < delta; yy++)
                {
                    for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                    pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
                }
            }
        }
        else
        {
            pixels = (DOUBLEPIXEL*)((addr)pixels - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < length / 2; xx++) { pixels[xx] = pixels[xx] - pix; }

                pixels = (DOUBLEPIXEL*)((addr)pixels + (addr)(length * sizeof(PIXEL) + stride));
            }
        }
    }
}

// 0x10002900
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
            ModuleState.Surface.Renderer = (PIXEL*)desc.lpSurface;

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

// 0x10002980
VOID UnlockRendererSurface()
{
    ModuleState.DirectX.Surface->Unlock(NULL);

    ModuleState.Surface.Renderer = NULL;
}

// 0x100029a0
BOOL WriteRendererSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* pixels)
{
    BOOL locked = FALSE;

    if (ModuleState.Surface.Renderer == NULL)
    {
        if (!LockRendererSurface()) { return FALSE; }

        locked = TRUE;
    }

    PIXEL* src = (PIXEL*)((addr)pixels + (addr)((stride * sy + sx) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((addr)ModuleState.Surface.Renderer + (addr)(ModuleState.Pitch * dy + dx * sizeof(PIXEL)));

    CONST S32 length = ACQUIREWIDTH(width);

    for (S32 yy = 0; yy < height; yy++)
    {
        for (S32 xx = 0; xx < length; xx++) { dst[xx] = src[xx]; }

        src = (PIXEL*)((addr)src + (addr)(stride * sizeof(PIXEL)));
        dst = (PIXEL*)((addr)dst + (addr)ModuleState.Pitch);
    }

    if (locked) { UnlockRendererSurface(); }

    return locked;
}

// 0x10002a40
VOID WriteSurfaceSurfaceRectangle(S32 sx, S32 sy, S32 sstr, PIXEL* input, S32 dx, S32 dy, S32 dstr, PIXEL* output, S32 width, S32 height)
{
    PIXEL* src = (PIXEL*)((addr)input + (addr)((sstr * sy + sx) * sizeof(PIXEL)));
    PIXEL* dst = (PIXEL*)((addr)output + (addr)((dstr * dy + dx) * sizeof(PIXEL)));

    for (s32 yy = 0; yy < height; yy++)
    {
        for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

        src = (PIXEL*)((addr)src + (addr)(sstr * sizeof(PIXEL)));
        dst = (PIXEL*)((addr)dst + (addr)(dstr * sizeof(PIXEL)));
    }
}

// 0x10002aa0
BOOL WriteMainSurfaceRendererSurfaceRectangle(S32 x, S32 y, S32 width, S32 height)
{
    BOOL locked = FALSE;

    if (ModuleState.Surface.Renderer == NULL)
    {
        if (!LockRendererSurface()) { return FALSE; }

        locked = TRUE;
    }

    PIXEL* src = (PIXEL*)((addr)RendererState.Surfaces.Main
        + (addr)(ModuleState.Surface.Offset + y * MAX_RENDERER_WIDTH + x) * sizeof(PIXEL));
    PIXEL* dst = (PIXEL*)((addr)ModuleState.Surface.Renderer + (addr)(ModuleState.Pitch * y + x * sizeof(PIXEL)));

    if (y < ModuleState.Surface.Y)
    {
        CONST S32 delta = y + height - ModuleState.Surface.Y;

        if ((y + height) < ModuleState.Surface.Y || delta == 0)
        {
            for (S32 yy = 0; yy < height; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)ModuleState.Pitch);
            }
        }
        else
        {
            for (S32 yy = 0; yy < height - delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)ModuleState.Pitch);
            }

            src = (PIXEL*)((addr)src - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

            for (S32 yy = 0; yy < delta; yy++)
            {
                for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

                src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
                dst = (PIXEL*)((addr)dst + (addr)ModuleState.Pitch);
            }
        }
    }
    else
    {
        src = (PIXEL*)((addr)src - (addr)(MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL)));

        for (S32 yy = 0; yy < height; yy++)
        {
            for (S32 xx = 0; xx < width; xx++) { dst[xx] = src[xx]; }

            src = (PIXEL*)((addr)src + (addr)(MAX_RENDERER_WIDTH * sizeof(PIXEL)));
            dst = (PIXEL*)((addr)dst + (addr)ModuleState.Pitch);
        }
    }

    if (locked) { UnlockRendererSurface(); }

    return locked;
}

// 0x10002ba0
BOOL FUN_10002ba0(S32 x, S32 y, S32 width, S32 height)
{
    // TODO NOT IMPLEMENTED

    return TRUE;
}

// 0x10002fc0
VOID FUN_10002fc0(S32 x, S32 y, S32 width, S32 height)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10003330
S32 FUN_10003330(U8* param_1, U8* param_2)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED

    return 0;
}

// 0x10003370
VOID FUN_10003370(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100033d0
VOID FUN_100033d0(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100043a0
VOID FUN_100043a0(S32 param_1, S32 param_2, LPVOID param_3)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100046c6
VOID FUN_100046c6(S32 param_1, S32 param_2, LPVOID param_3)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100049f6
VOID FUN_100049f6(S32 param_1, S32 param_2, U16 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10004dc0
VOID FUN_10004dc0(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100050ef
VOID DrawMainSurfaceText(S32 x, S32 y, LPVOID param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100053d3
VOID FUN_100053d3(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100057ac
VOID FUN_100057ac(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10005ad6
VOID FUN_10005ad6(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10005e41
VOID FUN_10005e41(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x1000619d
VOID FUN_1000619d(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100064c6
VOID DrawMainSurfaceSprite(S32 x, S32 y, SPRITEPTR sprite)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100067bd
VOID FUN_100067bd(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10006b31
VOID DrawMainSurfaceCursor(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10006f08
VOID FUN_10006f08(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100072a2
VOID FUN_100072a2(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007672
VOID FUN_10007672(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007938
VOID FUN_10007938(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007bf8
VOID FUN_10007bf8(S32 x, S32 y, U16 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007fcc
VOID FUN_10007fcc(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10008edd
VOID FUN_10008edd(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10009ec3
VOID FUN_10009ec3(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x1000a503
VOID FUN_1000a503(S32 param_1, S32 param_2, S32 param_3, S32 param_4, LPVOID param_5, LPVOID param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}
