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
#include "Strings.hxx"

#include <..\Text\Resources.hxx>

#include <stdio.h>

#define MAX_SETTINGS_VALUE_LENGTH 64
#define MAX_SETTINGS_DATA_VALUE_LENGTH 256
#define MAX_SETTINGS_OUTPUT_VALUE_LENGTH 512

// 0x00401060
VOID AcquireSettingsValue(LPSTRINGVALUE result, CONST U32 indx, ...)
{
    CHAR setting[MAX_SETTINGS_VALUE_LENGTH];
    LoadStringA(State.ModuleState->TextModule, indx, setting, MAX_SETTINGS_VALUE_LENGTH);

    CHAR output[MAX_SETTINGS_OUTPUT_VALUE_LENGTH];

    va_list args;
    va_start(args, indx);
    vsprintf(output, setting, args);
    va_end(args);

    result->Value = (CHAR*)malloc(strlen(output) + 1);

    strcpy(result->Value, output);
}

// 0x00401160
VOID AcquireActualSettingsValue(LPSTRINGVALUE result, STRINGVALUE name, STRINGVALUE value)
{
    CHAR setting[MAX_SETTINGS_VALUE_LENGTH];
    LoadStringA(State.ModuleState->TextModule, IDS_GAME, setting, MAX_SETTINGS_VALUE_LENGTH);

    if (State.IsIniActive)
    {
        CHAR output[MAX_SETTINGS_DATA_VALUE_LENGTH];
        GetPrivateProfileStringA(setting, name.Value, value.Value, output, MAX_SETTINGS_DATA_VALUE_LENGTH, State.AppState->Ini);
        AcquireStringValue(result, output);
    }
    else
    {
        LoadStringA(State.ModuleState->TextModule, IDS_REGISTRY_KEY, setting, MAX_SETTINGS_VALUE_LENGTH);

        HKEY key = NULL;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, setting, REG_OPTION_NON_VOLATILE, KEY_READ, &key) == ERROR_SUCCESS)
        {
            DWORD type = REG_NONE;
            CHAR output[MAX_SETTINGS_DATA_VALUE_LENGTH];
            DWORD length = MAX_SETTINGS_DATA_VALUE_LENGTH;

            if (RegQueryValueExA(key, name.Value, NULL, &type, (BYTE*)output, &length) == ERROR_SUCCESS)
            {
                RegCloseKey(key);

                AcquireStringValue(result, output);

                ReleaseStringValue(&name);
                ReleaseStringValue(&value);

                return;
            }
        }

        RegCloseKey(key);

        AcquireStringCopy(result, &value);
    }

    ReleaseStringValue(&name);
    ReleaseStringValue(&value);
}

// 0x004012a0
S32 AcquireGameSettingsValue(STRINGVALUE name, S32 value)
{
    CHAR input[MAX_SETTINGS_VALUE_LENGTH];
    LoadStringA(State.ModuleState->TextModule, IDS_GAME, input, MAX_SETTINGS_VALUE_LENGTH);

    if (State.IsIniActive)
    {
        CONST S32 result = GetPrivateProfileIntA(input, name.Value, value, State.AppState->Ini);

        ReleaseStringValue(&name);

        return result;
    }

    STRINGVALUE val;
    AcquireStringValue(&val, SETTINGS_INVALID_STRING_VALUE);

    STRINGVALUE nm;
    AcquireStringCopy(&nm, &name);

    STRINGVALUE actual;
    AcquireActualSettingsValue(&actual, nm, val);

    CHAR output[MAX_SETTINGS_OUTPUT_VALUE_LENGTH];
    strcpy(output, actual.Value);

    ReleaseStringValue(&actual);

    if (strncmp(output, SETTINGS_INVALID_STRING_VALUE, SETTINGS_MAX_INVALID_STRING_VALUE_LENGTH) == 0)
    {
        ReleaseStringValue(&name);

        return value;
    }

    CONST S32 result = atoi(output);

    ReleaseStringValue(&name);

    return result;
}

// 0x004015f0
// 0x00401450
BOOL AcquireRendererSettingsValue()
{
    STRINGVALUE setting;
    AcquireSettingsValue(&setting, IDS_VIDEO_MODE);

    CONST S32 mode = AcquireGameSettingsValue(setting, INVALID_RENDERER_VIDEO_MODE_VALUE);

    if (mode == INVALID_RENDERER_VIDEO_MODE_VALUE) { return FALSE; }

    if (mode != RendererVideoMode)
    {
        RendererVideoMode = mode;

        if (State.Renderer.State != NULL) { State.Renderer.State->Actions.ReleaseDirectX(); }

        ReleaseRenderStateModule();

        State.Renderer.State = NULL;
        State.ModuleState->RendererState = NULL;

        // Draw Mode
        {
            STRINGVALUE value;
            AcquireStringValue(&value, "undefined");

            STRINGVALUE configuration;
            AcquireSettingsValue(&configuration, IDS_DRAW_MODE_INDEX, RendererVideoMode);

            STRINGVALUE actual;
            AcquireActualSettingsValue(&actual, configuration, value);

            CHAR output[MAX_SETTINGS_OUTPUT_VALUE_LENGTH];
            strcpy(output, actual.Value);

            ReleaseStringValue(&actual);

            if (!InitializeRendererStateModule(output)) { return FALSE; }
        }

        // Full Screen
        {
            STRINGVALUE configuration;
            AcquireSettingsValue(&configuration, IDS_FULL_SCREEN);

            CONST BOOL fullscreen = (BOOL)AcquireGameSettingsValue(configuration, TRUE);

            if (!State.Renderer.State->Actions.InitializeDirectX(State.Window->HWND, fullscreen)) { return FALSE; }
        }

        State.ModuleState->RendererState = State.Renderer.State;
    }

    return TRUE;
}