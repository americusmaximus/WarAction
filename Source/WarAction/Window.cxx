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

#include "Module.hxx"
#include "Renderer.hxx"
#include "Settings.hxx"
#include "State.hxx"
#include "WarAction.hxx"

#include <..\Text\Resources.hxx>

#define WINDOW_CLASS_STYLE_NONE                 0

#define MAX_WINDOW_STATE_SETTING_VALUE_LENGTH   256

// ORIGINAL: VITYACLASS
#define DEFAULT_WINDOW_STATE_CLASS_NAME         "WarAction"

// ORIGINAL: Just a simple VITYA window :-)
#define DEFAULT_WINDOW_STATE_TITLE_NAME         "War Action"

#define DEFAULT_WINDOW_STATE_WIDTH              200
#define DEFAULT_WINDOW_STATE_HEIGHT             200

// 0x00410388
WINDOW WindowState;

// 0x00401870
BOOL ActivateWindowStateAction(VOID)
{
    State.Window->Class.hIcon = LoadIconA(State.Window->Instance, MAKEINTRESOURCEA(IDI_WARACTION));
    State.Window->Class.hCursor = NULL;

    strcpy(State.Window->Title, "Starting...");

    return TRUE;
}

// 0x00401600
BOOL InitializeWindowStateAction(VOID)
{
    CoInitialize(NULL);
    AcquireStartArguments();

    CHAR file[MAX_FILE_NAME_LENGTH];

    if (!AcquireStartArguments("ini", file, MAX_FILE_NAME_LENGTH)) { strcpy(file, SETTINGS_FILE_NAME); }

    if (!InitializeApplicationState(file)) { return FALSE; }

    State.App->AcquireRendererSettingsValue = AcquireRendererSettingsValue;

    State.App->Module = (MODULEPTR)malloc(sizeof(MODULE));
    ZeroMemory(State.App->Module, sizeof(MODULE));

    State.Module = State.App->Module;

    GetPrivateProfileStringA("StartUp", "TextResource", ".", file, MAX_FILE_NAME_LENGTH, State.App->Ini);

    State.Text.Handle = LoadLibraryA(file);

    if (State.Text.Handle == NULL) { return FALSE; }

    State.Module->Text = State.Text.Handle;

    AcquireIniFileState();

    {
        STRINGVALUE configuration;
        AcquireSettingsValue(&configuration, IDS_CHANNELS);

        if (!InitializeSoundState(&SoundState, State.Window->HWND,
            AcquireSettingsValue(configuration, MAX_SOUND_STATE_CHANNEL_COUNT)))
        {
            CHAR value[MAX_WINDOW_STATE_SETTING_VALUE_LENGTH];

            AcquireSettingsValue(IDS_REVERSE_STEREO, value, MAX_WINDOW_STATE_SETTING_VALUE_LENGTH);
            AcquireSettingsValue(&configuration, IDS_REVERSE_STEREO, value);

            SoundState.IsReverseStereo = (BOOL)AcquireSettingsValue(configuration, FALSE);
        }
    }

    State.Module->Sound = &SoundState;
    State.Module->Text = State.Text.Handle;

    State.Module->Network = NULL;
    State.Module->Handle = GetModuleHandleA(NULL);
    State.Module->Game.Status = STATUS_NONE;

    SetWindowTextA(State.Window->HWND, State.App->Title);

    return TRUE;
}

// 0x00401930
VOID CLASSCALL ActivateWindowStateContainer(WINDOWPTR self, ACTIONHANDLERLAMBDA activate, ACTIONHANDLERLAMBDA initialize, ACTIONHANDLERLAMBDA action, ACTIONHANDLERLAMBDA release, ACTIONHANDLERLAMBDA message)
{
    INITIALIZEACTIONHANDLER(&State.Actions.Activate, activate);
    INITIALIZEACTIONHANDLER(&State.Actions.Initialize, initialize);
    INITIALIZEACTIONHANDLER(&State.Actions.Action, action);
    INITIALIZEACTIONHANDLER(&State.Actions.Release, release);
    INITIALIZEACTIONHANDLER(&State.Actions.Message, message);

    ActivateWindowStateContainer(self);
}

// 0x004026c0
LRESULT WINAPI WindowStateMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY: { PostQuitMessage(EXIT_SUCCESS); break; }
    case WM_SETFOCUS: { State.Window->IsActive = TRUE; break; }
    case WM_KILLFOCUS: {State.Window->IsActive = FALSE; break; }
    }

    for (State.Actions.Active = State.Actions.Message;
        State.Actions.Active != NULL; State.Actions.Active = State.Actions.Active->Next)
    {
        LRESULT result = 0;
        if (!INVOKEWINDOWACTIONHANDLERLAMBDA(State.Actions.Active->Action, hwnd, msg, wp, lp, &result)) { return result; }

        if (State.Actions.Active->Next == NULL) { break; }
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

// 0x004023b0
VOID CLASSCALL ActivateWindowStateContainer(WINDOWPTR self)
{
    self->Instance = NULL;
    self->Args = NULL;
    self->HWND = NULL;

    State.Window = self;

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

    self->X = 0;
    self->Y = 0;
    self->Width = DEFAULT_WINDOW_STATE_WIDTH;
    self->Height = DEFAULT_WINDOW_STATE_HEIGHT;

    self->Menu = NULL;

    self->IsActive = TRUE;
}

// 0x004017e0
BOOL ReleaseWindowStateAction(VOID)
{
    ReleaseApplicationStateModules();

    if (State.App != NULL && State.App->Module != NULL)
    {
        DIRECTPLAYRELEASE(State.Module->Network);

        free(State.App->Module);
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

        *result = TRUE;
    }

    return FALSE;
}

// 0x004018f0
VOID ActivateWindowState(VOID)
{
    ActivateWindowStateContainer(&WindowState, ActivateWindowStateAction,
        InitializeWindowStateAction, NULL, ReleaseWindowStateAction, (ACTIONHANDLERLAMBDA)MessageWindowStateAction);
}