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

#include "Native.Basic.hxx"
#include "Renderer.Basic.hxx"

#define IMAGESPRITE_ITEM_COUNT_MASK     0x7F
#define IMAGESPRITE_ITEM_COMPACT_MASK   0x80

#pragma pack(push, 1)
typedef struct BinAsset
{
    LPVOID  Content;
    BOOL    IsIndexed;
    LPCSTR  Name;
    U8      IsImage;
} BINASSET, * BINASSETPTR;
#pragma pack(pop)

typedef struct BinAssetHeader
{
    U32 Offset[1];
} BINASSETHEADER, * BINASSETHEADERPTR;

#pragma pack(push, 1)
typedef struct ImageSpritePixel
{
    U8      Count;
    PIXEL   Pixels[1];
} IMAGESPRITEPIXEL, * IMAGESPRITEPIXELPTR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ImageSprite
{
    S16                 X;
    S16                 Y;
    U16                 Width;
    U16                 Height;
    U8                  Unk04; // TODO
    U16                 Next;
    IMAGESPRITEPIXEL    Pixels[1];
} IMAGESPRITE, * IMAGESPRITEPTR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ImagePaletteSpritePixel
{
    U8      Count;
    U8      Pixels[1];
} IMAGEPALETTESPRITEPIXEL, * IMAGEPALETTESPRITEPIXELPTR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ImagePaletteSprite
{
    S16                     X;
    S16                     Y;
    U16                     Width;
    U16                     Height;
    U8                      Unk04; // TODO
    U16                     Next;
    IMAGEPALETTESPRITEPIXEL Pixels[1];
} IMAGEPALETTESPRITE, * IMAGEPALETTESPRITEPTR;
#pragma pack(pop)