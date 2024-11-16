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

#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "Logger.hxx"
#include "Module.hxx"
#include "State.hxx"
#include "Window.hxx"

#include <stdlib.h>

#define WINDOW_CLASS_STYLE_NONE             0

#define WINDOW_ACTION_HANDLER_PRIORITY      0x8100

// ORIGINAL: VITYACLASS
#define DEFAULT_WINDOW_STATE_CLASS_NAME     "WarAction"

// ORIGINAL: Just a simple VITYA window :-)
#define DEFAULT_WINDOW_STATE_TITLE_NAME     "War Action"

#define DEFAULT_WINDOW_STATE_WIDTH          200
#define DEFAULT_WINDOW_STATE_HEIGHT         200

#define DEFAULT_INITIAL_CURSOR_POSITION     (-100000)
#define DEFAULT_UPDATE_CURSOR_POSITION      (-1000000)

#define MAX_WINDOW_MESSAGE_CHARACTER_LENGTH 128

WINDOWSTATEMODULECONTAINER WindowState;

// 0x10021f10
WINDOWPTR CLASSCALL ActivateWindowState(WINDOWPTR self, ACTIONHANDLERLAMBDA init, ACTIONHANDLERLAMBDA execute, ACTIONHANDLERLAMBDA release)
{
    INITIALIZEACTIONHANDLER(&ActionState.Initialize, init);
    INITIALIZEACTIONHANDLER(&ActionState.Execute, execute);
    INITIALIZEACTIONHANDLER(&ActionState.Release, release);

    InitializeWindowState(self);

    return self;
}

// 0x10022e20
LRESULT WINAPI WindowStateMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY: { PostQuitMessage(EXIT_SUCCESS); break; }
    case WM_SETFOCUS: { State.Window->IsActive = TRUE; break; }
    case WM_KILLFOCUS: { State.Window->IsActive = FALSE; break; }
    }

    for (ActionState.Active = ActionState.Message;
        ActionState.Active != NULL; ActionState.Active = ActionState.Active->Next)
    {
        LRESULT result = 0;
        if (!INVOKEWINDOWACTIONHANDLERLAMBDA(ActionState.Active->Action, hwnd, msg, wp, lp, &result)) { return result; }

        if (ActionState.Active->Next == NULL) { break; }
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

// 0x10022d90
VOID CLASSCALL InitializeWindowState(WINDOWPTR self)
{
    State.Window = self;

    self->Instance = NULL;
    self->Args = NULL;
    self->HWND = NULL;

    self->Class.style = WINDOW_CLASS_STYLE_NONE;
    self->Class.lpfnWndProc = WindowStateMessageHandler;
    self->Class.cbClsExtra = 0;
    self->Class.cbWndExtra = 0;
    self->Class.hInstance = NULL;
    self->Class.hIcon = NULL;
    self->Class.hCursor = NULL;
    self->Class.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    self->Class.lpszMenuName = NULL;
    self->Class.lpszClassName = DEFAULT_WINDOW_STATE_CLASS_NAME;

    strcpy(self->Title, DEFAULT_WINDOW_STATE_TITLE_NAME);

    self->Style = WS_POPUP;

    self->Y = 0;
    self->X = 0;
    self->Width = DEFAULT_WINDOW_STATE_WIDTH;
    self->Height = DEFAULT_WINDOW_STATE_HEIGHT;

    self->Menu = NULL;

    self->IsActive = TRUE;
}

// 0x10022320
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

    WindowState.Ticks = 0;
    WindowState.State = state;

    WindowState.Cursor.Left.X = WindowState.Cursor.Left.Y = DEFAULT_INITIAL_CURSOR_POSITION;
    WindowState.Cursor.Right.X = WindowState.Cursor.Right.Y = DEFAULT_INITIAL_CURSOR_POSITION;
}

// 0x10022f30
VOID InitializeWindowActionHandler(CONST U32 priority, WINDOWACTIONHANDLERLAMBDA action)
{
    InitializeActionHandler(&ActionState.Message, priority, (ACTIONHANDLERLAMBDA)action);
}

// 0x10022ed0
VOID InitializeActionHandler(ACTIONHANDLERPTR* destination, CONST U32 priority, ACTIONHANDLERLAMBDA action)
{
    if (action != NULL)
    {
        ACTIONHANDLERPTR handler = ALLOCATE(ACTIONHANDLER);

        if (handler != NULL) { InitializeActionHandler(handler, destination, priority, action); }
    }
}

