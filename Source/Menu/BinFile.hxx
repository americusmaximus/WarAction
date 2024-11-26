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

#include "Basic.hxx"

#include <BinFile.hxx>

#define INVALID_BINFILE_VALUE       (-1)
#define INVALID_BINARCHIVE_INDEX    (-1)

#define BINARCHIVE_MAGIC            0x53465A46 /* FZFS */

#define BINFILEHANDLE(x) ((HANDLE)x)

typedef enum BinFileOpenType
{
    BINFILEOPENTYPE_READ            = 0,
    BINFILEOPENTYPE_WRITE           = 1,
    BINFILEOPENTYPE_CREATE          = 2,
    BINFILEOPENTYPE_FORCE_DWORD     = 0x7FFFFFF
} BINFILEOPENTYPE, * BINFILEOPENTYPEPTR; // TODO Change to flags, similar to zup file

typedef enum BinFileContentType
{
    BINFILECONTENTTYPE_NONE             = 0,
    BINFILECONTENTTYPE_FILE             = 1, // Actual file
    BINFILECONTENTTYPE_COMBINED         = 2, // An archive with no data compression
    BINFILECONTENTTYPE_COMPRESSED       = 8, // An archive with data compression
    BINFILECONTENTTYPE_FORCE_DWORD      = 0x7FFFFFF
} BINFILECONTENTTYPE, * BINFILECONTENTTYPEPTR;

typedef struct BinFileContent
{
    LPSTR               Name;
    BINFILECONTENTTYPE  Type;
    U32                 Archive;
    BINFILE             File;
    U32                 Size;
    U32                 Chunk;
    U32                 Offset;
    BOOL                IsActive;
} BINFILECONTENT, * BINFILECONTENTPTR;

typedef enum BinArchiveType
{
    BINARCHIVETYPE_NONE             = 0,
    BINARCHIVETYPE_FILE             = 1,
    BINARCHIVETYPE_DIRECTORY        = 2,
    BINARCHIVETYPE_FORCE_DWORD      = 0x7FFFFFF
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

typedef struct BinFileInfo
{
    U32                 Name;
    BINFILECONTENTTYPE  Type;
    U32                 Index;
    U32                 Size;
    U32                 Chunk;
} BINFILEINFO, * BINFILEINFOPTR;

typedef struct BinArchive
{
    BINARCHIVETYPE      Type;
    CHAR                Name[MAX_FILE_NAME_LENGTH];
    U32*                Offsets;
    LPSTR               Names;
    BINFILE             File;
} BINARCHIVE, * BINARCHIVEPTR;

typedef struct BinFileChunk
{
    BFH                 Index;
    U32                 Chunk;
    U32                 Size;
    LPVOID              Content;
} BINFILECHUNK, * BINFILECHUNKPTR;

BOOL InitializeBinArchives(LPSTR names);
BOOL OpenBinArchiveFile(LPSTR name);
BOOL OpenBinArchives(LPSTR names);

BOOL CLASSCALL OpenBinFile(BINFILEPTR self, LPCSTR name, CONST BINFILEOPENTYPE type);
BOOL OpenBinArchiveDirectory(LPSTR name);
LPVOID ReadBinArchive(BINFILEPTR self, U32* count);
U32 CLASSCALL AcquireBinFileSize(BINFILEPTR self);
U32 CLASSCALL PointBinFile(BINFILEPTR self, CONST LONG distance, CONST DWORD method);
U32 CLASSCALL ReadBinFile(BINFILEPTR self, LPVOID content, CONST U32 size);
U32 CLASSCALL WriteBinFile(BINFILEPTR self, LPCVOID content, CONST U32 size);
VOID CLASSCALL CloseBinFile(BINFILEPTR self);
VOID CopyBinFile(BINFILEPTR src, BINFILEPTR dst, CONST U32 size);

BOOL AcquireBinArchive(CONST U32 indx, LPSTR name, LPSTR result, CONST BOOL overwrite);
VOID AcquireBinFile(BINFILEINFOPTR self, CONST U32 archive, LPSTR names, CONST BOOL overwrite);
VOID AcquireBinFile(LPSTR name, CONST U32 archive, CONST BOOL overwrite);