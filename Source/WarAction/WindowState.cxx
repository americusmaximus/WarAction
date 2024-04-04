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

#include "RendererState.hxx"
#include "Settings.hxx"
#include "State.hxx"
#include "WarAction.hxx"

#include <Mathematics.Basic.hxx>
#include <..\Text\Resources.hxx>

#define CS_NONE 0

#define WINDOW_STATE_MAX_SETTING_VALUE_LENGTH 256

// ORIGINAL: VITYACLASS
#define WINDOW_STATE_DEFAULT_WINDOW_CLASS_NAME "WarAction"

// ORIGINAL: Just a simple VITYA window :-)
#define WINDOW_STATE_DEFAULT_WINDOW_TITLE_NAME "War Action"

#define WINDOW_STATE_DEFAULT_WINDOW_HEIGHT_VALUE 200
#define WINDOW_STATE_DEFAULT_WINDOW_WIDTH_VALUE 200

#define INITIALIZEWINDOWSTATEHANDLER(I, A)                                                                                          \
    if (A != NULL) {                                                                                                                \
        LPWINDOWSTATECONTAINERHANDLER handler = (LPWINDOWSTATECONTAINERHANDLER)malloc(sizeof(WINDOWSTATECONTAINERHANDLER));         \
        if (handler != NULL) {                                                                                                      \
            InitializeWindowStateHandler(handler, &State.Window.Handlers[I], WINDOW_STATE_HANDLER_DEFAULT_PRIORITY_VALUE, A);       \
        }                                                                                                                           \
    }

using namespace Mathematics;

// 0x00410388
WINDOWSTATECONTAINER WindowState;

// 0x00401130
VOID AcquireIniFileState()
{
    State.IsIniActive = GetPrivateProfileIntA("debug", "useinifile", FALSE, State.AppState->Ini);
}

// 0x00401430
VOID AcquireStringValue(CONST U32 indx, LPSTR value, CONST U32 length)
{
    LoadStringA(State.Text.Handle, indx, value, length);
}

// 0x00401870
BOOL StartWindowStateAction()
{
    State.Window.WindowState->Class.hIcon = LoadIconA(State.Window.WindowState->Instance, MAKEINTRESOURCEA(IDI_WARACTION));
    State.Window.WindowState->Class.hCursor = NULL;

    strcpy(State.Window.WindowState->Title, "Starting...");

    return TRUE;
}

// 0x00401600
BOOL InitializeWindowStateAction()
{
    CoInitialize(NULL);
    InitializeWindowStateArguments();

    CHAR file[APP_STATE_CONTAINER_MAX_FILE_NAME_LENGTH];

    if (!AcquireWindowStateArgumentValue("ini", file, APP_STATE_CONTAINER_MAX_FILE_NAME_LENGTH)) { strcpy(file, SETTINGS_FILE_NAME); }

    if (!InitializeApplicationState(file)) { return FALSE; }

    State.AppState->AcquireRendererSettingsValue = AcquireRendererSettingsValue;

    State.AppState->ModuleState = (LPMODULESTATECONTAINER)malloc(sizeof(MODULESTATECONTAINER));
    ZeroMemory(State.AppState->ModuleState, sizeof(MODULESTATECONTAINER));

    State.ModuleState = State.AppState->ModuleState;

    GetPrivateProfileStringA("StartUp", "TextResource", ".", file, APP_STATE_CONTAINER_MAX_FILE_NAME_LENGTH, State.AppState->Ini);

    State.Text.Handle = LoadLibraryA(file);

    if (State.Text.Handle == NULL) { return FALSE; }

    State.ModuleState->TextModule = State.Text.Handle;

    AcquireIniFileState();

    {
        STRINGVALUE configuration;
        AcquireSettingsValue(&configuration, IDS_CHANNELS);

        if (!InitializeSoundState(&SoundState, State.Window.WindowState->HWND,
            AcquireGameSettingsValue(configuration, SOUND_STATE_DEFAULT_CHANNEL_COUNT)))
        {
            CHAR value[WINDOW_STATE_MAX_SETTING_VALUE_LENGTH];

            AcquireStringValue(IDS_REVERSE_STEREO, value, WINDOW_STATE_MAX_SETTING_VALUE_LENGTH);
            AcquireSettingsValue(&configuration, IDS_REVERSE_STEREO, value);

            SoundState.IsReverseStereo = (BOOL)AcquireGameSettingsValue(configuration, FALSE);
        }
    }

    State.ModuleState->SoundState = &SoundState;
    State.ModuleState->TextModule = State.Text.Handle;

    State.ModuleState->Unknown0x5cc = 0; // TODO
    State.ModuleState->Handle = GetModuleHandleA(NULL);
    State.ModuleState->Unknown0x30 = 0; // TODO

    SetWindowTextA(State.Window.WindowState->HWND, State.AppState->Title);

    return TRUE;
}

