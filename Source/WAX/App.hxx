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

#include "Logger.hxx"
#include "Module.hxx"
#include "RendererModule.Export.hxx"
#include "VisualModule.Export.hxx"
#include "Window.hxx"

#define INVALID_MODULE_STATE_INDEX          (-1)
#define LAUNCHER_MODULE_STATE_INDEX         0
#define MENU_MODULE_STATE_INDEX             1
#define GAME_MODULE_STATE_INDEX             2

#define MAX_APP_STATE_WINDOW_TITLE_LENGTH   64

#define MAX_MODULE_COUNT                    16

typedef struct App
{
    S32                                 InitModule;
    LPCSTR                              ModuleName;
    HMODULE                             ModuleHandle;
    CHAR                                Ini[MAX_FILE_NAME_LENGTH];
    CHAR                                Title[MAX_APP_STATE_WINDOW_TITLE_LENGTH];
    CHAR                                Modules[MAX_MODULE_COUNT][MAX_FILE_NAME_LENGTH];
    WINDOWPTR                           Window;
    MODULEPTR                           Module;
    S32                                 Unknown0x1154; // TODO
    LOGGERPTR                           Logger;
    S32                                 ActiveModule;
    struct
    {
        VISUALMODULEINITACTIONLAMBDA    Initialize;
        VISUALMODULEEXECUTEACTIONLAMBDA Execute;
        VISUALMODULEDONEACTIONLAMBDA    Done;
        VISUALMODULEHANDLEACTIONLAMBDA  Handle;
        VISUALMODULERELEASEACTIONLAMBDA Release;
    } Actions;
    ACQUIRERENDERERSETTINGSVALUELAMBDA  AcquireRendererSettingsValue;
} APP, * APPPTR;