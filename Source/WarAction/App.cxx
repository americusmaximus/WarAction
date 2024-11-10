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

#include "Message.hxx"
#include "Settings.hxx"
#include "State.hxx"

#include <VisualModule.Export.hxx>
#include <VisualModule.Import.hxx>

#define MAX_MODULE_TEMPLATE_LENGTH 16

// 0x00401f20
BOOL ApplicationStateWindowStateActionHandler(VOID)
{
    if (State.App->InitModule == LAUNCHER_MODULE_STATE_INDEX)
    {
        if (State.App->ActiveModule == LAUNCHER_MODULE_STATE_INDEX) { return FALSE; }

        if (!State.App->Actions.Execute()) { return FALSE; }
    }
    else if (!InitializeApplicationStateModule()) { return FALSE; }

    return TRUE;
}

// 0x00401f60
BOOL ApplicationStateWindowStateMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result)
{
    if (State.App != NULL && State.App->Actions.Handle != NULL)
    {
        if (!State.App->Actions.Handle(hwnd, msg, wp, lp, result)) { return FALSE; }
    }

    return TRUE;
}

// 0x00401e30
BOOL InitializeApplicationState(LPCSTR file)
{
    State.App = (APPPTR)malloc(sizeof(APP));

    ZeroMemory(State.App, sizeof(APP));

    State.App->Logger = State.Logger;

    State.App->Window = State.Window;
    State.App->AcquireRendererSettingsValue = NULL;

    State.App->Actions.Release = NULL;

    if (!InitializeApplicationStateModules(file)) { Message("Can't load profile %s.\n", file); return FALSE; }

    InitializeActionHandler(DEFAULT_ACTION_HANDLER_PRIORITY, ApplicationStateWindowStateActionHandler);
    InitializeWindowActionHandler(DEFAULT_ACTION_HANDLER_PRIORITY, ApplicationStateWindowStateMessageHandler);

    return TRUE;
}

// 0x00401ae0
BOOL InitializeApplicationStateModules(LPCSTR file)
{
    strcpy(State.App->Ini, file);

    GetPrivateProfileStringA("StartUp", "ProcessName", INVALID_SETTINGS_STRING_VALUE, State.App->Title, MAX_APP_STATE_WINDOW_TITLE_LENGTH, file);

    if (strncmp(State.App->Title, INVALID_SETTINGS_STRING_VALUE, MAX_INVALID_SETTINGS_STRING_VALUE_LENGTH) == 0) { return FALSE; }

    for (U32 x = 0; x < MAX_MODULE_COUNT; x++)
    {
        CHAR name[MAX_MODULE_TEMPLATE_LENGTH];

        wsprintfA(name, "Module%i", x + 1);

        GetPrivateProfileStringA("StartUp", name, NULL, State.App->Modules[x], MAX_FILE_NAME_LENGTH, file);
    }

    State.App->InitModule = GetPrivateProfileIntA("StartUp", "StartModule", LAUNCHER_MODULE_STATE_INDEX, file);

    return TRUE;
}

// 0x00401bd0
VOID ReleaseApplicationStateModules(VOID)
{
    if (State.App == NULL || State.App->ActiveModule == LAUNCHER_MODULE_STATE_INDEX) { return; }

    if (State.App->Actions.Done != NULL) { State.App->Actions.Done(); }

    if (State.App->ModuleHandle != NULL)
    {
        if (State.App->Actions.Release != NULL) { State.App->Actions.Release(State.App); }

        FreeLibrary(State.App->ModuleHandle);

        State.App->ModuleHandle = NULL;
    }

    State.App->Actions.Initialize = NULL;
    State.App->Actions.Execute = NULL;
    State.App->Actions.Done = NULL;
    State.App->Actions.Handle = NULL;

    State.App->ActiveModule = LAUNCHER_MODULE_STATE_INDEX;
}

// 0x00401ee0
BOOL ReleaseApplicationState(VOID)
{
    ReleaseApplicationStateModules();

    free(State.App);

    State.App = NULL;

    ReleaseActionHandler(ApplicationStateWindowStateActionHandler);
    ReleaseWindowActionHandler(ApplicationStateWindowStateMessageHandler);

    return TRUE;
}

// 0x00401c70
BOOL InitializeApplicationStateModule(VOID)
{
    if (State.App->InitModule < MENU_MODULE_STATE_INDEX)
    {
        ReleaseApplicationStateModules();

        State.App->InitModule = LAUNCHER_MODULE_STATE_INDEX;

        return TRUE;
    }

    ReleaseApplicationStateModules();

    if (State.App->AcquireRendererSettingsValue != NULL
        && !State.App->AcquireRendererSettingsValue()) { return FALSE; }

    State.App->ModuleHandle = LoadLibraryA(State.App->Modules[State.App->InitModule - 1]);

    if (State.App->ModuleHandle == NULL)
    {
        Message("Can't load %s: %i\n", State.App->Modules[State.App->InitModule - 1], GetLastError());

        return FALSE;
    }

    State.App->ActiveModule = State.App->InitModule;
    State.App->InitModule = LAUNCHER_MODULE_STATE_INDEX;

    State.App->Actions.Initialize = (VISUALMODULEINITACTIONLAMBDA)GetProcAddress(State.App->ModuleHandle, VISUAL_MODULE_INIT_NAME);

    if (State.App->Actions.Initialize == NULL)
    {
        Message("Can't retrive VModule_Init function address.\n");

        ReleaseApplicationStateModules();

        return FALSE;
    }

    State.App->Actions.Execute = (VISUALMODULEEXECUTEACTIONLAMBDA)GetProcAddress(State.App->ModuleHandle, VISUAL_MODULE_PLAY_NAME);

    if (State.App->Actions.Execute == NULL)
    {
        Message("Can't retrive VModule_Play function address.\n");

        ReleaseApplicationStateModules();

        return FALSE;
    }

    State.App->Actions.Done = (VISUALMODULEDONEACTIONLAMBDA)GetProcAddress(State.App->ModuleHandle, VISUAL_MODULE_DONE_NAME);

    if (State.App->Actions.Done == NULL)
    {
        Message("Can't retrive VModule_Done function address.\n");

        ReleaseApplicationStateModules();

        return FALSE;
    }

    State.App->Actions.Handle = (VISUALMODULEHANDLEACTIONLAMBDA)GetProcAddress(State.App->ModuleHandle, VISUAL_MODULE_HANDLE_NAME);

    if (State.App->Actions.Handle == NULL)
    {
        Message("Can't retrive VModule_Handle function address.\n");

        ReleaseApplicationStateModules();

        return FALSE;
    }

    if (!State.App->Actions.Initialize(State.App))
    {
        ReleaseApplicationStateModules();

        return FALSE;
    }

    return TRUE;
}
