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
#include "BinAsset.hxx"
#include "Logger.hxx"
#include "State.hxx"

#include <stdlib.h>

// 0x10006050
BINASSETPTR CLASSCALL ActivateBinAsset(BINASSETPTR self)
{
    self->Content = NULL;
    self->IsCollection = FALSE;
    self->Name = NULL;
    self->IsImage = FALSE;

    return self;
}

// 0x10006080
BOOL CLASSCALL InitializeBinAsset(BINASSETPTR self, LPCSTR name, CONST BOOL load)
{
    if (load) { self->Name = name; }

    if (name != NULL && !load)
    {
        self->Name = name;

        return TRUE;
    }

    LogMessage("Loading %s\n", self->Name);

    AcquireAssetContent(self->Name, (LPVOID*)&self->Content, 0);

    if (self->IsImage)
    {
        LogMessage("Adjusting %s\n", self->Name);

        AdjustBinAssetImage(self);
    }

    self->Name = NULL;

    return TRUE;
}

// 0x10006200
VOID CLASSCALL AdjustBinAssetImage(BINASSETPTR self)
{
    if (self->Content == NULL) { return; }

    CONST U32 count = AcquireBinAssetItemCount(self);

    for (U32 x = 0; x < count; x++)
    {
        AdjustBinAssetImage((IMAGESPRITEPTR)AcquireBinAssetContent(self, x),
            State.Renderer->RedOffset, State.Renderer->ActualRedMask,
            State.Renderer->GreenOffset, State.Renderer->ActualGreenMask,
            State.Renderer->BlueOffset, State.Renderer->ActualBlueMask);
    }

    self->IsImage = TRUE;
}

// 0x10006130
VOID CLASSCALL AsStringBinAsset(BINASSETPTR self)
{
    CONST U32 count = ((U32*)self->Content)[0]; // TODO

    CopyMemory(self->Content, (LPVOID)((ADDR)self->Content + sizeof(U32)), count * sizeof(U32));

    ((U32*)self->Content)[count] = NULL;
}

// 0x10006060
VOID CLASSCALL DisposeBinAsset(BINASSETPTR self)
{
    if (self->Content != NULL)
    {
        free(self->Content);

        self->Content = NULL;
    }
}

// 0x100061c0
LPVOID CLASSCALL AcquireBinAssetContent(BINASSETPTR self, CONST U32 indx)
{
    if (self->Content == NULL) { InitializeBinAsset(self, NULL, FALSE); }

    // Counted:
    //      1. Count of elements at 0 offset
    //      2. An array of offsets into the file
    //      3. Actual data at those offsets
    if (self->IsCollection) { return self->Collection->Items[indx]; }

    // Non-Counted:
    //      1. An array of offsets into the file
    //      2. Actual data at those offsets
    return (LPVOID)((ADDR)self->Content + self->Content->Offset[indx]); // TODO types
}

// 0x100060f0
U32 CLASSCALL AcquireBinAssetItemCount(BINASSETPTR self)
{
    if (self->Content == NULL) { AcquireAssetContent(self->Name, (LPVOID*)&self->Content, 0); }

    return self->Content->Offset[0] / sizeof(BINASSETCONTENT);
}

// 0x10006170
VOID CLASSCALL InitializeBinAsset(BINASSETPTR self, CONST U32 count)
{
    CONST U32 offset = count * sizeof(U32);

    // Initialize a dynamic-size structure.
    BINASSETCOLLECTIONCONTENTPTR content = (BINASSETCOLLECTIONCONTENTPTR)malloc(sizeof(U32) + offset);

    content->Offset = offset;

    self->Collection = content;
    self->IsCollection = TRUE;
}

// 0x100061a0
VOID CLASSCALL SelectBinAssetItem(BINASSETPTR self, CONST U32 indx, LPVOID value)
{
    self->Collection->Items[indx] = value;
}

// 0x10001780
VOID AdjustBinAssetImage(IMAGESPRITEPTR self, CONST U32 ro, CONST U32 rm, CONST U32 go, CONST U32 gm, CONST U32 bo, CONST U32 bm)
{
    if (ro == 0 && rm == 0xF800 && go == 5 && gm == 0x7E0 && bo == 11 && bm == 0x1F) { return; }
    else if (rm == 0x7C00 && go == 6 && gm == 0x3E0 && bo == 11 && bm == 0x1F)
    {
        // TODO make the loop pretty

        U16* next = &self->Next;
        IMAGESPRITEPIXELPTR pixels = self->Pixels;

        for (U32 x = 0; x < self->Height; x++)
        {
            CONST U32 count = (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                ? 1 : (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

            for (U32 xx = 0; xx < count; xx++)
            {
                CONST PIXEL color = pixels->Pixels[xx];

                if (color != MAGENTA_PIXEL)
                {
                    pixels->Pixels[xx] = ((color & 0x1F) | (color >> 1)) & 0xFFE0; // TODO
                }
            }

            CONST U32 offset = next[0];
            next = (U16*)(ADDR(next) + offset + sizeof(U16));
            pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + offset);
        }
    }
    else
    {
        // TODO offsets
        S32 DAT_10046090 = go - 5; // TODO Name
        S32 DAT_100460a0 = bo - 11; // TODO Name
        S32 DAT_10046098 = 0; // TODO Name

        // TODO masks
        U32 DAT_1004609c = rm; // TODO Name
        U32 DAT_10046094 = gm; // TODO Name
        U32 DAT_100460a4 = bm; // TODO Name

        if (DAT_10046090 < 0) { DAT_10046090 = go + 11; }

        if (DAT_100460a0 < 0) { DAT_100460a0 = bo + 5; }

        // TODO make the loop pretty

        U16* next = &self->Next;
        IMAGESPRITEPIXELPTR pixels = self->Pixels;

        for (U32 x = 0; x < self->Height; x++)
        {
            CONST U32 count = (pixels->Count & IMAGESPRITE_ITEM_COMPACT_MASK)
                ? 1 : (pixels->Count & IMAGESPRITE_ITEM_COUNT_MASK);

            for (U32 xx = 0; xx < count; xx++)
            {
                CONST PIXEL color = pixels->Pixels[xx];
                if (color != MAGENTA_PIXEL)
                {
                    // TODO
                    pixels->Pixels[xx] =
                        (color >> ((byte)DAT_10046098 & 0xF) | color << 0x10 - ((byte)DAT_10046098 & 0xF)) & DAT_1004609c
                        | (color >> ((byte)DAT_10046090 & 0xF) | color << 0x10 - ((byte)DAT_10046090 & 0xF)) & DAT_10046094
                        | (color >> ((byte)DAT_100460a0 & 0xF) | color << 0x10 - ((byte)DAT_100460a0 & 0xF)) & DAT_100460a4;
                }
            }

            CONST U32 offset = next[0];
            next = (U16*)(ADDR(next) + offset + sizeof(U16));
            pixels = (IMAGESPRITEPIXELPTR)((ADDR)pixels + offset);
        }
    }
}