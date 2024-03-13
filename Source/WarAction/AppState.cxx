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

#include <VisualModuleExport.hxx>

#define MAX_MODULE_TEMPLATE_LENGTH 16

// 0x00401f20
BOOL ApplicationStateWindowStateActionHandler(VOID)
{
    if (State.AppState->InitModule == APP_STATE_CONTAINER_INVALID_MODULE_INDEX)
    {
        if (State.AppState->ActiveModule == APP_STATE_CONTAINER_INVALID_MODULE_INDEX) { return FALSE; }

        if (!State.AppState->ExecuteAction()) { return FALSE; }
    }
    else if (!InitializeApplicationStateModule()) { return FALSE; }

    return TRUE;
}

// 0x00401f60
BOOL ApplicationStateWindowStateMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result)
{
    if (State.AppState != NULL && State.AppState->HandleAction != NULL)
    {
        if (!State.AppState->HandleAction(hwnd, msg, wp, lp, result)) { return FALSE; }
    }

    return TRUE;
}

// 0x00401e30
BOOL InitializeApplicationState(LPCSTR file)
{
    State.AppState = (LPAPPSTATECONTAINER)malloc(sizeof(APPSTATECONTAINER));

    ZeroMemory(State.AppState, sizeof(APPSTATECONTAINER));

    State.AppState->Logger = State.Logger;

    State.AppState->WindowState = State.Window.WindowState;
    State.AppState->AcquireRendererSettingsValue = NULL;

    State.AppState->Unknown0x1170 = NULL;

    if (!InitializeApplicationStateModules(file)) { Message("Can't load profile %s.\n", file); return FALSE; }

    InitializeWindowStateActionHandler(WINDOW_STATE_HANDLER_DEFAULT_PRIORITY_VALUE, ApplicationStateWindowStateActionHandler);
    InitializeWindowStateMessageHandler(WINDOW_STATE_HANDLER_DEFAULT_PRIORITY_VALUE, ApplicationStateWindowStateMessageHandler);

    return TRUE;
}

// 0x00401ae0
BOOL InitializeApplicationStateModules(LPCSTR file)
{
    strcpy(State.AppState->Ini, file);

    GetPrivateProfileStringA("StartUp", "ProcessName", SETTINGS_INVALID_STRING_VALUE, State.AppState->Title, APP_STATE_CONTAINER_MAX_WINDOW_TITLE_LENGTH, file);

    if (strncmp(State.AppState->Title, SETTINGS_INVALID_STRING_VALUE, SETTINGS_MAX_INVALID_STRING_VALUE_LENGTH) == 0) { return FALSE; }

    for (U32 x = 0; x < APP_STATE_CONTAINER_MAX_MODULE_COUNT; x++)
    {
        CHAR name[MAX_MODULE_TEMPLATE_LENGTH];

        wsprintfA(name, "Module%i", x + 1);

        GetPrivateProfileStringA("StartUp", name, NULL, State.AppState->Modules[x], APP_STATE_CONTAINER_MAX_MODULE_NAME_LENGTH, file);
    }

    State.AppState->InitModule = GetPrivateProfileIntA("StartUp", "StartModule", APP_STATE_CONTAINER_INVALID_MODULE_INDEX, file);

    return TRUE;
}

// 0x00401bd0
VOID ReleaseApplicationStateModules()
{
    if (State.AppState == NULL || State.AppState->ActiveModule == APP_STATE_CONTAINER_INVALID_MODULE_INDEX) { return; }

    if (State.AppState->DoneAction != NULL) { State.AppState->DoneAction(); }

    if (State.AppState->ModuleHandle != NULL)
    {
        if (State.AppState->Unknown0x1170 != NULL) { State.AppState->Unknown0x1170(State.AppState); }

        FreeLibrary(State.AppState->ModuleHandle);

        State.AppState->ModuleHandle = NULL;
    }

    State.AppState->InitAction = NULL;
    State.AppState->ExecuteAction = NULL;
    State.AppState->DoneAction = NULL;
    State.AppState->HandleAction = NULL;

    State.AppState->ActiveModule = APP_STATE_CONTAINER_INVALID_MODULE_INDEX;
}

// 0x00401ee0
BOOL ReleaseApplicationState()
{
    ReleaseApplicationStateModules();
    
    free(State.AppState);

    State.AppState = NULL;

    ReleaseWindowStateActionHandler(ApplicationStateWindowStateActionHandler);
    ReleaseWindowStateMessageHandler(ApplicationStateWindowStateMessageHandler);

    return TRUE;
}

// 0x00401c70
BOOL InitializeApplicationStateModule()
{
    if (State.AppState->InitModule < APP_STATE_CONTAINER_MIN_VALID_MODULE_INDEX)
    {
        ReleaseApplicationStateModules();

        State.AppState->InitModule = APP_STATE_CONTAINER_INVALID_MODULE_INDEX;
    }
    else
    {
        ReleaseApplicationStateModules();

        if (State.AppState->AcquireRendererSettingsValue != NULL
            && !State.AppState->AcquireRendererSettingsValue()) { return FALSE; }

        State.AppState->ModuleHandle = LoadLibraryA(State.AppState->Modules[State.AppState->InitModule - 1]);

        if (State.AppState->ModuleHandle == NULL)
        {
            Message("Can't load %s: %i\n", State.AppState->Modules[State.AppState->InitModule - 1], GetLastError()); return FALSE;
        }

        State.AppState->ActiveModule = State.AppState->InitModule;
        State.AppState->InitModule = APP_STATE_CONTAINER_INVALID_MODULE_INDEX;

        State.AppState->InitAction = (VISUALMODULEINITACTIONLAMBDA)GetProcAddress(State.AppState->ModuleHandle, VISUAL_MODULE_INIT_NAME);

        if (State.AppState->InitAction == NULL)
        {
            Message("Can't retrive VModule_Init function address.\n");

            ReleaseApplicationStateModules();

            return FALSE;
        }

        State.AppState->ExecuteAction = (VISUALMODULEEXECUTEACTIONLAMBDA)GetProcAddress(State.AppState->ModuleHandle, VISUAL_MODULE_PLAY_NAME);

        if (State.AppState->ExecuteAction == NULL)
        {
            Message("Can't retrive VModule_Play function address.\n");

            ReleaseApplicationStateModules();

            return FALSE;
        }

        State.AppState->DoneAction = (VISUALMODULEDONEACTIONLAMBDA)GetProcAddress(State.AppState->ModuleHandle, VISUAL_MODULE_DONE_NAME);

        if (State.AppState->DoneAction == NULL)
        {
            Message("Can't retrive VModule_Done function address.\n");

            ReleaseApplicationStateModules();

            return FALSE;
        }

        State.AppState->HandleAction = (VISUALMODULEHANDLEACTIONLAMBDA)GetProcAddress(State.AppState->ModuleHandle, VISUAL_MODULE_HANDLE_NAME);

        if (State.AppState->HandleAction == NULL)
        {
            Message("Can't retrive VModule_Handle function address.\n");

            ReleaseApplicationStateModules();

            return FALSE;
        }

        if (!State.AppState->InitAction(State.AppState))
        {
            ReleaseApplicationStateModules(); return FALSE;
        }
    }

    return TRUE;
}