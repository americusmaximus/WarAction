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

#include "Strings.hxx"

#include <FontAsset.hxx>

BOOL CLASSCALL InitializeFontAsset(FONTASSETPTR self, LPCSTR name, CONST FONTTYPE type);
FONTASSETPTR CLASSCALL ActivateFontAsset(FONTASSETPTR self);
S32 CLASSCALL AcquireFontAssetHeight(FONTASSETPTR self);
U32 CLASSCALL AcquireFontAssetItemHeight(FONTASSETPTR self, CONST UNICHAR value);
U32 CLASSCALL AcquireFontAssetItemWidth(FONTASSETPTR self, CONST UNICHAR value);
U32 CLASSCALL AcquireFontAssetTextWidth(FONTASSETPTR self, LPCSTR text);
VOID CLASSCALL DisposeFontAsset(FONTASSETPTR self);
VOID CLASSCALL DrawFontAssetItem(FONTASSETPTR self, CONST U32 x, CONST U32 y, CONST UNICHAR value);
VOID CLASSCALL DrawFontAssetText(FONTASSETPTR self, CONST U32 x, CONST U32 y, LPCSTR text);
VOID CLASSCALL DrawFontAssetText(FONTASSETPTR self, CONST U32 x, CONST U32 y, LPCSTR text, CONST U32 alignment);
VOID CLASSCALL InitializeFontAsset(FONTASSETPTR self);
VOID CLASSCALL SelectFontAssetColor(FONTASSETPTR self, CONST U32 color);
VOID CLASSCALL SelectFontAssetColor(FONTASSETPTR self, CONST U32 r, CONST U32 g, CONST U32 b);

IMAGEPALETTESPRITEPTR AcquireFontAssetItem(LPCVOID content, CONST U32 indx);
S32 AcquireFontAssetHeight(LPCVOID content);
U32 AcquireFontAssetTextWidth(LPCSTR text, LPCVOID content, CONST U32 spacing);
VOID DrawFontAssetText(CONST U32 x, CONST U32 y, LPCSTR text, LPCVOID asset, PIXEL* pixels, CONST U32 alignment, CONST U32 spacing);
U32 AcquireFontAssetItemWidth(CONST U32 x, CONST U32 width, CONST U32 alignment);