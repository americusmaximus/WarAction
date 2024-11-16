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

#include "Compression.hxx"

#include <zlib.h>

// 0x10025780
S32 UnZip(LPVOID dst, U32* dstlen, LPVOID src, U32 srclen)
{
    z_stream stream;
    ZeroMemory(&stream, sizeof(z_stream));

    stream.avail_in = srclen;
    stream.next_in = (BYTE*)src;

    stream.avail_out = *dstlen;
    stream.next_out = (BYTE*)dst;

    S32 result = inflateInit(&stream);

    if (result == Z_OK)
    {
        CONST S32 code = inflate(&stream, Z_FINISH);

        if (code == Z_STREAM_END)
        {
            *dstlen = stream.total_out;
            result = inflateEnd(&stream);
        }
        else
        {
            inflateEnd(&stream);

            result = Z_BUF_ERROR;

            if (code != Z_OK) { return code; }
        }
    }

    return result;
}