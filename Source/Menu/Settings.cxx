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

#include "Settings.hxx"
#include "State.hxx"

#include <..\Text\Resources.hxx>

#include <stdio.h>

#define MAX_SETTINGS_VALUE_LENGTH           64
#define MAX_SETTINGS_DATA_VALUE_LENGTH      256
#define MAX_SETTINGS_OUTPUT_VALUE_LENGTH    512

// 0x10014af0
VOID CLASSCALL SaveSettingsValue(SETTINGSPTR self, CONST S32 input)
{
    self->Value = input;

    STRINGVALUE name, value;
    AcquireSettingsValue(&name, self->ID);
    AcquireStringValue(&value, "%d", input);
    SaveSettingsValue(name, value);
}

// 0x100173c0
VOID AcquireSettingsValue(STRINGVALUEPTR result, CONST U32 indx, ...)
{
    CHAR setting[MAX_SETTINGS_VALUE_LENGTH];
    LoadStringA(State.Module->Text, indx, setting, MAX_SETTINGS_VALUE_LENGTH);

    CHAR output[MAX_SETTINGS_OUTPUT_VALUE_LENGTH];

    va_list args;
    va_start(args, indx);
    vsprintf(output, setting, args);
    va_end(args);

    result->Value = (LPSTR)malloc(strlen(output) + 1);

    strcpy(result->Value, output);
}

// 0x100174c0
STRINGVALUEPTR AcquireSettingsValue(STRINGVALUEPTR result, STRINGVALUE name, STRINGVALUE value)
{
    CHAR setting[MAX_SETTINGS_VALUE_LENGTH];
    LoadStringA(State.Module->Text, IDS_GAME, setting, MAX_SETTINGS_VALUE_LENGTH);

    if (State.IsIniActive)
    {
        CHAR output[MAX_SETTINGS_DATA_VALUE_LENGTH];
        GetPrivateProfileStringA(setting, name.Value, value.Value, output, MAX_SETTINGS_DATA_VALUE_LENGTH, State.App->Ini);
        AcquireStringValue(result, output);
    }
    else
    {
        LoadStringA(State.Module->Text, IDS_REGISTRY_KEY, setting, MAX_SETTINGS_VALUE_LENGTH);

        HKEY key = NULL;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, setting, REG_OPTION_NON_VOLATILE, KEY_READ, &key) == ERROR_SUCCESS)
        {
            U32 type = REG_NONE;
            U32 length = MAX_SETTINGS_DATA_VALUE_LENGTH;
            CHAR output[MAX_SETTINGS_DATA_VALUE_LENGTH];

            if (RegQueryValueExA(key, name.Value, NULL, (LPDWORD)&type, (BYTE*)output, (LPDWORD)&length) == ERROR_SUCCESS)
            {
                RegCloseKey(key);

                AcquireStringValue(result, output);

                ReleaseStringValue(&name);
                ReleaseStringValue(&value);

                return result;
            }
        }

        RegCloseKey(key);

        AcquireStringValue(result, &value);
    }

    ReleaseStringValue(&name);
    ReleaseStringValue(&value);

    return result;
}

// 0x10017600
S32 AcquireSettingsValue(STRINGVALUE name, CONST S32 value)
{
    CHAR input[MAX_SETTINGS_VALUE_LENGTH];
    LoadStringA(State.Module->Text, IDS_GAME, input, MAX_SETTINGS_VALUE_LENGTH);

    if (State.IsIniActive)
    {
        CONST S32 result = GetPrivateProfileIntA(input, name.Value, value, State.App->Ini);

        ReleaseStringValue(&name);

        return result;
    }

    STRINGVALUE nm, val;
    AcquireStringValue(&nm, &name);
    AcquireStringValue(&val, INVALID_SETTINGS_STRING_VALUE);

    STRINGVALUE setting;
    STRINGVALUEPTR actual = AcquireSettingsValue(&setting, nm, val);

    CHAR output[MAX_SETTINGS_OUTPUT_VALUE_LENGTH];
    strcpy(output, actual->Value);

    ReleaseStringValue(actual);

    if (strncmp(output, INVALID_SETTINGS_STRING_VALUE, MAX_INVALID_SETTINGS_STRING_VALUE_LENGTH) == 0)
    {
        ReleaseStringValue(&name);

        return value;
    }

    ReleaseStringValue(&name);

    return atoi(output);
}

// 0x100176f0
VOID SaveSettingsValue(STRINGVALUE name, STRINGVALUE value)
{
    CHAR setting[MAX_SETTINGS_VALUE_LENGTH];

    LoadStringA(State.Module->Text, IDS_GAME, setting, MAX_SETTINGS_VALUE_LENGTH);

    if (!State.IsIniActive)
    {
        HKEY key;
        CHAR registry[MAX_SETTINGS_VALUE_LENGTH];
        LoadStringA(State.Module->Text, IDS_REGISTRY_KEY, registry, MAX_SETTINGS_VALUE_LENGTH);
        RegOpenKeyExA(HKEY_LOCAL_MACHINE, registry, REG_OPTION_NON_VOLATILE, KEY_WRITE, &key);
        RegSetValueExA(key, name.Value, 0, REG_SZ, (BYTE*)value.Value, 4); // TODO 4 ?
        RegCloseKey(key);
    }
    else { WritePrivateProfileStringA(setting, name.Value, value.Value, State.App->Ini); }

    ReleaseStringValue(&name);
    ReleaseStringValue(&value);
}