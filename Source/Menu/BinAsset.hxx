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

#include "Basic.hxx"

#include <BinAsset.hxx>

BINASSETPTR CLASSCALL ActivateBinAsset(BINASSETPTR self);
BOOL CLASSCALL InitializeBinAsset(BINASSETPTR self, LPCSTR name, CONST BOOL load);
U32 CLASSCALL AcquireBinAssetContent(BINASSETPTR self, CONST U32 indx);
U32 CLASSCALL AcquireBinAssetImageCount(BINASSETPTR self);
VOID CLASSCALL AdjustBinAssetImage(BINASSETPTR self);
VOID CLASSCALL AsStringBinAsset(BINASSETPTR self);
VOID CLASSCALL DisposeBinAsset(BINASSETPTR self);
VOID CLASSCALL InitializeBinAsset(BINASSETPTR self, CONST U32 count);
VOID CLASSCALL SelectBinAssetItem(BINASSETPTR self, CONST U32 indx, CONST U32 value);

VOID AdjustBinAssetImage(IMAGESPRITEPTR self, CONST U32 ro, CONST U32 rm, CONST U32 go, CONST U32 gm, CONST U32 bo, CONST U32 bm);