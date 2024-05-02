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
    if (State.App->InitModule == APP_STATE_CONTAINER_INVALID_MODULE_INDEX)
    {
        if (State.App->ActiveModule == APP_STATE_CONTAINER_INVALID_MODULE_INDEX) { return FALSE; }

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
    State.App = (LPAPPSTATECONTAINER)malloc(sizeof(APPSTATECONTAINER));

    ZeroMemory(State.App, sizeof(APPSTATECONTAINER));

    State.App->Logger = State.Logger;

    State.App->Window = State.Window;
    State.App->AcquireRendererSettingsValue = NULL;

    State.App->Unknown0x1170 = NULL;

    if (!InitializeApplicationStateModules(file)) { Message("Can't load profile %s.\n", file); return FALSE; }

    InitializeActionHandler(ACTION_HANDLER_DEFAULT_PRIORITY, ApplicationStateWindowStateActionHandler);
    InitializeWindowActionHandler(ACTION_HANDLER_DEFAULT_PRIORITY, ApplicationStateWindowStateMessageHandler);

    return TRUE;
}

// 0x00401ae0
BOOL InitializeApplicationStateModules(LPCSTR file)
{
    strcpy(State.App->Ini, file);

    GetPrivateProfileStringA("StartUp", "ProcessName", SETTINGS_INVALID_STRING_VALUE, State.App->Title, APP_STATE_CONTAINER_MAX_WINDOW_TITLE_LENGTH, file);

    if (strncmp(State.App->Title, SETTINGS_INVALID_STRING_VALUE, SETTINGS_MAX_INVALID_STRING_VALUE_LENGTH) == 0) { return FALSE; }

    for (U32 x = 0; x < APP_STATE_CONTAINER_MAX_MODULE_COUNT; x++)
    {
        CHAR name[MAX_MODULE_TEMPLATE_LENGTH];

        wsprintfA(name, "Module%i", x + 1);

        GetPrivateProfileStringA("StartUp", name, NULL, State.App->Modules[x], APP_STATE_CONTAINER_MAX_MODULE_NAME_LENGTH, file);
    }

    State.App->InitModule = GetPrivateProfileIntA("StartUp", "StartModule", APP_STATE_CONTAINER_INVALID_MODULE_INDEX, file);

    return TRUE;
}

// 0x00401bd0
VOID ReleaseApplicationStateModules()
{
    if (State.App == NULL || State.App->ActiveModule == APP_STATE_CONTAINER_INVALID_MODULE_INDEX) { return; }

    if (State.App->Actions.Done != NULL) { State.App->Actions.Done(); }

    if (State.App->ModuleHandle != NULL)
    {
        if (State.App->Unknown0x1170 != NULL) { State.App->Unknown0x1170(State.App); }

        FreeLibrary(State.App->ModuleHandle);

        State.App->ModuleHandle = NULL;
    }

    State.App->Actions.Initialize = NULL;
    State.App->Actions.Execute = NULL;
    State.App->Actions.Done = NULL;
    State.App->Actions.Handle = NULL;

    State.App->ActiveModule = APP_STATE_CONTAINER_INVALID_MODULE_INDEX;
}

// 0x00401ee0
BOOL ReleaseApplicationState()
{
    ReleaseApplicationStateModules();
    
    free(State.App);

    State.App = NULL;

    ReleaseActionHandler(ApplicationStateWindowStateActionHandler);
    ReleaseWindowActionHandler(ApplicationStateWindowStateMessageHandler);

    return TRUE;
}

// 0x00401c70
BOOL InitializeApplicationStateModule()
{
    if (State.App->InitModule < APP_STATE_CONTAINER_MIN_VALID_MODULE_INDEX)
    {
        ReleaseApplicationStateModules();

        State.App->InitModule = APP_STATE_CONTAINER_INVALID_MODULE_INDEX;
    }
    else
    {
        ReleaseApplicationStateModules();

        if (State.App->AcquireRendererSettingsValue != NULL
            && !State.App->AcquireRendererSettingsValue()) { return FALSE; }

        State.App->ModuleHandle = LoadLibraryA(State.App->Modules[State.App->InitModule - 1]);

        if (State.App->ModuleHandle == NULL)
        {
            Message("Can't load %s: %i\n", State.App->Modules[State.App->InitModule - 1], GetLastError()); return FALSE;
        }

        State.App->ActiveModule = State.App->InitModule;
        State.App->InitModule = APP_STATE_CONTAINER_INVALID_MODULE_INDEX;

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
            ReleaseApplicationStateModules(); return FALSE;
        }
    }

    return TRUE;
}