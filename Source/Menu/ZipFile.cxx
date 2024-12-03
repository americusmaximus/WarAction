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
#include "State.hxx"
#include "ZipFile.hxx"

#include <DirectDraw.hxx>

#define MAX_ZIPFILE_MODE_COUNT      4

// 0x10025630
BOOL CLASSCALL OpenZipFile(ZIPFILEPTR self, LPCSTR name, CONST U32 mode)
{
    CHAR path[MAX_FILE_NAME_LENGTH];
    AcquireAnsiString(path, MAX_FILE_NAME_LENGTH, name, -1);

    CONST CHAR* modes[MAX_ZIPFILE_MODE_COUNT] = { "rb", "r+b", "wb", "w+b" };

    self->File = gzopen(path, modes[mode]);

    return self->File != NULL;
}

// 0x100256e0
VOID CLASSCALL ReadZipFile(ZIPFILEPTR self, LPVOID value, CONST U32 size)
{
    if (self->Unk02 == 0) // TODO
    {
        gzread(self->File, value, size);

        self->Offset = (U32)gztell(self->File);

        return;
    }

    self->Offset = self->Offset + size;
}

// 0x10018820
VOID WriteZipFile(ZIPFILEPTR self, BINFILEPTR dst, CONST U32 size)
{
    U32 actual = size;

    while (actual != 0)
    {
        CONST U32 length = ((GRAPHICS_RESOLUTION_640 * GRAPHICS_RESOLUTION_480 * sizeof(PIXEL)) - 1) < actual
            ? (GRAPHICS_RESOLUTION_640 * GRAPHICS_RESOLUTION_480 * sizeof(PIXEL)) : actual;

        ReadZipFile(self, State.Renderer->Surface.Back, length);
        WriteBinFile(dst, State.Renderer->Surface.Back, length);

        actual = actual - length;
    }
}

// 0x100256d0
VOID CLASSCALL CloseZipFile(ZIPFILEPTR self)
{
    gzclose(self->File);
}

// 0x10025720
VOID CLASSCALL WriteZipFile(ZIPFILEPTR self, LPVOID value, CONST U32 size)
{
    if (self->Unk02 == 0) // TODO
    {
        gzwrite(self->File, value, size);

        self->Offset = (U32)gztell(self->File);

        return;
    }

    self->Offset = self->Offset + size;
}

// 0x10025760
U32 CLASSCALL TellZipFile(ZIPFILEPTR self)
{
    return self->Unk02 == 0 ? (U32)gztell(self->File) : self->Offset; // TODO
}