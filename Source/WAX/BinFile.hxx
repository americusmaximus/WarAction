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
#include "Native.Basic.hxx"

#define INVALID_BINFILE_VALUE       (-1)

#define BINFILEOPENTYPE_READ        0x0
#define BINFILEOPENTYPE_WRITE       0x1
#define BINFILEOPENTYPE_CREATE      0x2

#define BINFILEHANDLE(x) ((HANDLE)x)

#ifdef _WIN64
typedef U64 BFH;
#else
typedef U32 BFH;
#endif

typedef struct BinFile
{
    BFH Value;
} BINFILE, * BINFILEPTR;

typedef enum BinFileContentType
{
    BINFILECONTENTTYPE_NONE         = 0,
    BINFILECONTENTTYPE_FILE         = 1, // Actual file
    BINFILECONTENTTYPE_COMBINED     = 2, // An archive with no data compression
    BINFILECONTENTTYPE_COMPRESSED   = 8, // An archive with data compression
    BINFILECONTENTTYPE_FORCE_DWORD  = 0x7FFFFFF
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

typedef struct BinFileInfo
{
    U32                 Name;
    BINFILECONTENTTYPE  Type;
    U32                 Index;
    U32                 Size;
    U32                 Chunk;
} BINFILEINFO, * BINFILEINFOPTR;

typedef struct BinFileChunk
{
    BFH                 Index;
    U32                 Chunk;
    U32                 Size;
    LPVOID              Content;
} BINFILECHUNK, * BINFILECHUNKPTR;