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

typedef struct BinAssetContent
{
    U32 Offset[1];
} BINASSETCONTENT, * BINASSETCONTENTPTR;

typedef struct BinAssetCollectionContent
{
    U32 Offset;
    LPVOID Items[1];
} BINASSETCOLLECTIONCONTENT, * BINASSETCOLLECTIONCONTENTPTR;

#pragma pack(push, 1)
typedef struct BinAsset
{
    union
    {
        BINASSETCOLLECTIONCONTENTPTR    Collection;
        BINASSETCONTENTPTR              Content;
    };
    BOOL                                IsCollection;
    LPCSTR                              Name;
    U8                                  IsImage;
} BINASSET, * BINASSETPTR;
#pragma pack(pop)

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
    S16                 Width;
    S16                 Height;
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
    S16                     Width;
    S16                     Height;
    U8                      Unk04; // TODO
    U16                     Next;
    IMAGEPALETTESPRITEPIXEL Pixels[1];
} IMAGEPALETTESPRITE, * IMAGEPALETTESPRITEPTR;
#pragma pack(pop)

#define ANIMATION_VERSION_0             0
#define ANIMATION_VERSION_1             1
#define ANIMATION_VERSION_2             2
#define ANIMATION_VERSION_3             3
#define ANIMATION_VERSION_4             4
#define ANIMATION_VERSION_5             5 /* Bright Animation*/
#define ANIMATION_VERSION_6             6 /* 16 BPP Animation*/
#define ANIMATION_VERSION_7             7

#define ANIMATION_HEADER_MAGIC_MASK     0xFF000000
#define ANIMATION_HEADER_MAGIC_VALUE    0xCA000000

#define IS_ANIMATION(x)                 ((x & ANIMATION_HEADER_MAGIC_MASK) == ANIMATION_HEADER_MAGIC_VALUE)
#define IS_NOT_ANIMATION(x)             ((x & ANIMATION_HEADER_MAGIC_MASK) != ANIMATION_HEADER_MAGIC_VALUE)

#define IS_ANIMATION_HEADER_FORMAT(x)   (x & 0xFF)

#pragma pack(push, 1)
typedef struct AnimationSpriteHeader
{
    U32 Magic;
    U32 Count;
    U32 Offsets[1];
} ANIMATIONSPRITEHEADER, * ANIMATIONSPRITEHEADERPTR;
#pragma pack(pop)