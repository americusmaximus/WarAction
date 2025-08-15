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

#include "Assets.hxx"
#include "BinFile.hxx"

#include <stdio.h>
#include <stdlib.h>

// 0x100909e0
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