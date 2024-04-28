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

// 0x10001000
VOID InitializeRenderer()
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
BOOL InitializeDirectDraw(HWND hwnd, BOOL fullscreen)
{
    RestoreDisplayMode();

#if ACTIVATE_MODERN_MODE
    if (FAILED(DirectDrawCreateEx(NULL, (LPVOID*)&ModuleState.DirectDraw.Instance, IID_IDirectDraw7, NULL))) { return FALSE; }
#else
    if (FAILED(DirectDrawCreate(NULL, &ModuleState.DirectDraw.Instance, NULL))) { return FALSE; }
#endif

    if (FAILED(ModuleState.DirectDraw.Instance->SetCooperativeLevel(hwnd,
        fullscreen ? (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) : DDSCL_NORMAL))) { return FALSE; }

    ModuleState.IsFullScreen = fullscreen;
    ModuleState.HWND = hwnd;

    return TRUE;
}

// 0x100010d0
VOID RestoreDisplayMode()
{
    ReleaseRendererSurface();

    if (ModuleState.DirectDraw.Instance != NULL)
    {
        if (ModuleState.IsFullScreen) { ModuleState.DirectDraw.Instance->RestoreDisplayMode(); }
        
        ModuleState.DirectDraw.Instance->Release();
        ModuleState.DirectDraw.Instance = NULL;
    }
}

// 0x10001330
BOOL InitializeWindow(S32 width, S32 height)
{
    ReleaseRendererSurface();

    if (ModuleState.IsFullScreen)
    {
        if (FAILED(ModuleState.DirectDraw.Instance->SetDisplayMode(width, height, DEFAULT_SCREEN_DEPTH_BITS))) { return FALSE; }
    }

    SetWindowPos(ModuleState.HWND, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);

    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if (FAILED(ModuleState.DirectDraw.Instance->CreateSurface(&desc, &ModuleState.DirectDraw.Surface, NULL))) { return FALSE; }

    if (FAILED(ModuleState.DirectDraw.Surface->GetSurfaceDesc(&desc))) { return FALSE; }

    SetPixelColorMasks(desc.ddpfPixelFormat.dwRBitMask, desc.ddpfPixelFormat.dwGBitMask, desc.ddpfPixelFormat.dwBBitMask);

    ModuleState.Actions.InitializeRenderer();

    ModuleState.Window.X = 0;
    ModuleState.Window.Y = 0;
    ModuleState.Window.Width = width - 1;
    ModuleState.Window.Height = height - 1;

    return TRUE;
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

// 0x100010b0
VOID ReleaseRendererSurface()
{
    if (ModuleState.DirectDraw.Surface != NULL)
    {
        ModuleState.DirectDraw.Surface->Release();
        ModuleState.DirectDraw.Surface = NULL;
    }
}

// 0x100028f0
BOOL LockRendererSurface()
{
    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);

    HRESULT result = ModuleState.DirectDraw.Surface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

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
            if (FAILED(ModuleState.DirectDraw.Surface->Restore())) { break; }
        }

        result = ModuleState.DirectDraw.Surface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);
    }

    return FALSE;
}

// 0x10002970
VOID UnlockRendererSurface()
{
    ModuleState.DirectDraw.Surface->Unlock(NULL);

    ModuleState.Surface.Renderer = NULL;
}

// 0x10002810
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

// 0x10001be0
// TODO: Better name.
// TODO: Implement 32-bit colors.
VOID ConvertColorsExtra(PIXEL* input, PIXEL* output, S32 count)
{
    for (S32 x = 0; x < count; x++)
    {
        CONST PIXEL pixel = input[x];

        if (pixel == 0xF81F) { output[x] = 0xF81F; continue; }

        CONST PIXEL r = (((pixel & 0xF800) << 0) >> (ModuleState.RedOffset & 0x1f)) & ModuleState.ActualRedMask;
        CONST PIXEL g = (((pixel & 0x07E0) << 5) >> (ModuleState.GreenOffset & 0x1f)) & ModuleState.ActualGreenMask;
        CONST PIXEL b = (((pixel & 0x001F) << 11) >> (ModuleState.BlueOffset & 0x1f)) & ModuleState.ActualBlueMask;

        output[x] = (PIXEL)(r | g | b);
    }
}

