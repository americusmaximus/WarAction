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
#include "AssetFile.hxx"
#include "BinFile.hxx"
#include "Logger.hxx"

#include <stdlib.h>

ASSETSTATEMODULECONTAINER AssetsState;

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