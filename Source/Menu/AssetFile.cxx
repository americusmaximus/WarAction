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

#include "AssetFile.hxx"
#include "Assets.hxx"
#include "BinFile.hxx"

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

// 0x10024f00
BFH AcquireAssetFileIndex(LPCSTR name)
{
    BFH indx = crc32(0, (BYTE*)name, strlen(name)) & (MAX_BINARY_FILE_COUNT - 1);

    for (BFH x = 0; x < MAX_BINARY_FILE_COUNT; x++)
    {
        CONST BINFILECONTENTPTR file = &AssetsState.Files[indx];

        if (file->Type != BINFILECONTENTTYPE_NONE && _strcmpi(file->Name, name) == 0) { return indx; }

        indx = (indx + 1) % MAX_BINARY_FILE_COUNT;
    }

    return INVALID_BINFILE_VALUE;
}

// 0x10024e40
BOOL OpenAssetFile(CONST BFH indx)
{
    if (indx == INVALID_BINFILE_VALUE) { return FALSE; }

    switch (AssetsState.Files[indx].Type)
    {
    case BINFILECONTENTTYPE_FILE:
    {
        CHAR path[MAX_FILE_NAME_LENGTH];
        sprintf(path, "%s%s", AssetsState.Archives[AssetsState.Files[indx].Archive].Name, AssetsState.Files[indx].Name);

        return OpenBinFile(&AssetsState.Files[indx].File, path, BINFILEOPENTYPE_READ);
    }
    case BINFILECONTENTTYPE_COMBINED:
    case BINFILECONTENTTYPE_COMPRESSED:
    {
        if (AssetsState.Files[indx].IsActive) { return FALSE; }

        AssetsState.Files[indx].Offset = 0;
        AssetsState.Files[indx].IsActive = TRUE;

        return TRUE;
    }
    }

    return FALSE;
}

// 0x10024e10
BOOL CLASSCALL OpenAssetFile(ASSETFILEPTR self, LPCSTR name)
{
    self->Value = AcquireAssetFileIndex(name);

    return self->Value != INVALID_BINFILE_VALUE ? OpenAssetFile(self->Value) : FALSE;
}

// 0x10024f90
BOOL IsAssetFileActive(CONST BFH indx)
{
    if (indx == INVALID_BINFILE_VALUE) { return FALSE; }

    switch (AssetsState.Files[indx].Type)
    {
    case BINFILECONTENTTYPE_FILE: { return AssetsState.Files[indx].File.Value != INVALID_BINFILE_VALUE; }
    case BINFILECONTENTTYPE_COMBINED:
    case BINFILECONTENTTYPE_COMPRESSED: { return AssetsState.Files[indx].IsActive; }
    }

    return FALSE;
}

// 0x10025010
VOID CloseAssetFile(CONST BFH indx)
{
    switch (AssetsState.Files[indx].Type)
    {
    case BINFILECONTENTTYPE_FILE: { CloseBinFile(&AssetsState.Files[indx].File); break; }
    case BINFILECONTENTTYPE_COMBINED:
    case BINFILECONTENTTYPE_COMPRESSED: { AssetsState.Files[indx].IsActive = FALSE; break; }
    }
}

// 0x10024ff0
VOID CLASSCALL CloseAssetFile(ASSETFILEPTR self)
{
    if (IsAssetFileActive(self)) { CloseAssetFile(self->Value); }
}

// 0x10024f80
BOOL CLASSCALL IsAssetFileActive(ASSETFILEPTR self)
{
    return IsAssetFileActive(self->Value);
}

// 0x100253e0
U32 AcquireAssetFileSize(CONST BFH indx)
{
    switch (AssetsState.Files[indx].Type)
    {
    case BINFILECONTENTTYPE_FILE: { return AcquireBinFileSize(&AssetsState.Files[indx].File); }
    case BINFILECONTENTTYPE_COMBINED:
    case BINFILECONTENTTYPE_COMPRESSED: { return AssetsState.Files[indx].Size; }
    }

    return 0;
}

// 0x100253c0
U32 CLASSCALL AcquireAssetFileSize(ASSETFILEPTR self)
{
    if (!IsAssetFileActive(self)) { return 0; }

    return AcquireAssetFileSize(self->Value);
}

// 0x10025040
U32 CLASSCALL ReadAssetFile(ASSETFILEPTR self, LPVOID content, U32 size)
{
    if (!IsAssetFileActive(self)) { return 0; }

    return ReadAssetFile(self->Value, content, size);
}

// 0x10025410
S32 CLASSCALL SelectAssetFileOffset(ASSETFILEPTR self, LONG distance, DWORD method)
{
    if (!IsAssetFileActive(self)) { return INVALID_ASSET_FILE_OFFSET; }

    return SelectAssetFileOffset(self->Value, distance, method);
}

// 0x100254c0
S32 CLASSCALL AcquireAssetFileString(ASSETFILEPTR self, LPSTR content, CONST U32 length)
{
    if (!IsAssetFileActive(self)) { return 0; }

    CONST U32 size = ReadAssetFile(self, content, length - 1);

    if (size == 0) { return INVALID_ASSET_FILE_STRING_LENGTH; }

    U32 actual = length;

    U32 count = 0;
    while (count < size)
    {
        if (content[count] == '\r') { actual = content[count + 1] == '\n'; break; }
        if (content[count] == '\n') { actual = content[count + 1] == '\r'; break; }

        count = count + 1;
    }

    content[count] = NULL;

    SelectAssetFileOffset(self, actual - size + 1 + count, FILE_CURRENT);

    return count;
}

