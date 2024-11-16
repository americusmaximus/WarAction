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

#include "Action.hxx"

#include <DirectInput.hxx>

#define INPUT_ACTION_HANDLER_PRIORITY   0x8000

#define MAX_INPUT_STATE_COUNT           256

typedef struct InputStateModuleContainer
{
    ACTIONHANDLER           Release;                        // 0x1005ac18
    ACTIONHANDLER           Message;                        // 0x1005ac24
    ACTIONHANDLER           Initialize;                     // 0x1005ac30
    ACTIONHANDLER           Execute;                        // 0x1005ac3c

    LPDIRECTINPUTA          Instance;                       // 0x1005ac48

    LPDIRECTINPUTDEVICEA    Device;                         // 0x1005ad54

    U8                      State[MAX_INPUT_STATE_COUNT];   // 0x1005ac50
} INPUTSTATEMODULECONTAINER, * INPUTSTATEMODULECONTAINERPTR;

EXTERN INPUTSTATEMODULECONTAINER InputState;

BOOL InitializeDirectInputAction(VOID);
BOOL AcquireDirectInputAction(VOID);
BOOL ReleaseDirectInputAction(VOID);
BOOL InputMessageAction(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result);