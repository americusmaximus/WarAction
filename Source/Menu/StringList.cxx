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

#include "StringList.hxx"

#include <stdlib.h>

// 0x1003a598
STRINGLISTSELF StringListSelfState =
{
    AcquireStringListItem
};

// NOTE: Inlined.
STRINGLISTPTR CLASSCALL ActivateStringList(STRINGLISTPTR self)
{
    self->Self = &StringListSelfState;

    self->Count = 0;

    return self;
}

// 0x10006900
VOID CLASSCALL AcquireStringListItem(STRINGLISTPTR self, CONST U32 indx, LPSTR value)
{
    strcpy(value, self->Items[indx]);
}

// 0x10006880
VOID CLASSCALL ReleaseStringList(STRINGLISTPTR self)
{
    for (S32 x = 0; x < self->Count; x++) { free(self->Items[x]); }

    self->Count = 0;
}

// 0x100068c0
VOID CLASSCALL AppendStringList(STRINGLISTPTR self, LPCSTR value)
{
    LPSTR content = (LPSTR)malloc(strlen(value) + 1);

    strcpy(content, value);

    InsertStringList(self, content);
}

// 0x10006860
VOID CLASSCALL InsertStringList(STRINGLISTPTR self, LPSTR value)
{
    self->Items[self->Count] = value;
    self->Count = self->Count + 1;
}

// 0x100122e0
LPCSTR CLASSCALL AcquireStringListItem(STRINGLISTPTR self, CONST U32 indx)
{
    return self->Items[indx];
}