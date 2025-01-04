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
#include "AssetFile.hxx"
#include "BinFile.hxx"
#include "Logger.hxx"

#include <..\Text\Resources.hxx>

#include <stdlib.h>

ASSETSTATEMODULECONTAINER AssetsState;

// 0x10056990
BOOL AcquireAssetContent(LPCSTR name, CONST U32 offset, LPVOID content, CONST U32 size)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenAssetFile(&file, name))
    {
        LogError("CAFILE", "File not found %s\n", name);

        return FALSE;
    }

    if (offset != INVALID_ASSET_FILE_OFFSET) { SelectAssetFileOffset(&file, offset, FILE_BEGIN); }

    U32 length = size;

    if (length == INVALID_ASSET_FILE_SIZE)
    {
        CONST U32 end = AcquireAssetFileSize(&file);

        length = offset == INVALID_ASSET_FILE_OFFSET ? end - AcquireAssetFileOffset(&file) : end - offset;
    }

    ReadAssetFile(&file, content, length);
    CloseAssetFile(&file);

    return TRUE;
}

// 0x10056a30
LPVOID AcquireAssetContent(LPCSTR name, CONST U32 offset, CONST U32 size)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenAssetFile(&file, name))
    {
        LogError("CAFILE", "File not found %s\n", name);

        return NULL;
    }

    if (offset != INVALID_ASSET_FILE_OFFSET) { SelectAssetFileOffset(&file, offset, FILE_BEGIN); }

    U32 length = size;

    if (length == INVALID_ASSET_FILE_SIZE)
    {
        CONST U32 end = AcquireAssetFileSize(&file);

        length = offset == INVALID_ASSET_FILE_OFFSET ? end - AcquireAssetFileOffset(&file) : end - offset;
    }

    if (length == 0)
    {
        CloseAssetFile(&file);

        return NULL;
    }

    LPVOID content = AllocateZero(length);

    if (content == NULL)
    {
        CloseAssetFile(&file);

        return NULL;
    }

    ReadAssetFile(&file, content, length);
    CloseAssetFile(&file);

    return content;
}

// 0x10056b00
LPVOID AcquireAssetContentIndexes(LPCSTR name, U32* indexes, CONST U32 count)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenAssetFile(&file, name))
    {
        LogError("CAFILE", "File not found %s\n", name);

        return NULL;
    }

    ReadAssetFile(&file, indexes, count * sizeof(U32));

    U32 end = 0;
    ReadAssetFile(&file, &end, sizeof(U32));

    CONST U32 size = end - indexes[0];

    LPVOID content = AllocateZero(size);

    if (content == NULL)
    {
        CloseAssetFile(&file);
        LogError("CAFILE", IDS_OUT_OF_MEMORY);

        return NULL;
    }

    ReadAssetFile(&file, content, size);
    CloseAssetFile(&file);

    for (U32 x = 0; x < count; x++)
    {
        indexes[x] = (ADDR)content + indexes[x] - (count + 1) * sizeof(U32);
    }

    return content;
}

// 0x10056bf0
LPVOID AcquireAssetContentCount(LPCSTR name, U32* count)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenAssetFile(&file, name))
    {
        LogError("CAFILE", "File not found %s\n", name);

        return NULL;
    }

    CONST U32 size = AcquireAssetFileSize(&file) - sizeof(U32);
    LPVOID content = AllocateZero(size);

    U32 items = 0;
    ReadAssetFile(&file, &items, sizeof(U32));
    ReadAssetFile(&file, content, size);
    CloseAssetFile(&file);

    ADDR* indxs = (ADDR*)content;
    for (U32 x = 0; x < items; x++)
    {
        indxs[x] = (ADDR)content + indxs[x] - sizeof(U32);
    }

    if (count != NULL) { *count = items; }

    return content;
}

// 0x10056ca0
U32 AcquireAssetContentOffset(LPCSTR name, CONST U32 indx, LPVOID content, CONST U32 size)
{
    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenAssetFile(&file, name))
    {
        LogError("CAFILE", "File not found %s\n", name);

        return FALSE;
    }

    SelectAssetFileOffset(&file, (indx + 1) * sizeof(U32), FILE_BEGIN);

    U32 start = 0, end = 0;
    ReadAssetFile(&file, &start, sizeof(U32));
    ReadAssetFile(&file, &end, sizeof(U32));

    if (start == end) { return 0; }

    U32 length = end - start;

    if (size < length) { length = size; }

    CONST U32 result = SelectAssetFileOffset(&file, start, FILE_BEGIN);

    ReadAssetFile(&file, content, length);
    CloseAssetFile(&file);

    return result;
}

