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
#include "BinFile.hxx"

#include <stdio.h>

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