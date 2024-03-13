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

#include "Basic.hxx"
#include "State.hxx"
#include "WarAction.hxx"

#define WS_EX_NONE 0

STATECONTAINER State;

// NOTE: A substitute for dynamic initialization preceding Main in the original game.
VOID InitializeState(VOID)
{
    ActivateSoundStateContainer();
    ActivateWindowStateContainer();
}

// 0x00402440
BOOL InitializeWindow(CONST INT command)
{
    if (State.Window.WindowState->Class.hInstance == NULL)
    {
        State.Window.WindowState->Class.hInstance = State.Window.WindowState->Instance;
    }

    if (State.Window.WindowState->Class.hCursor == NULL)
    {
        State.Window.WindowState->Class.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
    }

    if (State.Window.WindowState->Class.hIcon == NULL)
    {
        State.Window.WindowState->Class.hIcon = LoadIconA(NULL, (LPCSTR)IDI_APPLICATION);
    }

    RegisterClassA(&State.Window.WindowState->Class);

    State.Window.WindowState->HWND = CreateWindowExA(WS_EX_NONE,
        State.Window.WindowState->Class.lpszClassName,
        State.Window.WindowState->Title, State.Window.WindowState->Style,
        State.Window.WindowState->X, State.Window.WindowState->Y,
        State.Window.WindowState->Width, State.Window.WindowState->Height,
        NULL, State.Window.WindowState->Menu, State.Window.WindowState->Instance, NULL);

    if (State.Window.WindowState->HWND == NULL) { return FALSE; }

    ShowWindow(State.Window.WindowState->HWND, command);

    UpdateWindow(State.Window.WindowState->HWND);

    return TRUE;
}

// 0x00402530
INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR args, INT command)
{
    InitializeState();

    if (State.Window.WindowState == NULL)
    {
        MessageBoxA(NULL, "Problem with process description.", "Fatal Error", MB_ICONHAND); return EXIT_SUCCESS;
    }

    State.Window.WindowState->Instance = instance;
    State.Window.WindowState->Args = args;
    State.Window.WindowState->IsActive = TRUE;

    // Run all start events.
    for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_START_HANDLER_INDEX];
        State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
    {
        if (!INVOKEWINDOWSTATEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke)) { return EXIT_SUCCESS; }

        if (State.Window.ActiveHandler == NULL) { break; }
    }

    if (!InitializeWindow(command))
    {
        MessageBoxA(NULL, "Internal-init process failed", "Error", MB_ICONHAND); return EXIT_SUCCESS;
    }

    // Run all initialize events.
    for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_INITIALIZE_HANDLER_INDEX];
        State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
    {
        if (!INVOKEWINDOWSTATEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke))
        {
            // Run release events in case at least one initialize event failed.
            for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_RELEASE_HANDLER_INDEX];
                State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
            {
                INVOKEWINDOWSTATEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke);

                if (State.Window.ActiveHandler == NULL) { break; }
            }

            return EXIT_FAILURE;
        }

        if (State.Window.ActiveHandler == NULL) { break; }
    }

    while (TRUE)
    {
        MSG message;

        if (PeekMessageA(&message, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessageA(&message, NULL, 0, 0))
            {
                // Run all release events.
                for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_RELEASE_HANDLER_INDEX];
                    State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
                {
                    INVOKEWINDOWSTATEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke);

                    if (State.Window.ActiveHandler == NULL) { break; }
                }

                return EXIT_FAILURE;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);

            continue;
        }

        // Run all action events.
        for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_ACTION_HANDLER_INDEX];
            State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
        {
            if (!INVOKEWINDOWSTATEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke))
            {
                // Run release events in case at least one action event failed.
                for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_RELEASE_HANDLER_INDEX];
                    State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
                {
                    INVOKEWINDOWSTATEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke);

                    if (State.Window.ActiveHandler == NULL) { break; }
                }

                return EXIT_FAILURE;
            }

            if (State.Window.ActiveHandler == NULL) { break; }
        }
    }

    return EXIT_SUCCESS;
}