// 0x100228f0
BOOL WindowMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result)
{
    if (WindowState.State & WINDOWINPUTSTATE_ACCEPT_MOUSE)
    {
        CONST U32 ticks = WindowState.IsActive ? GetTickCount() : wp;

        CONST S32 x = CursorState.X;
        CONST S32 y = CursorState.Y;

        switch (msg)
        {
        case WM_MOUSEMOVE:
        {
            CursorState.X = GET_X_LPARAM(lp);
            CursorState.Y = GET_Y_LPARAM(lp);

            SelectCursorCoordinates(x, y, CursorState.X, CursorState.Y);

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_LBUTTONDOWN:
        {
            CursorMessageHandler(CURSORMESSAGE_LEFT_BUTTON_DOWN);

            if (WindowState.IsActive)
            {
                CONST U32 click = GetDoubleClickTime();

                if (ticks - WindowState.Ticks < click)
                {
                    CONST BOOL xs = (CursorState.X - WindowState.Cursor.Left.X) >> 0x1F;

                    if (((CursorState.X - WindowState.Cursor.Left.X ^ xs) - xs) < 4)
                    {
                        CONST BOOL ys = (CursorState.Y - WindowState.Cursor.Left.Y) >> 0x1F;

                        if (((CursorState.Y - WindowState.Cursor.Left.Y ^ ys) - ys) < 4)
                        {
                            CursorMessageHandler(CURSORMESSAGE_LEFT_BUTTON_DOUBLE_CLICK);
                        }
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
        case WM_LBUTTONUP: { CursorMessageHandler(CURSORMESSAGE_LEFT_BUTTON_UP); break; }
        case WM_LBUTTONDBLCLK: { CursorMessageHandler(CURSORMESSAGE_LEFT_BUTTON_DOUBLE_CLICK); break; }
        case WM_RBUTTONDOWN:
        {
            CursorMessageHandler(CURSORMESSAGE_RIGHT_BUTTON_DOWN);

            if (WindowState.IsActive)
            {
                CONST U32 click = GetDoubleClickTime();

                if (ticks - WindowState.Ticks < click)
                {
                    CONST BOOL xs = (CursorState.X - WindowState.Cursor.Right.X) >> 0x1F;

                    if (((CursorState.X - WindowState.Cursor.Right.X ^ xs) - xs) < 4)
                    {
                        CONST BOOL ys = (CursorState.Y - WindowState.Cursor.Right.Y) >> 0x1F;

                        if (((CursorState.Y - WindowState.Cursor.Right.Y ^ ys) - ys) < 4)
                        {
                            CursorMessageHandler(CURSORMESSAGE_RIGHT_BUTTON_DOUBLE_CLICK);
                        }
                    }
                }

                WindowState.Ticks = ticks;

                WindowState.Cursor.Left.Y = CursorState.Y;
                WindowState.Cursor.Left.X = CursorState.X;

                WindowState.Cursor.Right.X = WindowState.Cursor.Right.Y = DEFAULT_UPDATE_CURSOR_POSITION;
            }

            CursorState.IsLeft = wp & MK_LBUTTON;
            CursorState.IsRight = wp & MK_RBUTTON;

            break;
        }
        case WM_RBUTTONUP: { CursorMessageHandler(CURSORMESSAGE_RIGHT_BUTTON_UP); break; }
        case WM_RBUTTONDBLCLK: { CursorMessageHandler(CURSORMESSAGE_RIGHT_BUTTON_DOUBLE_CLICK); break; }
        }
    }

    if (WindowState.State & WINDOWINPUTSTATE_ACCEPT_KEYBOARD)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            EnqueueControlCommand(CONTROLCOMMAND_KBD, wp + 0x100, CursorState.X, CursorState.Y); // TODO
            EnqueueControlCommand(CONTROLCOMMAND_UTF, wp + 0x1000000, CursorState.X, CursorState.Y); // TODO

            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            EnqueueControlCommand(CONTROLCOMMAND_KBD, wp + 0x200, CursorState.X, CursorState.Y); // TODO
            EnqueueControlCommand(CONTROLCOMMAND_UTF, wp + 0x2000000, CursorState.X, CursorState.Y); // TODO

            break;
        }
        case WM_CHAR:
        case WM_SYSCHAR:
        {
            EnqueueControlCommand(CONTROLCOMMAND_KBD, wp, CursorState.X, CursorState.Y);

            if (!IsAnsiCharacter(wp))
            {
                if (WindowState.Key == 0)
                {
                    EnqueueControlCommand(CONTROLCOMMAND_UTF,
                        AcquireUnicodeCharacter(wp & 0xFF), CursorState.X, CursorState.Y);

                    WindowState.Key = 0;
                }
            }
            else if (WindowState.Key == 0) { WindowState.Key = wp & 0xFF; }
            else
            {
                EnqueueControlCommand(CONTROLCOMMAND_UTF,
                    AcquireUnicodeCharacter((wp & 0xFF) << 8 | WindowState.Key), CursorState.X, CursorState.Y);
                
                WindowState.Key = 0;
            }

            break;
        }

        break;
        }
    }

    return TRUE;
}

// 0x10023320
BOOL IsAnsiCharacter(U8 value)
{
    return IsDBCSLeadByte(value);
}

// 0x10023330
U32 AcquireUnicodeCharacter(CONST U32 value)
{
    WCHAR string[MAX_WINDOW_MESSAGE_CHARACTER_LENGTH];

    CONST S32 result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
        (LPCCH)&value, -1, string, MAX_WINDOW_MESSAGE_CHARACTER_LENGTH * sizeof(WCHAR));

    LogMessage("KB A2U %X %X res=%d\n", value, string[0], result);

    if (result != 2) { string[0] = '?'; string[1] = NULL; }

    return (U32)(((U32)string[0]) << 16 | string[1]);
}