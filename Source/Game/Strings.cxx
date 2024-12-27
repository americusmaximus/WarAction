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

#include "Strings.hxx"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_STRING_BUFFER_LENGTH 64
#define UNIITEM(x, indx) ((BYTE)((LPBYTE)x)[indx])

// 0x1008eef0
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

// 0x1008ee90
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

// 0x1008f220
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