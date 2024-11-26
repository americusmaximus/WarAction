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
#include "Compression.hxx"
#include "State.hxx"
#include "Strings.hxx"

#include <DirectDraw.hxx>

#include <stdio.h>
#include <zlib.h>

// 0x10001930
BOOL InitializeBinArchives(LPSTR names)
{
    AssetsState.IsActive = TRUE;

    return OpenBinArchives(names);
}

// 0x10024d80
BOOL OpenBinArchives(LPSTR names)
{
    while (names != NULL)
    {
        LPSTR divider = strchr(names, ';');

        if (divider != NULL) { divider[0] = NULL; }

        CONST U32 indx = strlen(names) - 1;

        if (names[indx] == '\\' || names[indx] == '/')
        {
            if (!OpenBinArchiveDirectory(names)) { return FALSE; }
        }
        else if (!OpenBinArchiveFile(names)) { return FALSE; }

        names = divider;

        if (divider != NULL)
        {
            while (TRUE)
            {
                CONST CHAR value = divider[1];

                if (value == NULL) { return TRUE; }

                divider = (LPSTR)((ADDR)divider + 1);

                if (value != ' ' && value != '\t') { break; }
            }

            names = divider;
        }
    }

    return TRUE;
}

// 0x10024740
BOOL OpenBinArchiveFile(LPSTR name)
{
    U32 indx = INVALID_BINARCHIVE_INDEX;

    for (U32 x = 0; x < MAX_BINARY_ARCHIVE_COUNT; x++)
    {
        if (AssetsState.Archives[x].Type == BINARCHIVETYPE_NONE) { indx = x; break; }
    }

    if (indx == INVALID_BINARCHIVE_INDEX) { return FALSE; }

    CONST BOOL overwrite = name[0] == '^';

    if (overwrite) { name = (LPSTR)((ADDR)name + 1); }

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    BINARCHIVEHEADER header;
    ZeroMemory(&header, sizeof(BINARCHIVEHEADER));

    OpenBinFile(&file, name, BINFILEOPENTYPE_READ);
    ReadBinFile(&file, &header, sizeof(BINARCHIVEHEADER));

    if (header.Magic == BINARCHIVE_MAGIC)
    {
        PointBinFile(&file, header.Offset, FILE_CURRENT);

        U32 count = 0;
        BINFILEINFOPTR files = (BINFILEINFOPTR)ReadBinArchive(&file, &count);
        LPSTR names = (LPSTR)ReadBinArchive(&file, NULL);
        U32* offsets = (U32*)ReadBinArchive(&file, NULL);

        for (U32 x = 0; x < count; x++) { AcquireBinFile(&files[x], indx, names, overwrite); }

        free(files);

        strcpy(AssetsState.Archives[indx].Name, name);

        AssetsState.Archives[indx].Type = BINARCHIVETYPE_FILE;
        AssetsState.Archives[indx].File.Value = INVALID_BINFILE_VALUE;
        AssetsState.Archives[indx].Names = names;
        AssetsState.Archives[indx].Offsets = offsets;

        CloseBinFile(&file);

        return TRUE;
    }

    CloseBinFile(&file);

    return FALSE;
}

#include <stdlib.h>

// 0x10022010
BOOL CLASSCALL OpenBinFile(BINFILEPTR self, LPCSTR name, CONST BINFILEOPENTYPE type)
{
    CHAR path[MAX_FILE_NAME_LENGTH];
    AcquireAnsiString(path, MAX_FILE_NAME_LENGTH, name, -1);

    DWORD access = GENERIC_READ;
    DWORD mode = FILE_SHARE_READ;

    if (mode & BINFILEOPENTYPE_WRITE)
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

// 0x10022110
U32 CLASSCALL PointBinFile(BINFILEPTR self, CONST LONG distance, CONST DWORD method)
{
    return SetFilePointer(BINFILEHANDLE(self->Value), distance, NULL, method);
}

// 0x100248a0
LPVOID ReadBinArchive(BINFILEPTR self, U32* count)
{
    BINARCHIVEDESCRIPTOR desc;
    ZeroMemory(&desc, sizeof(BINARCHIVEDESCRIPTOR));

    ReadBinFile(self, &desc, sizeof(BINARCHIVEDESCRIPTOR));

    LPVOID src = malloc(desc.Size);

    U32 length = desc.Length * desc.Count;
    LPVOID dst = malloc(length);

    ReadBinFile(self, src, desc.Size);

    UnZip(dst, &length, src, desc.Size);

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

// 0x10024ca0
BOOL OpenBinArchiveDirectory(LPSTR name)
{
    CHAR path[MAX_FILE_NAME_LENGTH];
    ZeroMemory(&path, MAX_FILE_NAME_LENGTH);

    ZeroMemory(StringsState.Scratch, MAX_SCRATCH_STRING_LENGTH);

    CONST BOOL overwrite = name[0] == '^';

    if (overwrite) { name = (LPSTR)((ADDR)name + 1); }

    for (U32 x = 0; x < MAX_BINARY_ARCHIVE_COUNT; x++)
    {
        if (AssetsState.Archives[x].Type != BINARCHIVETYPE_NONE) { continue; }

        strcpy(path, name);

        strcpy(AssetsState.Archives[x].Name, name);

        AssetsState.Archives[x].Type = BINARCHIVETYPE_DIRECTORY;

        return AcquireBinArchive(x, path, StringsState.Scratch, overwrite);
    }

    return FALSE;
}

// 0x10024a20
BOOL AcquireBinArchive(CONST U32 indx, LPSTR name, LPSTR result, CONST BOOL overwrite)
{
    CONST U32 namelen = strlen(name);

    strcpy(result, name);

    strcat(name, "*"); // Append * to the end

    WIN32_FIND_DATAA finder;
    ZeroMemory(&finder, sizeof(WIN32_FIND_DATAA));

    HANDLE search = FindFirstFileA(name, &finder);

    name[namelen] = NULL; // Restore name

    if (search != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (strcmp(finder.cFileName, ".") != 0 && strcmp(finder.cFileName, "..") != 0)
                {
                    CONST U32 reslen = strlen(result);

                    strcpy(result, finder.cFileName);

                    strcat(result, "\\"); // Append \ to the end

                    AcquireBinArchive(indx, name, result, overwrite);

                    result[reslen] = NULL; // Restore result
                }
            }
            else
            {
                LPSTR path = (LPSTR)malloc(strlen(result) + strlen(finder.cFileName) + 1);

                sprintf(path, "%s%s", result, finder.cFileName);

                AcquireBinFile(path, indx, overwrite);

            }
        } while (FindNextFileA(search, &finder));
    }

    FindClose(search);

    return TRUE;
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

// 0x100220e0
U32 CLASSCALL WriteBinFile(BINFILEPTR self, LPCVOID content, CONST U32 size)
{
    U32 length = size;

    WriteFile(BINFILEHANDLE(self->Value), content, length, (LPDWORD)&length, NULL);

    return length;
}

// 0x10022130
U32 CLASSCALL AcquireBinFileSize(BINFILEPTR self)
{
    return GetFileSize(BINFILEHANDLE(self->Value), NULL);
}