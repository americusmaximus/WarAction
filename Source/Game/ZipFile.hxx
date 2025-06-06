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

#include "BinFile.hxx"

#include <zlib.h>

#define ZIPFILE_OPEN_READ               0 /* rb */
#define ZIPFILE_OPEN_READ_MODIFY        1 /* r+b */
#define ZIPFILE_OPEN_WRITE              2 /* wb */
#define ZIPFILE_OPEN_WRITE_MODIFY       3 /* w+b */

typedef struct ZipFile
{
    gzFile  File;
    z_off_t Offset;
    S32     Unk02; // TODO
} ZIPFILE, * ZIPFILEPTR;

// NOTE. The original hame uses zlib 1.1.3 where the type z_off_t is defined as long.
// Since then, zlib updated the type z_off_t to long long, thus breaking excplicit backward compatibility.

BOOL CLASSCALL OpenZipFile(ZIPFILEPTR self, LPCSTR name, CONST U32 mode);
VOID CLASSCALL CloseZipFile(ZIPFILEPTR self);
VOID CLASSCALL WriteZipFile(ZIPFILEPTR self, LPVOID value, CONST U32 size);
VOID CLASSCALL SelectZipFileCompression(ZIPFILEPTR self, CONST U32 level);