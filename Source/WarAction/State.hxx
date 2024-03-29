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

#include "AppState.hxx"
#include "SoundState.hxx"
#include "WindowState.hxx"

typedef struct StateContainer
{
    BOOL IsIniActive; // 0x00410370

    struct
    {
        HMODULE Module; // 0x00410374
        LPRENDERERSTATECONTAINER State; // 0x00410378
    } Renderer;

    LPMODULESTATECONTAINER ModuleState; // 0x00410380

    struct
    {
        HMODULE Handle; // 0x00410384
    } Text;

    LPAPPSTATECONTAINER AppState; // 0x004104fc

    LPSOUNDSTATECONTAINER SoundState; // 0x00410504

    struct
    {
        LPWINDOWSTATECONTAINERHANDLER Handlers[WINDOW_STATE_MAX_HANDLER_COUNT]; // 0x00410508
        LPWINDOWSTATECONTAINERHANDLER ActiveHandler; // 0x0041051c
        LPWINDOWSTATECONTAINER WindowState; // 0x00410520
    } Window;

    struct
    {
        CHAR* All; // 0x00410524
        CHAR** Args; // 0x00410528
        U32 Count; // 0x0041052c
    } Arguments;

    LPLOGGERSTATECONTAINER Logger; // 0x00410530
} STATECONTAINER, * LPSTATECONTAINER;

extern STATECONTAINER State;