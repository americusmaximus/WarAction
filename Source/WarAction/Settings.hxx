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

#include "Strings.hxx"

#define INVALID_SETTINGS_STRING_VALUE "!"
#define MAX_INVALID_SETTINGS_STRING_VALUE_LENGTH 2

// ORIGINAL: .\\SudTest.ini
#ifdef _WIN64
#define SETTINGS_FILE_NAME ".\\WarAction.x64.ini"
#else
#define SETTINGS_FILE_NAME ".\\WarAction.x32.ini"
#endif

U32 AcquireSettingsValue(CONST U32 indx, LPSTR value, CONST U32 length);
STRINGVALUEPTR CLASSCALL AcquireSettingsValue(STRINGVALUEPTR result, CONST U32 indx, ...);
STRINGVALUEPTR AcquireSettingsValue(STRINGVALUEPTR result, STRINGVALUE name, STRINGVALUE value);

S32 AcquireSettingsValue(STRINGVALUE name, S32 value);

BOOL AcquireRendererSettingsValue(VOID);