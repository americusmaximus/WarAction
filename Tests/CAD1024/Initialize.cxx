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

#include "Initialize.hxx"

#define WINDOW_CLASS_NAME "CAD1024"
#define WINDOW_TITLE_NAME "CAD x 1024"

VOID Initialize(RENDERERPTR state)
{
    ZeroMemory(state->Surface.Main, MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL));
    ZeroMemory(state->Surface.Back, MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL));
    ZeroMemory(state->Surface.Stencil, MAX_RENDERER_WIDTH * MAX_RENDERER_HEIGHT * sizeof(PIXEL));
}

BOOL RegisterWindowClass(VOID)
{
    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(WNDCLASSA));

    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    return RegisterClassA(&wc) != NULL;
}

BOOL ReleaseWindowClass(VOID)
{
    return UnregisterClassA(WINDOW_CLASS_NAME, GetModuleHandle(NULL));
}

HWND InitializeWindow(VOID)
{
    return CreateWindowA(
        WINDOW_CLASS_NAME,
        WINDOW_TITLE_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, MAX_RENDERER_WIDTH, MAX_RENDERER_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
}

BOOL ReleaseWindow(HWND hWnd)
{
    return DestroyWindow(hWnd);
}