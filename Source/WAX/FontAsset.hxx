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

#pragma once

#include "BinAsset.hxx"

#define FONTALIGNMENT_LEFT              (-1)
#define FONTALIGNMENT_CENTER            0
#define FONTALIGNMENT_RIGHT             1

#define COMPLEXFONTALIGNMENT_LEFT       0
#define COMPLEXFONTALIGNMENT_RIGHT      1
#define COMPLEXFONTALIGNMENT_CENTER     2

#define MAX_FONT_ASSET_CHARACTER_COUNT  256

typedef enum FontType
{
    FONTTYPE_BASIC          = 0,
    FONTTYPE_COMPLEX        = 1,
    FONTTYPE_FORCE_DWORD    = 0x7FFFFFF
} FONTTYPE, * FONTTYPEPTR;

typedef struct FontAsset
{
    FONTTYPE    Type;
    BINASSET    Asset;
    U32         Color;
    PIXEL*      Palette;
    LPVOID      Font; // TODO Type
    PIXEL       Pixels[MAX_FONT_ASSET_CHARACTER_COUNT];
    U32         Height;
    U32         Offset;
} FONTASSET, * FONTASSETPTR;
