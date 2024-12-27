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

#include <BinFile.hxx>

BOOL CLASSCALL OpenBinFile(BINFILEPTR self, LPCSTR name, CONST U32 type);
U32 CLASSCALL AcquireBinFilePosition(BINFILEPTR self);
U32 CLASSCALL AcquireBinFileSize(BINFILEPTR self);
U32 CLASSCALL PointBinFile(BINFILEPTR self, CONST LONG distance, CONST DWORD method);
U32 CLASSCALL ReadBinFile(BINFILEPTR self, LPVOID content, CONST U32 size);
U32 CLASSCALL WriteBinFile(BINFILEPTR self, LPCVOID content, CONST U32 size);
VOID CLASSCALL CloseBinFile(BINFILEPTR self);

VOID AcquireBinFile(BINFILEINFOPTR self, CONST U32 archive, LPSTR names, CONST BOOL overwrite);
VOID AcquireBinFile(LPSTR name, CONST U32 archive, CONST BOOL overwrite);

VOID CopyBinFile(BINFILEPTR src, BINFILEPTR dst, CONST U32 size);