// 0x10025070
U32 ReadAssetFile(CONST BFH indx, LPVOID content, U32 size)
{
    switch (AssetsState.Files[indx].Type)
    {
    case BINFILECONTENTTYPE_FILE:
    {
        if (AssetsState.Files[indx].File.Value == INVALID_BINFILE_VALUE) { return 0; }

        return ReadBinFile(&AssetsState.Files[indx].File, content, size);
    }
    case BINFILECONTENTTYPE_COMBINED:
    {
        CONST U32 result = ReadSingleChunkAssetFile(content, indx, AssetsState.Files[indx].Offset, size);

        AssetsState.Files[indx].Offset = AssetsState.Files[indx].Offset + result;

        return result;
    }
    case BINFILECONTENTTYPE_COMPRESSED:
    {
        CONST U32 result = ReadMultiChunkAssetFile(content, indx, AssetsState.Files[indx].Offset, size);

        AssetsState.Files[indx].Offset = AssetsState.Files[indx].Offset + result;

        return result;
    }
    }

    return 0;
}

// 0x10025340
U32 ReadSingleChunkAssetFile(LPVOID result, CONST BFH indx, CONST U32 offset, CONST U32 size)
{
    BINFILEPTR file = &AssetsState.Archives[AssetsState.Files[indx].Archive].File;

    if (AssetsState.Archives[AssetsState.Files[indx].Archive].File.Value == INVALID_BINFILE_VALUE)
    {
        OpenBinFile(file, AssetsState.Archives[AssetsState.Files[indx].Archive].Name, BINFILEOPENTYPE_READ);
    }

    U32 actual = size;

    if (AssetsState.Files[indx].Size < offset + actual) { actual = AssetsState.Files[indx].Size - offset; }

    if (actual != 0)
    {
        CONST U32 archive = AssetsState.Files[indx].Archive;
        CONST U32 offset = AssetsState.Files[indx].Offset;

        CONST U32 start = AssetsState.Archives[archive].Offsets[offset];

        PointBinFile(file, start + offset, FILE_BEGIN);

        ReadBinFile(file, result, actual);
    }

    return actual;
}

// 0x10025110
U32 ReadMultiChunkAssetFile(LPVOID result, CONST BFH indx, CONST U32 offset, CONST U32 size)
{
    if (AssetsState.Files[indx].Size <= offset) { return 0; }

    U32 chunk = offset / AssetsState.Files[indx].Chunk;

    // Offset within current chunk, 0 for non-initial chunk.
    U32 start = offset - chunk * AssetsState.Files[indx].Chunk;

    U32 completed = 0;
    U32 left = size;

    U32 actual = size;
    if (AssetsState.Files[indx].Size < offset + actual)
    {
        left = AssetsState.Files[indx].Size - offset;
        actual = left;
    }

    while (left != 0)
    {
        CONST U32 length = left < AssetsState.Files[indx].Chunk - start
            ? left : AssetsState.Files[indx].Chunk - start;

        LPVOID content = ReadAssetFileChunk(indx, chunk);

        if (content == NULL) { break; }

        memcpy((LPVOID)((ADDR)result + completed), (LPVOID)((ADDR)content + start), length);

        completed = completed + length;
        left = left - length;

        start = 0; // Start reading from the beginning of chunk for non-first chunk.
        chunk = chunk + 1;
    }

    return actual;
}

// 0x100251e0
LPVOID ReadAssetFileChunk(CONST BFH indx, CONST U32 chunk)
{
    LPVOID result = AcquireBinFileChunk(indx, chunk);

    if (result == NULL)
    {
        CONST U32 archive = AssetsState.Files[indx].Archive;
        BINFILEPTR file = &AssetsState.Archives[archive].File;

        if (AssetsState.Archives[archive].File.Value == INVALID_BINFILE_VALUE)
        {
            OpenBinFile(file, AssetsState.Archives[archive].Name, BINFILEOPENTYPE_READ);
        }

        CONST U32 index = AssetsState.Files[indx].File.Value + chunk;

        PointBinFile(file, AssetsState.Archives[archive].Offsets[index], FILE_BEGIN);

        CONST U32 length =
            AssetsState.Archives[archive].Offsets[index + 1] - AssetsState.Archives[archive].Offsets[index];

        Bytef* content = (Bytef*)malloc(length);

        U32 read = AcquireBinFileChunkSize(indx, chunk * AssetsState.Files[indx].Chunk);

        result = InitializeBinFileChunk(indx, chunk, read);

        if (ReadBinFile(file, content, length) != length) { return NULL; }

        uncompress((Bytef*)result, (uLongf*)&read, content, length);

        free(content);
    }

    return result;
}

// 0x10025440
U32 SelectAssetFileOffset(CONST U32 indx, LONG distance, DWORD method)
{
    CONST BINFILECONTENTTYPE type = AssetsState.Files[indx].Type;

    if (type == BINFILECONTENTTYPE_FILE) { return PointBinFile(&AssetsState.Files[indx].File, distance, method); }

    if (type != BINFILECONTENTTYPE_COMBINED && type != BINFILECONTENTTYPE_COMPRESSED) { return 0; }

    switch (method)
    {
    case FILE_BEGIN: { AssetsState.Files[indx].Offset = distance; break; }
    case FILE_CURRENT: { AssetsState.Files[indx].Offset = AssetsState.Files[indx].Offset + distance; break; }
    case FILE_END: { AssetsState.Files[indx].Offset = AssetsState.Files[indx].Size - distance; break; }
    }

    return AssetsState.Files[indx].Offset;
}