// 0x10001c80
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

// 0x10003320
S32 FUN_10003320(U8* param_1, U8* param_2)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED

    return 0;
}

// 0x100033c0
VOID FUN_100033c0(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10003360
VOID FUN_10003360(S32 param_1, S32 param_2, U8* param_3, U8* param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

VOID FUN_10001e90(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6, S32 param_7)
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

// 0x10005e31
VOID FUN_10005e31(S32 param_1, S32 param_2, U16 param_3, S32 param_4, LPVOID param_5)
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

// 0x1000579c
VOID FUN_1000579c(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

VOID FUN_10007928(S32 param_1, S32 param_2, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001f80
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

// 0x10001ed0
VOID FUN_10001ed0(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5, S32 param_6)
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

// 0x10002fb0
VOID FUN_10002fb0(S32 x, S32 y, S32 width, S32 height)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100050df
VOID FUN_100050df(S32 x, S32 y, LPVOID param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100064b6
VOID FUN_100064b6(S32 x, S32 y, LPSPRITE sprite)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x1000618d
VOID FUN_1000618d(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10004db0
VOID FUN_10004db0(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10006ef8
VOID FUN_10006ef8(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007292
VOID FUN_10007292(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10006b21
VOID FUN_10006b21(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100067ad
VOID FUN_100067ad(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007662
VOID FUN_10007662(S32 x, S32 y, S32 param_3, LPVOID param_4)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10007fbc
VOID FUN_10007fbc(S32 x, S32 y, U16 param_3, S32 param_4, LPVOID param_5)
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

// 0x100053c3
VOID FUN_100053c3(S32 x, S32 y, S32 param_3, S32 param_4, LPVOID param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100017e0
VOID FUN_100017e0(S32 x, S32 y, PIXEL pixel)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
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

// 0x10001570
VOID DrawMainSurfaceColorBox(S32 x, S32 y, S32 width, S32 height, PIXEL pixel)
{
    DrawMainSurfaceHorizontalColorLine(x, y, width, pixel);
    DrawMainSurfaceHorizontalColorLine(x, y + height - 1, width, pixel);
    DrawMainSurfaceVerticalColorLine(x, y, height, pixel);
    DrawMainSurfaceVerticalColorLine(x + width - 1, y, height, pixel);
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

// 0x100014b0
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

// 0x100016c0
VOID FUN_100016c0(S32 x, S32 y, S32 width, S32 height, S32 param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x100023e0
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

// 0x10002020
VOID FUN_10002020(S32 param_1, S32 param_2, S32 param_3, S32 param_4, S32 param_5)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10002b90
BOOL FUN_10002b90(S32 x, S32 y, S32 width, S32 height)
{
    // TODO NOT IMPLEMENTED

    return TRUE;
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

// 0x100018a0
VOID FUN_100018a0(S32 x, S32 y, S32 width, S32 height, S32 param_5, S32 param_6, S32 param_7, S32 param_8)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10002780
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

// 0x100026e0
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

// 0x10002990
BOOL WriteRendererSurfaceSurfaceRectangle(S32 x, S32 y, S32 width, S32 height, S32 dx, S32 dy, S32 stride, PIXEL* pixels)
{
    BOOL locked = FALSE;

    if (ModuleState.Surface.Renderer == NULL)
    {
        if (!LockRendererSurface()) { return FALSE; }

        locked = TRUE;
    }

    PIXEL* src = (PIXEL*)((addr)pixels + (addr)((stride * y + x) * sizeof(PIXEL)));
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

// 0x10002a30
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

// 0x10008ecd
VOID FUN_10008ecd(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, LPVOID param_5)
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

// 0x10009eb3
VOID FUN_10009eb3(S32 param_1, S32 param_2, LPVOID param_3, S32 param_4, S32 param_5, S32 param_6)
{
    OutputDebugStringA(__FUNCTION__); OutputDebugStringA("\r\n");
    // TODO NOT IMPLEMENTED
}

// 0x10001110
VOID ReleaseDirectDraw(VOID)
{
    ReleaseRendererSurface();

    if (ModuleState.DirectDraw.Instance != NULL)
    {
        ModuleState.DirectDraw.Instance->Release();
        ModuleState.DirectDraw.Instance = NULL;
    }
}