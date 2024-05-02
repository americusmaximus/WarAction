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

#include "LoggerState.hxx"
#include "ModuleState.hxx"
#include "RendererModule.Export.hxx"
#include "VisualModule.Export.hxx"
#include "WindowState.hxx"

#define APP_STATE_CONTAINER_INVALID_MODULE_INDEX 0
#define APP_STATE_CONTAINER_MIN_VALID_MODULE_INDEX 1

#define APP_STATE_CONTAINER_MAX_FILE_NAME_LENGTH 256
#define APP_STATE_CONTAINER_MAX_WINDOW_TITLE_LENGTH 64

#define APP_STATE_CONTAINER_MAX_MODULE_COUNT 16
#define APP_STATE_CONTAINER_MAX_MODULE_NAME_LENGTH 256

struct AppStateContainer;

typedef VOID(*UNKNOWN0X1170)(AppStateContainer*); // TODO

typedef struct AppStateContainer
{
    S32 InitModule;
    LPSTR ModuleName;
    HMODULE ModuleHandle;
    CHAR Ini[APP_STATE_CONTAINER_MAX_FILE_NAME_LENGTH];
    CHAR Title[APP_STATE_CONTAINER_MAX_WINDOW_TITLE_LENGTH];
    CHAR Modules[APP_STATE_CONTAINER_MAX_MODULE_COUNT][APP_STATE_CONTAINER_MAX_MODULE_NAME_LENGTH];
    LPWINDOWSTATECONTAINER Window;
    LPMODULESTATECONTAINER Module;
    S32 Unknown0x1154; // TODO
    LPLOGGERSTATECONTAINER Logger;
    S32 ActiveModule;
    struct
    {
        VISUALMODULEINITACTIONLAMBDA Initialize;
        VISUALMODULEEXECUTEACTIONLAMBDA Execute;
        VISUALMODULEDONEACTIONLAMBDA Done;
        VISUALMODULEHANDLEACTIONLAMBDA Handle;
    } Actions;
    UNKNOWN0X1170 Unknown0x1170; // TODO
    ACQUIRERENDERERSETTINGSVALUELAMBDA AcquireRendererSettingsValue;
} APPSTATECONTAINER, * LPAPPSTATECONTAINER;