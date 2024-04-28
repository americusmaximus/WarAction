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

#include "BitMap.hxx"

#include <stdio.h>
#include <stdlib.h>

typedef struct Color
{
    BYTE B;
    BYTE G;
    BYTE R;
} COLOR, * LPCOLOR;

BOOL SavePixels(LPCSTR name, CONST PIXEL* pixels, CONST U32 width, CONST U32 height)
{
    return SavePixels(name, pixels, width, height, width);
}

BOOL SavePixels(LPCSTR name, CONST PIXEL* pixels, CONST U32 width, CONST U32 height, CONST U32 stride)
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;

    CONST U32 bits = sizeof(COLOR) << 3;
    CONST U32 bistride = ((((width * bits) + 31) & ~31) >> 3);

    CONST U32 size = bistride * height;

    header.bfType = 0x4D42; // 'BM'
    header.bfSize = size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    header.bfReserved1 = header.bfReserved2 = 0;
    header.bfOffBits = header.bfSize - size;

    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biWidth = width;
    info.biHeight = height;
    info.biPlanes = 1;
    info.biBitCount = bits;
    info.biCompression = BI_RGB;
    info.biSizeImage = size;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    LPCOLOR colors = (LPCOLOR)malloc(size);

    if (colors == NULL) { return FALSE; }

    ZeroMemory(colors, size);

    for (U32 y = 0; y < height; y++)
    {
        LPCOLOR line = (LPCOLOR)((addr)colors + (addr)((height - y - 1) * bistride));

        for (U32 x = 0; x < width; x++)
        {
            CONST PIXEL pixel = pixels[y * stride + x];

#if ACTIVATE_COMPLETE_RGBA_MODE
            DebugBreak(); // TODO
#else
            line[x].R = (((U32)(pixel >> 11) & 0x1f) * 527 + 23) >> 6;
            line[x].G = (((U32)(pixel >> 5) & 0x3f) * 259 + 33) >> 6;
            line[x].B = (((U32)(pixel >> 0) & 0x1f) * 527 + 23) >> 6;
#endif
        }
    }

    FILE* f = NULL;

    if (fopen_s(&f, name, "wb") != 0) { return FALSE; }

    BOOL result = TRUE;

    result = fwrite(&header, 1, sizeof(BITMAPFILEHEADER), f) == sizeof(BITMAPFILEHEADER);
    result = fwrite(&info, 1, sizeof(BITMAPINFOHEADER), f) == sizeof(BITMAPINFOHEADER);
    result = fwrite(colors, 1, size, f) == size;
    result = fclose(f);

    free(colors);

    return result;
}