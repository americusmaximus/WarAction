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
    if (State.Window->Class.hInstance == NULL)
    {
        State.Window->Class.hInstance = State.Window->Instance;
    }

    if (State.Window->Class.hCursor == NULL)
    {
        State.Window->Class.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
    }

    if (State.Window->Class.hIcon == NULL)
    {
        State.Window->Class.hIcon = LoadIconA(NULL, (LPCSTR)IDI_APPLICATION);
    }

    RegisterClassA(&State.Window->Class);

    State.Window->HWND = CreateWindowExA(WS_EX_NONE,
        State.Window->Class.lpszClassName,
        State.Window->Title, State.Window->Style,
        State.Window->X, State.Window->Y,
        State.Window->Width, State.Window->Height,
        NULL, State.Window->Menu, State.Window->Instance, NULL);

    if (State.Window->HWND == NULL) { return FALSE; }

    ShowWindow(State.Window->HWND, command);

    UpdateWindow(State.Window->HWND);

    return TRUE;
}

// 0x00402530
INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR args, INT command)
{
    InitializeState();

    if (State.Window == NULL)
    {
        MessageBoxA(NULL, "Problem with process description.", "Fatal Error", MB_ICONHAND); return EXIT_SUCCESS;
    }

    State.Window->Instance = instance;
    State.Window->Args = args;
    State.Window->IsActive = TRUE;

    // Run all activate events.
    for (State.Handlers.Active = State.Handlers.Activate;
        State.Handlers.Active != NULL; State.Handlers.Active = State.Handlers.Active->Next)
    {
        if (!INVOKEACTIONHANDLERLAMBDA(State.Handlers.Active->Invoke)) { return EXIT_SUCCESS; }

        if (State.Handlers.Active == NULL) { break; }
    }

    if (!InitializeWindow(command))
    {
        MessageBoxA(NULL, "Internal-init process failed", "Error", MB_ICONHAND); return EXIT_SUCCESS;
    }

    // Run all initialize events.
    for (State.Handlers.Active = State.Handlers.Initialize;
        State.Handlers.Active != NULL; State.Handlers.Active = State.Handlers.Active->Next)
    {
        if (!INVOKEACTIONHANDLERLAMBDA(State.Handlers.Active->Invoke))
        {
            // Run release events in case at least one initialize event failed.
            for (State.Handlers.Active = State.Handlers.Release;
                State.Handlers.Active != NULL; State.Handlers.Active = State.Handlers.Active->Next)
            {
                INVOKEACTIONHANDLERLAMBDA(State.Handlers.Active->Invoke);

                if (State.Handlers.Active == NULL) { break; }
            }

            return EXIT_FAILURE;
        }

        if (State.Handlers.Active == NULL) { break; }
    }

    while (TRUE)
    {
        MSG message;

        if (PeekMessageA(&message, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessageA(&message, NULL, 0, 0))
            {
                // Run all release events.
                for (State.Handlers.Active = State.Handlers.Release;
                    State.Handlers.Active != NULL; State.Handlers.Active = State.Handlers.Active->Next)
                {
                    INVOKEACTIONHANDLERLAMBDA(State.Handlers.Active->Invoke);

                    if (State.Handlers.Active == NULL) { break; }
                }

                return EXIT_FAILURE;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);

            continue;
        }

        // Run all action events.
        for (State.Handlers.Active = State.Handlers.Action;
            State.Handlers.Active != NULL; State.Handlers.Active = State.Handlers.Active->Next)
        {
            if (!INVOKEACTIONHANDLERLAMBDA(State.Handlers.Active->Invoke))
            {
                // Run release events in case at least one action event failed.
                for (State.Handlers.Active = State.Handlers.Release;
                    State.Handlers.Active != NULL; State.Handlers.Active = State.Handlers.Active->Next)
                {
                    INVOKEACTIONHANDLERLAMBDA(State.Handlers.Active->Invoke);

                    if (State.Handlers.Active == NULL) { break; }
                }

                return EXIT_FAILURE;
            }

            if (State.Handlers.Active == NULL) { break; }
        }
    }

    return EXIT_SUCCESS;
}