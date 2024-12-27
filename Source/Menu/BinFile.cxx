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
#include "State.hxx"

#include <stdio.h>

// 0x10022010
BOOL CLASSCALL OpenBinFile(BINFILEPTR self, LPCSTR name, CONST U32 type)
{
    CHAR path[MAX_FILE_NAME_LENGTH];
    AcquireAnsiString(path, MAX_FILE_NAME_LENGTH, name, -1);

    DWORD access = GENERIC_READ;
    DWORD mode = FILE_SHARE_READ;

    if (type & BINFILEOPENTYPE_WRITE)
    {
        access = access | GENERIC_WRITE;
        mode = mode | FILE_SHARE_WRITE;
    }

    CONST U32 disposition = (type & BINFILEOPENTYPE_CREATE) ? CREATE_ALWAYS : OPEN_EXISTING;

    self->Value = (BFH)CreateFileA(path, access, mode, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);

    return self->Value != INVALID_BINFILE_VALUE;
}

// 0x10022090
VOID CLASSCALL CloseBinFile(BINFILEPTR self)
{
    CloseHandle(BINFILEHANDLE(self->Value));

    self->Value = INVALID_BINFILE_VALUE;
}

// 0x100220b0
U32 CLASSCALL ReadBinFile(BINFILEPTR self, LPVOID content, CONST U32 size)
{
    U32 actual = size;

    ReadFile(BINFILEHANDLE(self->Value), content, size, (LPDWORD)&actual, NULL);

    return actual;
}

// 0x100220e0
U32 CLASSCALL WriteBinFile(BINFILEPTR self, LPCVOID content, CONST U32 size)
{
    U32 length = size;

    WriteFile(BINFILEHANDLE(self->Value), content, length, (LPDWORD)&length, NULL);

    return length;
}

// 0x10022110
U32 CLASSCALL PointBinFile(BINFILEPTR self, CONST LONG distance, CONST DWORD method)
{
    return SetFilePointer(BINFILEHANDLE(self->Value), distance, NULL, method);
}

// 0x10022130
U32 CLASSCALL AcquireBinFileSize(BINFILEPTR self)
{
    return GetFileSize(BINFILEHANDLE(self->Value), NULL);
}

// 0x100248a0
LPVOID ReadBinArchive(BINFILEPTR self, U32* count)
{
    BINARCHIVEDESCRIPTOR desc;
    ZeroMemory(&desc, sizeof(BINARCHIVEDESCRIPTOR));

    ReadBinFile(self, &desc, sizeof(BINARCHIVEDESCRIPTOR));

    Bytef* src = (Bytef*)malloc(desc.Size);

    U32 length = desc.Length * desc.Count;
    LPVOID dst = malloc(length);

    ReadBinFile(self, src, desc.Size);

    uncompress((Bytef*)dst, (uLongf*)&length, src, desc.Size);

    if (count != NULL) { *count = desc.Count; }

    free(src);

    return dst;
}

// 0x10024920
VOID AcquireBinFile(BINFILEINFOPTR self, CONST U32 archive, LPSTR names, CONST BOOL overwrite)
{
    CONST LPSTR name = (LPSTR)((ADDR)names + (ADDR)self->Name);

    BFH indx = crc32(0, (LPBYTE)name, strlen(name)) & (MAX_BINARY_FILE_COUNT - 1);

    for (BFH x = 0; x < MAX_BINARY_FILE_COUNT; x++)
    {
        if (AssetsState.Files[indx].Type == BINFILECONTENTTYPE_NONE
            || _strcmpi(AssetsState.Files[indx].Name, name) == 0) { break; }

        indx = (indx + 1) % MAX_BINARY_FILE_COUNT;
    }

    if (AssetsState.Files[indx].Type == BINFILECONTENTTYPE_NONE
        || _strcmpi(AssetsState.Files[indx].Name, name) != 0 || overwrite)
    {
        AssetsState.Files[indx].Name = name;
        AssetsState.Files[indx].Type = self->Type;
        AssetsState.Files[indx].Archive = archive;
        AssetsState.Files[indx].File.Value = self->Index;
        AssetsState.Files[indx].Size = self->Size;
        AssetsState.Files[indx].Chunk = self->Chunk;
        AssetsState.Files[indx].Offset = 0;
    }
}

// 0x10024bd0
VOID AcquireBinFile(LPSTR name, CONST U32 archive, CONST BOOL overwrite)
{
    BFH indx = crc32(0, (BYTE*)name, strlen(name)) & (MAX_BINARY_FILE_COUNT - 1);

    for (BFH x = 0; x < MAX_BINARY_FILE_COUNT; x++)
    {
        if (AssetsState.Files[indx].Type == BINFILECONTENTTYPE_NONE
            || _strcmpi(AssetsState.Files[indx].Name, name) == 0) { break; }

        indx = (indx + 1) % MAX_BINARY_FILE_COUNT;
    }

    if (AssetsState.Files[indx].Type == BINFILECONTENTTYPE_NONE
        || _strcmpi(AssetsState.Files[indx].Name, name) != 0 || overwrite)
    {
        AssetsState.Files[indx].Name = name;
        AssetsState.Files[indx].Type = BINFILECONTENTTYPE_FILE;
        AssetsState.Files[indx].Archive = archive;
        AssetsState.Files[indx].File.Value = INVALID_BINFILE_VALUE;
    }
}

// 0x100187c0
VOID CopyBinFile(BINFILEPTR src, BINFILEPTR dst, CONST U32 size)
{
    U32 actual = size;

    while (actual != 0)
    {
        CONST U32 length = ((GRAPHICS_RESOLUTION_640 * GRAPHICS_RESOLUTION_480 * sizeof(PIXEL)) - 1) < actual
            ? (GRAPHICS_RESOLUTION_640 * GRAPHICS_RESOLUTION_480 * sizeof(PIXEL)) : actual;

        ReadBinFile(src, State.Renderer->Surface.Back, size);
        WriteBinFile(dst, State.Renderer->Surface.Back, size);

        actual = actual - length;
    }
}