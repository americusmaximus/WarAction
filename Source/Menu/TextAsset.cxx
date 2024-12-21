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

#include "Assets.hxx"
#include "TextAsset.hxx"

#include <stdlib.h>

#define MAX_TEXT_ASSET_BUFFER_LENGTH 256

// 0x10002750
TEXTASSETPTR CLASSCALL ActivateTextAsset(TEXTASSETPTR self)
{
    self->Content = NULL;
    self->Count = 0;

    return self;
}

// 0x10002770
VOID CLASSCALL InitializeTextAsset(TEXTASSETPTR self, LPCSTR name)
{
    if (self->Content != NULL) { free(self->Content); }

    self->Content = NULL;
    self->Count = 0;

    if (name != NULL)
    {
        self->Length = AcquireAssetContent(name, (LPVOID*)&self->Content, 0);

        self->Content[self->Length - 1] = NULL;

        self->Count = 0;

        // Replace tabs with spaces.
        if (self->Content != NULL)
        {
            LPSTR pos = strchr(self->Content, '\t');

            while (pos != NULL)
            {
                *pos = ' ';

                pos = strchr(pos, '\t');
            }
        }

        // Count lines.
        {
            LPSTR content = self->Content;

            while (content[0] != NULL)
            {
                LPCSTR pos = strchr(content, '\n');

                if (pos == NULL) { content = (LPSTR)((ADDR)content + strlen(content) + 1); }
                else { content = (LPSTR)((ADDR)pos + 1); }

                self->Count = self->Count + 1;
            }
        }
    }
}

// 0x10002760
VOID CLASSCALL DisposeTextAsset(TEXTASSETPTR self)
{
    InitializeTextAsset(self, NULL);
}

// 0x10002810
// Acquire text line from the resouce at a specified index.
VOID CLASSCALL AcquireTextAssetString(TEXTASSETPTR self, CONST U32 line, LPSTR result)
{
    LPSTR value = self->Content;

    for (U32 x = 0; x < line; x++)
    {
        LPSTR line = strchr(value, '\n');

        if (line == NULL) { value = (LPSTR)((ADDR)value + strlen(value) + 1); }
        else { value = (LPSTR)((ADDR)line + 1); }
    }

    {
        U32 x = 0;

        while (value[x] != NULL && value[x] != '\n' && value[x] != '\r'
            && ((ADDR)value < ((ADDR)self->Content + self->Length)))
        {
            *result = value[x];

            x = x + 1;

            result = (LPSTR)((ADDR)result + 1);
        }

        *result = NULL;
    }
}

// 0x10002880
// Acquire text value from the resouce at a specified line and value indexes.
VOID CLASSCALL AcquireTextAssetStringValue(TEXTASSETPTR self, CONST U32 line, CONST U32 param, LPSTR result)
{
    CHAR value[MAX_TEXT_ASSET_BUFFER_LENGTH];
    AcquireTextAssetString(self, line, value);

    LPSTR pointer = value;

    for (U32 x = 0; x < param; x++)
    {
        // Skip all non-spaces until the end of the string.
        while (pointer[0] != NULL && pointer[0] != ' ') { pointer = (LPSTR)((ADDR)pointer + 1); }

        // Skip all sequential spaces.
        while (pointer[0] != NULL && pointer[0] == ' ') { pointer = (LPSTR)((ADDR)pointer + 1); }
    }

    {
        U32 x = 0;

        while (pointer[x] != NULL && pointer[x] != ' ')
        {
            *result = pointer[x];

            x = x + 1;

            result = (LPSTR)((ADDR)result + 1);
        }

        *result = NULL;
    }
}

// 0x10002900
U32 CLASSCALL AcquireTextAssetHash(TEXTASSETPTR self)
{
    S32 result = 0x31415926;

    CONST U32 count = self->Length / 4;

    for (U32 x = 0; x < count; x++)
    {
        result = ((U32*)self->Content)[x] + (result >> 29) + result * 8;
    }

    return result;
}