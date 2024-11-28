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

#include "Bink.hxx"
#include "Video.hxx"

#include <Renderer.Basic.hxx>

#define BINK_IO_SIZE    2000000
#define BINK_MAX_FRAMES 10000000

// 0x100015c0
BOOL InitializeBink(LPCSTR name, RENDERERMODULESTATECONTAINERPTR renderer, SOUNDPTR sound, CONST U32 max, CONST U32 frames)
{
    VideoState.Renderer = renderer;

    BinkSoundUseDirectSound(sound->Instance);

    BinkSetIOSize(BINK_IO_SIZE);

    VideoState.Instance = BinkOpen(name, BINKIOSIZE);

    if (max == 0)
    {
        VideoState.MaxFrames = max;

        if (frames != VideoState.Instance->Frames) { VideoState.MaxFrames = BINK_MAX_FRAMES; }
    }

#if ACTIVATE_MODERN_MODE
    {
        LPDIRECTDRAWSURFACE surface = NULL;
        if (FAILED(surface->QueryInterface(IID_IDirectDrawSurface, (LPVOID*)&surface))) { return FALSE; }

        VideoState.Surface = BinkDDSurfaceType(surface);

        DIRECTDRAWRELEASE(surface);
    }
#else
    VideoState.Surface = BinkDDSurfaceType(VideoState.Renderer->DirectX.Surface);
#endif

    if (VideoState.Surface == BINKSURFACEINVALID || VideoState.Surface == BINKSURFACE8P)
    {
        MessageBoxA(NULL, "Unsupported primary surface format.", "Error", MB_ICONHAND | MB_OK);

        return FALSE;
    }

    return TRUE;
}

// 0x10001660
BINKVIDEORESULT PlayBinkVideo(VOID)
{
    if (!BinkWait(VideoState.Instance))
    {
        return ShowBinkFrame() ? BINKVIDEORESULT_COMPLETED : BINKVIDEORESULT_CONTINUE;
    }

    return BINKVIDEORESULT_CONTINUE;
}

// 0x10001690
BOOL ShowBinkFrame(VOID)
{
    BinkDoFrame(VideoState.Instance);

    DDSURFACEDESC desc; // 0x10046000
    ZeroMemory(&desc, sizeof(DDSURFACEDESC));

    desc.dwSize = sizeof(DDSURFACEDESC);

    RECT rect = { 0, 0, GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480 };

    if (!VideoState.Renderer->IsFullScreen)
    {
        POINT point = { 0 };

        GetClientRect(VideoState.Renderer->HWND, &rect);
        ClientToScreen(VideoState.Renderer->HWND, &point);
        OffsetRect(&rect, point.x, point.y);
    }

    if (VideoState.Renderer->DirectX.Surface->Lock(VideoState.Renderer->IsFullScreen
        ? NULL : &rect, &desc, DDLOCK_WAIT, NULL) == DD_OK)
    {
        BinkCopyToBuffer(VideoState.Instance,
            desc.lpSurface, desc.lPitch, VideoState.Instance->Width,
            (GRAPHICS_RESOLUTION_640 - VideoState.Instance->Width) / 2,
            (GRAPHICS_RESOLUTION_480 - VideoState.Instance->Height) / 2, VideoState.Surface);

        VideoState.Renderer->DirectX.Surface->Unlock(desc.lpSurface);
    }

    if (VideoState.Instance->FrameNum < VideoState.Instance->Frames
        && VideoState.Instance->FrameNum < VideoState.MaxFrames)
    {
        BinkNextFrame(VideoState.Instance);

        return FALSE;
    }

    return TRUE;
}

// 0x10001760
BOOL ReleaseBink(VOID)
{
    BinkClose(VideoState.Instance);

    return TRUE;
}