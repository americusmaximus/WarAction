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

#include "Basic.hxx"

typedef U32             UNICHAR;

#define UNICHAR_SIZE    sizeof(UNICHAR)

#define MAX_SCRATCH_STRING_LENGTH       256

#define MAX_STRING_FILTER_ITEM_COUNT    128

typedef struct StringValue
{
    LPSTR Value;
} STRINGVALUE, * STRINGVALUEPTR;

LPSTR CLASSCALL AcquireStringValueValue(STRINGVALUEPTR self);
STRINGVALUEPTR AcquireStringValue(STRINGVALUEPTR self, LPCSTR format, ...);
STRINGVALUEPTR CLASSCALL AcquireStringValue(STRINGVALUEPTR self, STRINGVALUEPTR value);
VOID CLASSCALL ReleaseStringValue(STRINGVALUEPTR self);

typedef struct StringsState
{
    CHAR Scratch[MAX_SCRATCH_STRING_LENGTH]; // 0x10045fc4
} STRINGSSTATE, * STRINGSSTATEPTR;

EXTERN STRINGSSTATE StringsState;
EXTERN CONST WCHAR DefaultUnicodeStringFilter[MAX_STRING_FILTER_ITEM_COUNT];

LPSTR* AcquireNextStringValue(LPSTR* input, LPSTR* result, CONST U32 count);

U32 AcquireUnicode(CONST LPSTR value);
U32 AcquireAnsiLength(CONST LPSTR value);
U32 AcquireUnicodeLength(CONST UNICHAR code);

U32 AcquireAnsiString(LPSTR result, CONST U32 length, LPCSTR value, CONST S32 limit);
U32 AcquireUnicodeString(LPSTR result, CONST U32 length, LPCSTR value, CONST S32 limit);

U32 AppendUnicode(LPSTR value, CONST UNICHAR code);
U32 CleanUnicodeString(LPSTR result, CONST U32 length, LPCSTR value, CONST WCHAR* filter);

LPSTR AcquireCleanUnicodeString(CONST LPSTR value, CONST BOOL release);

LPSTR AcquireNextString(CONST LPSTR value);
LPSTR AcquirePriorString(CONST LPSTR value);

VOID AcquireUnicodeStringFilter(WCHAR* result, LPCSTR value);