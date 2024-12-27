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

#include "BinFile.hxx"

#define INVALID_BINARCHIVE_INDEX    (-1)

#define BINARCHIVE_MAGIC            0x53465A46 /* FZFS */

typedef enum BinArchiveType
{
    BINARCHIVETYPE_NONE = 0,
    BINARCHIVETYPE_FILE = 1,
    BINARCHIVETYPE_DIRECTORY = 2,
    BINARCHIVETYPE_FORCE_DWORD = 0x7FFFFFF
} BINARCHIVETYPE, * BINARCHIVETYPEPTR;

typedef struct BinArchiveHeader
{
    U32                 Magic;
    U32                 Offset;
} BINARCHIVEHEADER, * BINARCHIVEHEADERPTR;

typedef struct BinArchiveDescriptor
{
    U32                 Size;
    U32                 Count;
    U32                 Length;
} BINARCHIVEDESCRIPTOR, * BINARCHIVEDESCRIPTORPTR;

typedef struct BinArchive
{
    BINARCHIVETYPE      Type;
    CHAR                Name[MAX_FILE_NAME_LENGTH];
    U32* Offsets;
    LPSTR               Names;
    BINFILE             File;
} BINARCHIVE, * BINARCHIVEPTR;