// 0x00401930
VOID ActivateWindowStateContainer(LPWINDOWSTATECONTAINER self, WINDOWSTATEHANDLERLAMBDA start, WINDOWSTATEHANDLERLAMBDA init, WINDOWSTATEHANDLERLAMBDA action, WINDOWSTATEHANDLERLAMBDA release, WINDOWSTATEHANDLERLAMBDA message)
{
    INITIALIZEWINDOWSTATEHANDLER(WINDOW_STATE_START_HANDLER_INDEX, start);
    INITIALIZEWINDOWSTATEHANDLER(WINDOW_STATE_INITIALIZE_HANDLER_INDEX, init);;
    INITIALIZEWINDOWSTATEHANDLER(WINDOW_STATE_ACTION_HANDLER_INDEX, action);
    INITIALIZEWINDOWSTATEHANDLER(WINDOW_STATE_RELEASE_HANDLER_INDEX, release);
    INITIALIZEWINDOWSTATEHANDLER(WINDOW_STATE_MESSAGE_HANDLER_INDEX, message);

    ActivateWindowStateContainer(self);
}

// 0x004026c0
LRESULT WINAPI WindowStateEventHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY: { PostQuitMessage(EXIT_SUCCESS); break; }
    case WM_SETFOCUS: { State.Window.WindowState->IsActive = TRUE; break; }
    case WM_KILLFOCUS: {State.Window.WindowState->IsActive = FALSE; break; }
    }

    for (State.Window.ActiveHandler = State.Window.Handlers[WINDOW_STATE_MESSAGE_HANDLER_INDEX];
        State.Window.ActiveHandler != NULL; State.Window.ActiveHandler = State.Window.ActiveHandler->Next)
    {
        LRESULT result = 0;
        if (!INVOKEWINDOWSTATEMESSAGEHANDLERLAMBDA(State.Window.ActiveHandler->Invoke, hwnd, msg, wp, lp, &result)) { return result; }

        if (State.Window.ActiveHandler == NULL) { break; }
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

// 0x004023b0
VOID ActivateWindowStateContainer(LPWINDOWSTATECONTAINER self)
{
    self->Instance = NULL;
    self->Args = NULL;
    self->HWND = NULL;

    State.Window.WindowState = self;

    self->Class.style = CS_NONE;
    self->Class.lpfnWndProc = WindowStateEventHandler;
    self->Class.cbClsExtra = 0;
    self->Class.cbWndExtra = 0;
    self->Class.hInstance = NULL;
    self->Class.hIcon = NULL;
    self->Class.hCursor = NULL;
    self->Class.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    self->Class.lpszMenuName = NULL;
    self->Class.lpszClassName = WINDOW_STATE_DEFAULT_WINDOW_CLASS_NAME;

    strcpy(self->Title, WINDOW_STATE_DEFAULT_WINDOW_TITLE_NAME);

    self->Style = WS_POPUP;

    self->Y = 0;
    self->X = 0;
    self->Height = WINDOW_STATE_DEFAULT_WINDOW_HEIGHT_VALUE;
    self->Width = WINDOW_STATE_DEFAULT_WINDOW_WIDTH_VALUE;

    self->Menu = NULL;

    self->IsActive = TRUE;
}

// 0x00402850
VOID InitializeWindowStateArguments()
{
    U32 count = 0;
    U32 length = 0;

    SplitWindowStateArguments(State.Window.WindowState->Args, NULL, NULL, &count, &length);

    State.Arguments.All = (CHAR*)malloc(length);
    State.Arguments.Args = (CHAR**)malloc(count * sizeof(CHAR*));
    
    SplitWindowStateArguments(State.Window.WindowState->Args, State.Arguments.Args, State.Arguments.All, &count, &length);
    
    State.Arguments.Count = count;
}

// 0x004028d0
VOID SplitWindowStateArguments(LPCSTR value, CHAR** args, CHAR* values, U32* count, U32* length)
{
    BOOL start = FALSE;
    BOOL end = FALSE;

    *length = 0;
    *count = 0;

    if (value == NULL) { return; }

    while (*value != NULL)
    {
        // Skip spaces and tabs.
        for (; *value == ' ' || *value == '\t'; value = value + 1) {}

        if (*value == NULL) { break; }

        if (args != NULL)
        {
            *args = values;
            args = args + 1;
        }

        *count = *count + 1;

        while (TRUE)
        {
            U32 slashes = 0;
            BOOL append = TRUE;
            CHAR current = *value;

            // Count contiguous slashes.
            while (current == '\\')
            {
                value = value + 1;
                slashes = slashes + 1;
                current = *value;
            }

            if (*value == '\"')
            {
                if ((slashes & 1) == 0)
                {
                    if (start && value[1] == '\"')
                    {
                        value = value + 1;
                    }
                    else { append = FALSE; }

                    start = !end;
                    end = start;
                }

                slashes = slashes >> 1;
            }

            for (; slashes != 0; slashes = slashes - 1)
            {
                if (values != NULL)
                {
                    *values = '\\';
                    values = values + 1;
                }

                *length = *length + 1;
            }

            current = *value;

            if (current == NULL || (!start && (current == ' ' || current == '\t'))) { break; }

            if (append)
            {
                if (values != NULL)
                {
                    *values = current;
                    values = values + 1;
                }

                *length = *length + 1;
            }

            value = value + 1;
        }

        if (values != NULL)
        {
            *values = NULL;
            values = values + 1;
        }

        *length = *length + 1;
    }
}

// 0x004029c0
BOOL AcquireWindowStateArgumentValue(LPCSTR name, CHAR* value, CONST U32 length)
{
    for (U32 x = 0; x < State.Arguments.Count; x++)
    {
        CHAR* value = State.Arguments.Args[x];
        CHAR* current = strchr(value, '=');

        if (current != NULL)
        {
            if (strnicmp(value, name, (size_t)current - (size_t)value) == 0)
            {
                current++;

                strncpy(value, current, Min(length, strlen(current)));

                return TRUE;
            }
        }
    }

    return FALSE;
}

// 0x004017e0
BOOL ReleaseWindowStateAction(VOID)
{
    ReleaseApplicationStateModules();

    if (State.AppState != NULL && State.AppState->ModuleState != NULL)
    {
        if (State.ModuleState->Unknown0x5cc != NULL)
        {
            State.ModuleState->Unknown0x5cc->Release(); // TODO
        }

        free(State.AppState->ModuleState);
    }

    ReleaseApplicationState();

    if (State.Renderer.State != NULL)
    {
        State.Renderer.State->Actions.RestoreDisplayMode();

        ReleaseRenderStateModule();
    }

    ReleaseSoundState(&SoundState);

    if (State.Text.Handle != NULL) { FreeLibrary(State.Text.Handle); }

    CoUninitialize();

    return TRUE;
}

// 0x004018c0
 BOOL MessageWindowStateAction(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result)
{
    if (msg != WM_CLOSE)
    {
        if (msg != WM_SETCURSOR) { return TRUE; }

        SetCursor(NULL);

        *result = 1;
    }

    return FALSE;
}

 // 0x004018f0
 VOID ActivateWindowStateContainer()
 {
     ActivateWindowStateContainer(&WindowState, StartWindowStateAction,
         InitializeWindowStateAction, NULL, ReleaseWindowStateAction, (WINDOWSTATEHANDLERLAMBDA)MessageWindowStateAction);
 }