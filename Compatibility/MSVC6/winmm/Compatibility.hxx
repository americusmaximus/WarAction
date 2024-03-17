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

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

typedef DWORD DWORD_PTR;

#define strcat_s(dst, size, src) strcat(dst, src)
#define snprintf _snprintf

#define _fseeki64 fseek

#define MAKEMCIRESOURCE(wRet, wRes) MAKELRESULT((wRet), (wRes))

#define MCI_FALSE                       (MCI_STRING_OFFSET + 19)
#define MCI_TRUE                        (MCI_STRING_OFFSET + 20)

typedef VOID (TASKCALLBACK) (DWORD_PTR dwInst);
typedef TASKCALLBACK FAR *LPTASKCALLBACK;

typedef DWORD (WINAPI *DRIVERMSGPROC)(DWORD, DWORD, DWORD_PTR, DWORD_PTR, DWORD_PTR);