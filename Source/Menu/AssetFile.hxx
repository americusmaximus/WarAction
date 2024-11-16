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

#define INVALID_ASSET_FILE_OFFSET           (-1)
#define INVALID_ASSET_FILE_STRING_LENGTH    (-1)

typedef struct AssetFile
{
    BFH Value;
} ASSETFILE, * ASSETFILEPTR;

BOOL CLASSCALL IsAssetFileActive(ASSETFILEPTR self);
BOOL CLASSCALL OpenAssetFile(ASSETFILEPTR self, LPCSTR name);
S32 CLASSCALL AcquireAssetFileString(ASSETFILEPTR self, LPSTR content, CONST U32 length);
S32 CLASSCALL SelectAssetFileOffset(ASSETFILEPTR self, LONG distance, DWORD method);
U32 CLASSCALL AcquireAssetFileSize(ASSETFILEPTR self);
U32 CLASSCALL ReadAssetFile(ASSETFILEPTR self, LPVOID content, U32 size);
VOID CLASSCALL CloseAssetFile(ASSETFILEPTR self);

BFH AcquireAssetFileIndex(LPCSTR name);
BOOL IsAssetFileActive(CONST BFH indx);
BOOL OpenAssetFile(CONST BFH indx);
LPVOID ReadAssetFileChunk(CONST BFH indx, CONST U32 chunk);
U32 AcquireAssetFileSize(CONST BFH indx);
U32 ReadAssetFile(CONST BFH indx, LPVOID content, CONST U32 size);
U32 ReadMultiChunkAssetFile(LPVOID result, CONST BFH indx, CONST U32 offset, CONST U32 size);
U32 ReadSingleChunkAssetFile(LPVOID result, CONST BFH indx, CONST U32 offset, CONST U32 size);
U32 SelectAssetFileOffset(CONST U32 indx, LONG distance, DWORD method);
VOID CloseAssetFile(CONST BFH indx);