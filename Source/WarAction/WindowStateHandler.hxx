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

#define WINDOW_STATE_HANDLER_DEFAULT_PRIORITY_VALUE 0x10000000

#define WINDOW_STATE_MAX_HANDLER_COUNT 5

#define WINDOW_STATE_START_HANDLER_INDEX 0
#define WINDOW_STATE_INITIALIZE_HANDLER_INDEX 1
#define WINDOW_STATE_ACTION_HANDLER_INDEX 2
#define WINDOW_STATE_RELEASE_HANDLER_INDEX 3
#define WINDOW_STATE_MESSAGE_HANDLER_INDEX 4

typedef BOOL(*WINDOWSTATEHANDLERLAMBDA)(VOID);
typedef BOOL(*WINDOWSTATEMESSAGEHANDLERLAMBDA)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result);

#define INVOKEWINDOWSTATEHANDLERLAMBDA(ACTION) ((WINDOWSTATEHANDLERLAMBDA)ACTION)()
#define INVOKEWINDOWSTATEMESSAGEHANDLERLAMBDA(ACTION, WND, MSG, WP, LP, RESULT) ((WINDOWSTATEMESSAGEHANDLERLAMBDA)ACTION)(WND, MSG, WP, LP, RESULT)

typedef struct WindowStateContainerHandler
{
    LPVOID Invoke;
    U32 Priority;
    WindowStateContainerHandler* Next;
} WINDOWSTATECONTAINERHANDLER, * LPWINDOWSTATECONTAINERHANDLER;

VOID InitializeWindowStateHandler(LPWINDOWSTATECONTAINERHANDLER self, LPWINDOWSTATECONTAINERHANDLER* destination, U32 priority, WINDOWSTATEHANDLERLAMBDA lambda);

VOID InitializeWindowStateActionHandler(U32 priority, WINDOWSTATEHANDLERLAMBDA lambda);
VOID InitializeWindowStateMessageHandler(U32 priority, WINDOWSTATEMESSAGEHANDLERLAMBDA lambda);

VOID ReleaseWindowStateActionHandler(WINDOWSTATEHANDLERLAMBDA lambda);
VOID ReleaseWindowStateMessageHandler(WINDOWSTATEMESSAGEHANDLERLAMBDA lambda);