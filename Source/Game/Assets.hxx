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

#include "BinArchive.hxx"

#define MAX_BINARY_ARCHIVE_COUNT    16
#define MAX_BINARY_CHUNK_COUNT      8
#define MAX_BINARY_FILE_COUNT       4096

typedef struct AssetStateModuleContainer
{
    BINFILECONTENT      Files[MAX_BINARY_FILE_COUNT];       // 0x10385728
    struct
    {
        U32             Count;                              // 0x10385720
        BINFILECHUNK    Values[MAX_BINARY_CHUNK_COUNT];     // 0x103a5728
    } Chunks;
    BINARCHIVE          Archives[MAX_BINARY_ARCHIVE_COUNT]; // 0x103a57a8
} ASSETSTATEMODULECONTAINER, * ASSETSTATEMODULECONTAINERPTR;

EXTERN ASSETSTATEMODULECONTAINER AssetsState;

LPCSTR AcquireAssetMessage(CONST U32 indx);
LPVOID AcquireBinFileChunk(CONST BFH indx, CONST U32 chunk);
LPVOID InitializeBinFileChunk(CONST BFH indx, CONST U32 chunk, CONST U32 size);
U32 AcquireBinFileChunkSize(CONST BFH indx, CONST U32 offset);
VOID DisposeAssets(VOID);
VOID DisposeBinFileChunks(VOID);
VOID ReleaseAssets(VOID);
VOID ReleaseBinFileChunks(VOID);