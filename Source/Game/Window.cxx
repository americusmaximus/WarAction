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

#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "Logger.hxx"
#include "Module.hxx"
#include "State.hxx"
#include "Window.hxx"

#include <stdlib.h>

#define WINDOW_CLASS_STYLE_NONE             0

#define WINDOW_ACTION_HANDLER_PRIORITY      0x8100

#define DEFAULT_INITIAL_CURSOR_POSITION     (-100000)
#define DEFAULT_UPDATE_CURSOR_POSITION      (-1000000)

#define MAX_DOUBLE_CLICK_DISTANCE           4
#define MAX_WINDOW_MESSAGE_CHARACTER_LENGTH 128

WINDOWCONTAINER WindowState;

// 0x1008e5d0
VOID SelectWindowInputState(CONST WINDOWINPUTSTATE state)
{
    if (!(state & (WINDOWINPUTSTATE)(WINDOWINPUTSTATE_ACCEPT_KEYBOARD | WINDOWINPUTSTATE_ACCEPT_MOUSE)))
    {
        if (WindowState.State != WINDOWINPUTSTATE_NONE) { ReleaseWindowActionHandler(WindowMessageHandler); }
    }
    else if (WindowState.State == WINDOWINPUTSTATE_NONE)
    {
        InitializeWindowActionHandler(WINDOW_ACTION_HANDLER_PRIORITY, WindowMessageHandler);
    }

    WindowState.State = state;

    WindowState.Cursor.Ticks = 0;
    WindowState.Cursor.Left.X = WindowState.Cursor.Left.Y = DEFAULT_INITIAL_CURSOR_POSITION;
    WindowState.Cursor.Right.X = WindowState.Cursor.Right.Y = DEFAULT_INITIAL_CURSOR_POSITION;
}

// 0x1008ead0
BOOL WindowMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result)
{
    if (WindowState.State & WINDOWINPUTSTATE_ACCEPT_MOUSE)
    {
        CONST U32 ticks = WindowState.IsActive ? GetTickCount() : wp;

        switch (msg)
        {
        case WM_MOUSEMOVE:
        {
            CONST S32 x = CursorState.X;
            CONST S32 y = CursorState.Y;

            CursorState.X = GET_X_LPARAM(lp);
            CursorState.Y = GET_Y_LPARAM(lp);

            SelectCursorCoordinates(x, y, CursorState.X, CursorState.Y);

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_LBUTTONDOWN:
        {
            CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_LEFT_DOWN);

            if (WindowState.IsActive)
            {
                if (ticks - WindowState.Cursor.Ticks < GetDoubleClickTime())
                {
                    if (abs(CursorState.X - WindowState.Cursor.Left.X) < MAX_DOUBLE_CLICK_DISTANCE
                        && abs(CursorState.Y - WindowState.Cursor.Left.Y) < MAX_DOUBLE_CLICK_DISTANCE)
                    {
                        CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_LEFT_DOUBLECLICK);
                    }
                }

                WindowState.Cursor.Ticks = ticks;

                WindowState.Cursor.Left.X = CursorState.X;
                WindowState.Cursor.Left.Y = CursorState.Y;

                WindowState.Cursor.Right.X = WindowState.Cursor.Right.Y = DEFAULT_UPDATE_CURSOR_POSITION;
            }

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_LBUTTONUP:
        {
            CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_LEFT_UP);

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_LEFT_DOUBLECLICK);

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_RBUTTONDOWN:
        {
            CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_RIGHT_DOWN);

            if (WindowState.IsActive)
            {
                if (ticks - WindowState.Cursor.Ticks < GetDoubleClickTime())
                {
                    if (abs(CursorState.X - WindowState.Cursor.Right.X) < MAX_DOUBLE_CLICK_DISTANCE
                        && abs(CursorState.Y - WindowState.Cursor.Right.Y) < MAX_DOUBLE_CLICK_DISTANCE)
                    {
                        CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_RIGHT_DOUBLECLICK);
                    }
                }

                WindowState.Cursor.Ticks = ticks;

                WindowState.Cursor.Right.X = CursorState.X;
                WindowState.Cursor.Right.Y = CursorState.Y;

                WindowState.Cursor.Left.X = WindowState.Cursor.Left.Y = DEFAULT_UPDATE_CURSOR_POSITION;
            }

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_RBUTTONUP:
        {
            CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_RIGHT_UP);

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_RBUTTONDBLCLK:
        {
            CursorMessageHandler(CONTROLCOMMANDACTION_MOUSE_RIGHT_DOUBLECLICK);

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        }
    }

    if (WindowState.State & WINDOWINPUTSTATE_ACCEPT_KEYBOARD)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            EnqueueControlCommand(CONTROLCOMMAND_KBD,
                wp + CONTROLCOMMANDACTION_KBD_KEY_DOWN, CursorState.X, CursorState.Y);
            EnqueueControlCommand(CONTROLCOMMAND_UTF,
                wp + CONTROLCOMMANDACTION_UTF_KEY_DOWN, CursorState.X, CursorState.Y);

            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            EnqueueControlCommand(CONTROLCOMMAND_KBD,
                wp + CONTROLCOMMANDACTION_KBD_KEY_UP, CursorState.X, CursorState.Y);
            EnqueueControlCommand(CONTROLCOMMAND_UTF,
                wp + CONTROLCOMMANDACTION_UTF_KEY_UP, CursorState.X, CursorState.Y);

            break;
        }
        case WM_CHAR:
        case WM_SYSCHAR:
        {
            EnqueueControlCommand(CONTROLCOMMAND_KBD, wp, CursorState.X, CursorState.Y);

            if (!IsAnsiCharacter(wp))
            {
                if (WindowState.Key == VK_NULL)
                {
                    EnqueueControlCommand(CONTROLCOMMAND_UTF,
                        AcquireUnicodeCharacter(wp & VK_INPUT), CursorState.X, CursorState.Y);

                    WindowState.Key = VK_NULL;
                }
            }
            else if (WindowState.Key == VK_NULL) { WindowState.Key = wp & VK_INPUT; }
            else
            {
                EnqueueControlCommand(CONTROLCOMMAND_UTF,
                    AcquireUnicodeCharacter((wp & VK_INPUT) << 8 | WindowState.Key), CursorState.X, CursorState.Y);

                WindowState.Key = VK_NULL;
            }

            break;
        }
        }
    }

    return TRUE;
}

// 0x1008fdb0
VOID InitializeWindowActionHandler(CONST U32 priority, WINDOWACTIONHANDLERLAMBDA action)
{
    InitializeActionHandler(&ActionState.Message, priority, (ACTIONHANDLERLAMBDA)action);
}

// 0x1008fdd0
VOID ReleaseWindowActionHandler(WINDOWACTIONHANDLERLAMBDA lambda)
{
    ReleaseActionHandler(ActionState.Message, (ACTIONHANDLERLAMBDA)lambda);
}

// 0x1008f070
BOOL IsAnsiCharacter(U8 value)
{
    return IsDBCSLeadByte(value);
}

// 0x1008f080
U32 AcquireUnicodeCharacter(CONST U32 value)
{
    WCHAR string[MAX_WINDOW_MESSAGE_CHARACTER_LENGTH];

    CONST S32 result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
        (LPCCH)&value, -1, string, MAX_WINDOW_MESSAGE_CHARACTER_LENGTH * sizeof(WCHAR));

    LogMessage("KB A2U %X %X res=%d\n", value, string[0], result);

    if (result != 2) { string[0] = '?'; string[1] = NULL; }

    return *(U32*)string;
}