// 0x10091570
LPVOID AcquireBinFileChunk(CONST BFH indx, CONST U32 chunk)
{
    for (U32 x = 0; x < MAX_BINARY_CHUNK_COUNT; x++)
    {
        if (AssetsState.Chunks.Values[x].Index == indx
            && AssetsState.Chunks.Values[x].Chunk == chunk && AssetsState.Chunks.Values[x].Content != NULL)
        {
            return AssetsState.Chunks.Values[x].Content;
        }
    }

    return NULL;
}

// 0x100915b0
U32 AcquireBinFileChunkSize(CONST BFH indx, CONST U32 offset)
{
    return AssetsState.Files[indx].Size < AssetsState.Files[indx].Chunk + offset
        ? AssetsState.Files[indx].Size - offset : AssetsState.Files[indx].Chunk;
}

// 0x10090940
LPVOID InitializeBinFileChunk(CONST BFH indx, CONST U32 chunk, CONST U32 size)
{
    if (AssetsState.Chunks.Values[AssetsState.Chunks.Count].Content != NULL)
    {
        free(AssetsState.Chunks.Values[AssetsState.Chunks.Count].Content);
    }

    AssetsState.Chunks.Values[AssetsState.Chunks.Count].Index = indx;
    AssetsState.Chunks.Values[AssetsState.Chunks.Count].Chunk = chunk;
    AssetsState.Chunks.Values[AssetsState.Chunks.Count].Size = size;

    AssetsState.Chunks.Values[AssetsState.Chunks.Count].Content = malloc(size);

    LPVOID result = AssetsState.Chunks.Values[AssetsState.Chunks.Count].Content;

    AssetsState.Chunks.Count = (AssetsState.Chunks.Count + 1) % MAX_BINARY_CHUNK_COUNT;

    return result;
}

// 0x100909b0
VOID ReleaseBinFileChunks(VOID)
{
    for (U32 x = 0; x < MAX_BINARY_CHUNK_COUNT; x++)
    {
        if (AssetsState.Chunks.Values[x].Content != NULL) { free(AssetsState.Chunks.Values[x].Content); }
    }

    DisposeBinFileChunks();
}

// 0x10090910
VOID DisposeBinFileChunks(VOID)
{
    AssetsState.Chunks.Count = 0;

    for (U32 x = 0; x < MAX_BINARY_CHUNK_COUNT; x++)
    {
        if (AssetsState.Chunks.Values[x].Content != NULL) { AssetsState.Chunks.Values[x].Content = NULL; }
    }
}

// 0x10056940
// 0x10091880
VOID ReleaseAssets(VOID)
{
    ReleaseBinFileChunks();

    for (U32 x = 0; x < MAX_BINARY_FILE_COUNT; x++)
    {
        if (IsAssetFileActive(x)) { CloseAssetFile(x); }

        if (AssetsState.Files[x].Type == BINFILECONTENTTYPE_FILE) { free(AssetsState.Files[x].Name); }
    }

    for (U32 x = 0; x < MAX_BINARY_ARCHIVE_COUNT; x++)
    {
        if (AssetsState.Archives[x].Type == BINARCHIVETYPE_FILE)
        {
            if (AssetsState.Archives[x].File.Value != INVALID_BINFILE_VALUE) { CloseBinFile(&AssetsState.Archives[x].File); }

            free(AssetsState.Archives[x].Offsets);
            free(AssetsState.Archives[x].Names);
        }
    }

    DisposeAssets();
}

// 0x10091840
VOID DisposeAssets(VOID)
{
    for (U32 x = 0; x < MAX_BINARY_FILE_COUNT; x++)
    {
        AssetsState.Files[x].Type = BINFILECONTENTTYPE_NONE;
    }

    for (U32 x = 0; x < MAX_BINARY_ARCHIVE_COUNT; x++)
    {
        AssetsState.Archives[x].Type = BINARCHIVETYPE_NONE;
    }

    DisposeBinFileChunks();
}