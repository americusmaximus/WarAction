/*
Copyright (c) 2024 - 2025 Americus Maximus

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

#include "Basic.hxx"
#include "Strings.hxx"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_STRING_BUFFER_LENGTH 64
#define UNIITEM(x, indx) ((BYTE)((LPBYTE)x)[indx])

STRINGSSTATE StringsState;

// 0x100419b8
CONST WCHAR DefaultUnicodeStringFilter[MAX_STRING_FILTER_ITEM_COUNT] =
{
    0x20AC, 0x002E, 0x201A, 0x002E, 0x201E, 0x2026, 0x2020, 0x2021, // € . ‚ „ … † ‡
    0x002E, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179, // . ‰ Š ‹ Ś Ť Ž Ź
    0x002E, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, // . ‘ ’ “ ” • – —
    0x002E, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A, // . ™ š › ś ť ž ź
    0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7, //   ˇ ˘ Ł ¤ Ą ¦ §
    0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B, // ¨ © Ş « ¬   ® Ż
    0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7, // ° ± ˛ ł ´ µ ¶ ·
    0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C, // ¸ ą ş » Ľ ˝ ľ ż
    0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7, // Ŕ Á Â Ă Ä Ĺ Ć Ç
    0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E, // Č É Ę Ë Ě Í Î Ď
    0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7, // Đ Ń Ň Ó Ô Ő Ö ×
    0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF, // Ř Ů Ú Ű Ü Ý Ţ ß
    0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7, // ŕ á â ă ä ĺ ć ç
    0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F, // č é ę ë ě í î ď
    0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7, // đ ń ň ó ô ő ö ÷
    0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9  // ř ů ú ű ü ý ţ ˙
};

// 0x10017440
STRINGVALUEPTR CLASSCALL AcquireStringValue(STRINGVALUEPTR self, STRINGVALUEPTR value)
{
    self->Value = (LPSTR)malloc(strlen(value->Value) + 1);

    strcpy(self->Value, value->Value);

    return self;
}

// 0x10017360
STRINGVALUEPTR AcquireStringValue(STRINGVALUEPTR self, LPCSTR format, ...)
{
    CHAR value[DEFAULT_STRING_BUFFER_LENGTH];

    va_list args;
    va_start(args, format);
    vsprintf(value, format, args);
    va_end(args);

    self->Value = (LPSTR)malloc(strlen(value) + 1);

    strcpy(self->Value, value);

    return self;
}

// 0x1000c910
LPSTR CLASSCALL AcquireStringValueValue(STRINGVALUEPTR self)
{
    return self->Value;
}

// 0x10023200
LPSTR* AcquireNextStringValue(LPSTR* input, LPSTR* result, CONST U32 count)
{
    LPSTR value = *input;

    for (U32 x = 0; x < count; x++) { value = AcquireNextString(value); }

    *result = value;

    return result;
}

// 0x100232e0
U32 AcquireUnicodeLength(CONST UNICHAR code)
{
    if (code < 0x80) { return 1; }

    if (code < 0x800) { return 2; }

    if (code < 0x10000) { return 3; }

    return ((0x1FFFFF < code) - 1U) & 4;
}

// 0x100231c0
U32 AcquireAnsiLength(CONST LPSTR value)
{
    if (UNIITEM(value, 0) < 0x80) { return 1; }

    if ((UNIITEM(value, 0) & 0xE0) == 0xC0) { return 2; }

    if ((UNIITEM(value, 0) & 0xF0) == 0xE0) { return 3; }

    return (((UNIITEM(value, 0) & 0xF8) != 0xF0) - 1U) & 4;
}

// 0x10023130
UNICHAR AcquireUnicode(CONST LPSTR value)
{
    if (UNIITEM(value, 0) < 0x80) { return (U32)UNIITEM(value, 0); }

    if ((UNIITEM(value, 0) & 0xE0) == 0xC0)
    {
        return (((UNICHAR)UNIITEM(value, 0) & 0x1FU) << 6) | ((UNICHAR)UNIITEM(value, 1) & 0x3FU);
    }

    if ((UNIITEM(value, 0) & 0xF0) == 0xE0)
    {
        return (((UNICHAR)UNIITEM(value, 0) & 0xFU) << 12)
            | (((UNICHAR)UNIITEM(value, 1) & 0x3FU) << 6) | ((UNICHAR)UNIITEM(value, 2) & 0x3FU);
    }

    if ((UNIITEM(value, 0) & 0xF8) == 0xF0)
    {
        return (((UNICHAR)UNIITEM(value, 0) & 0xFU) << 18) | (((UNICHAR)UNIITEM(value, 1) & 0x3FU) << 12)
            | (((UNICHAR)UNIITEM(value, 2) & 0x3FU) << 6) | ((UNICHAR)UNIITEM(value, 3) & 0x3FU);
    }

    return NULL;
}

// 0x10023230
U32 AppendUnicode(LPSTR value, CONST UNICHAR code)
{
    CONST BYTE one = (BYTE)(code & 0xFF);
    CONST BYTE two = (BYTE)((code >> 6) & 0xFF);
    CONST BYTE three = (BYTE)((code >> 12) & 0xFF);
    CONST BYTE four = (BYTE)((code >> 0x18) & 0xFF);

    {
        if (code < 0x80)
        {
            value[0] = one;

            return 1;
        }
    }
    {
        if (code < 0x800)
        {
            value[0] = (CHAR)(two | 0xC0);
            value[1] = (CHAR)((one & 0x3F) | 0x80);

            return 2;
        }
    }

    {
        if (code < 0x10000)
        {
            value[0] = (CHAR)(three | 0xE0);
            value[1] = (CHAR)((two & 0x3F) | 0x80);
            value[2] = (CHAR)((one & 0x3F) | 0x80);

            return 3;
        }
    }

    if (code < 0x200000)
    {
        value[0] = (CHAR)(four | 0xF0);
        value[1] = (CHAR)((three & 0x3F) | 0x80);
        value[2] = (CHAR)((two & 0x3F) | 0x80);
        value[3] = (CHAR)((one & 0x3F) | 0x80);

        return 4;
    }

    return 0;
}

// 0x100230d0
LPSTR AcquireNextString(CONST LPSTR value)
{
    LPSTR result = (LPSTR)(value + 1);

    for (U32 x = 0; x < 3 && UNIITEM(result, 0) != NULL
        && ((UNIITEM(result, 0) & 0xC0) != 0xC0) && 0x80 <= UNIITEM(result, 0); x++)
    {
        result = (LPSTR)((ADDR)result + 1);
    }

    return result;
}

// 0x10023100
LPSTR AcquirePriorString(CONST LPSTR value)
{
    LPSTR result = (LPSTR)(value - 1);

    for (U32 x = 0; x < 3 && UNIITEM(result, 0) != NULL
        && ((UNIITEM(result, 0) & 0xC0) != 0xC0) && 0x80 <= UNIITEM(result, 0); x++)
    {
        result = (LPSTR)((ADDR)result - 1);
    }

    return result;
}

// 0x10023480
U32 AcquireAnsiString(LPSTR result, CONST U32 length, LPCSTR value, CONST S32 limit)
{
    S32 indx = 0;
    LPSTR current = (LPSTR)value;
    LPWSTR items = (LPWSTR)alloca((length * sizeof(WCHAR) + 3) & 0xFFFFFFFC);

    if (length >= 0)
    {
        while (limit < 1 || ((ADDR)current - (ADDR)value) < limit)
        {
            CONST UNICHAR item = AcquireUnicode(current);

            items[indx] = (WCHAR)item;
            indx = indx + 1;

            if ((limit < 0 && item == NULL) || length <= indx) { break; }

            current = AcquireNextString(current);
        }
    }

    CONST CHAR unknown = '?';
    return WideCharToMultiByte(CP_ACP,
        WC_COMPOSITECHECK | WC_DEFAULTCHAR | WC_DISCARDNS, items, indx, result, length, &unknown, NULL);
}

// 0x100233a0
U32 AcquireUnicodeString(LPSTR result, CONST U32 length, LPCSTR value, CONST S32 limit)
{
    U32 count = 0;
    U32 size = 0;

    if (length == 0)
    {
        count = limit;

        if (limit < 1) { count = strlen(value); }

        count = count * 2;

        if (limit < 1) { size = strlen(value); }
        else { size = limit * 2; }
    }
    else { count = size = length * 2; }

    LPWSTR items = (LPWSTR)alloca((size + 3) & 0xFFFFFFFC);

    CONST U32 converted = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, value, limit, items, count);

    if (converted == 0) { return 0; }

    U32 characters = 0;

    for (U32 x = 0; x < converted; x++)
    {
        if (result != NULL)
        {
            if (length <= characters) { return characters; }

            result = (LPSTR)((ADDR)result + AppendUnicode(result, items[x]));
        }

        characters = characters + AcquireUnicodeLength(items[x]);
    }

    return characters;
}

// 0x10023520
VOID AcquireUnicodeStringFilter(WCHAR* result, LPCSTR value)
{
    LPSTR str = (LPSTR)value;

    for (U32 x = 0; x < MAX_STRING_FILTER_ITEM_COUNT; x++) { result[x] = NULL; }

    UNICHAR code = AcquireUnicode(str);
    for (U32 x = 0; code != NULL; str = AcquireNextString(str), code = AcquireUnicode(str))
    {
        if (code != '@' && ' ' < code)
        {
            result[x] = (WCHAR)code;
            x = x + 1;
        }
    }
}

// 0x10023570
U32 CleanUnicodeString(LPSTR result, CONST U32 length, LPCSTR value, CONST WCHAR* filter)
{
    U32 count = 0;
    LPSTR start = result;

    CONST WCHAR* actual = filter == NULL ? DefaultUnicodeStringFilter : filter;

    for (U32 x = 0; value[x] != NULL; x++)
    {
        CONST UNICHAR code = 0x80 < value[x] ? actual[value[x] & (MAX_STRING_FILTER_ITEM_COUNT - 1)] : value[x];

        if (start != NULL)
        {
            if (length < AcquireUnicodeLength(code) + count) { break; }

            result = result + AppendUnicode(result, code);
        }

        count = count + AcquireUnicodeLength(code);

    }

    if (start != NULL)
    {
        if (length <= count) { return 0; }

        AppendUnicode(result, NULL);
    }

    return count + 1;
}

// 0x10023630
LPSTR AcquireCleanUnicodeString(CONST LPSTR value, CONST BOOL release)
{
    if (value[0] == '@')
    {
        WCHAR filter[MAX_STRING_FILTER_ITEM_COUNT];

        LPSTR string = NULL;

        {
            U32 x = 0;
            for (; value[x] == '@'; x++)
            {
                while (value[x] != '\r' && value[x] != '\n') { x++; }
                while (value[x] == '\r' || value[x] == '\n') { x++; }
            }

            string = &value[x];
            value[x - 1] = NULL;
        }

        AcquireUnicodeStringFilter(filter, value);

        CONST U32 length = CleanUnicodeString(NULL, 0, string, filter);

        LPSTR result = (LPSTR)malloc(length + 1);
        CleanUnicodeString(result, length + 1, string, filter);

        if (release) { free(value); }

        return result;
    }

    if (value[0] != '%')
    {
        CONST U32 length = CleanUnicodeString(NULL, 0, value, DefaultUnicodeStringFilter);

        LPSTR result = (LPSTR)malloc(length + 1);
        CleanUnicodeString(result, length + 1, value, DefaultUnicodeStringFilter);

        if (release) { free(value); }

        return result;
    }

    if (!release)
    {
        LPSTR result = (LPSTR)malloc(strlen((LPCSTR)value) + 1);

        return (LPSTR)strcpy((LPSTR)result, (LPCSTR)&value[1]);
    }

    MoveMemory(value, &value[1], strlen((LPCSTR)value));

    return value;
}

// 0x10017480
VOID CLASSCALL ReleaseStringValue(STRINGVALUEPTR self)
{
    if (self->Value != NULL) { free(self->Value); }
}
