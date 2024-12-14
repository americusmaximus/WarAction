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

#include "Assets.hxx"
#include "BinAsset.hxx"
#include "Graphics.hxx"
#include "State.hxx"
#include "Strings.hxx"

#include <stdlib.h>

#define DEFAULT_FONT_ASSET_HEIGHT       0
#define DEFAULT_FONT_ASSET_SPACING      2
#define DEFAULT_FONT_ASSET_WIDTH        0

// 0x10020b60
FONTASSETPTR CLASSCALL ActivateFontAsset(FONTASSETPTR self)
{
    ActivateBinAsset(&self->Asset);

    self->Palette = NULL;
    self->Font = NULL;

    return self;
}

// 0x10003800
BOOL CLASSCALL InitializeFontAsset(FONTASSETPTR self, LPCSTR name, CONST FONTTYPE type)
{
    self->Type = type;

    switch (type)
    {
    case FONTTYPE_BASIC:
    {
        CHAR path[MAX_FILE_NAME_LENGTH];
        wsprintfA(path, "%s.pck", name);

        return InitializeBinAsset(&self->Asset, path, TRUE);
    }
    case FONTTYPE_COMPLEX:
    {
        CHAR path[MAX_FILE_NAME_LENGTH];
        wsprintfA(path, "%s.fnt", name);
        AcquireAssetContent(path, &self->Font, 0);

        wsprintfA(path, "%s.pl", name);
        AcquireAssetContent(path, (LPVOID*)&self->Palette, 0);

        return TRUE;
    }
    }

    return FALSE;
}

// 0x10003920
VOID CLASSCALL InitializeFontAsset(FONTASSETPTR self)
{
    self->Height = 0;
    self->Offset = 0;

    switch (self->Type)
    {
    case FONTTYPE_BASIC:
    {
        for (U32 x = 0; x < MAX_FONT_ASSET_CHARACTER_COUNT; x++)
        {
            CONST U32 value = AcquireFontAssetItemHeight(self, x);

            if (self->Height < value) { self->Height = value; }
        }

        break;
    }
    case FONTTYPE_COMPLEX:
    {
        self->Height = AcquireFontAssetHeight(self->Font);

        for (U32 x = 0; x < MAX_FONT_ASSET_CHARACTER_COUNT; x++)
        {
            self->Palette[x] = ADJUSTSPRITECOLOR(self->Palette[x]);
        }

        break;
    }
    }
}

// 0x100038c0
VOID CLASSCALL DisposeFontAsset(FONTASSETPTR self)
{
    if (self->Type == FONTTYPE_COMPLEX)
    {
        free(self->Font);
        free(self->Palette);
    }

    DisposeBinAsset(&self->Asset);
}

// 0x10003cb0
U32 CLASSCALL AcquireFontAssetItemHeight(FONTASSETPTR self, CONST UNICHAR item)
{
    switch (self->Type)
    {
    case FONTTYPE_BASIC: { return ((IMAGEPALETTESPRITEPTR)AcquireBinAssetContent(&self->Asset, item & 0xFF))->Height; }
    case FONTTYPE_COMPLEX: { return item == NULL ? self->Height : AcquireFontAssetItem(self->Font, item)->Height; }
    }

    return DEFAULT_FONT_ASSET_HEIGHT;
}

// 0x10023390
U32 AcquireFontAssetHeight(LPCVOID content)
{
    return ((U32*)content)[542] & 0xFF; // TODO ???
}

// 0x10003e60
U32 CLASSCALL AcquireFontAssetHeight(FONTASSETPTR self)
{
    return self->Height;
}

// 0x10003de0
U32 CLASSCALL AcquireFontAssetTextWidth(FONTASSETPTR self, LPCSTR text)
{
    switch (self->Type)
    {
    case FONTTYPE_BASIC:
    {
        U32 result = DEFAULT_FONT_ASSET_WIDTH;

        for (U32 x = 0; text[x] != NULL; x++)
        {
            result = result + AcquireFontAssetItemWidth(self, text[x]);
        }

        return result;
    }
    case FONTTYPE_COMPLEX: { return AcquireFontAssetTextWidth(text, self->Font, DEFAULT_FONT_ASSET_SPACING); }
    }

    return DEFAULT_FONT_ASSET_WIDTH;
}

// 0x10003c50
U32 CLASSCALL AcquireFontAssetItemWidth(FONTASSETPTR self, CONST UNICHAR item)
{
    switch (self->Type)
    {
    case FONTTYPE_BASIC: { return ((IMAGEPALETTESPRITEPTR)AcquireBinAssetContent(&self->Asset, item & 0xFF))->Width; }
    case FONTTYPE_COMPLEX:
    {
        return item == NULL
            ? DEFAULT_FONT_ASSET_SPACING : AcquireFontAssetItem(self->Font, item)->Width;
    }
    }

    return DEFAULT_FONT_ASSET_WIDTH;
}

// 0x10023070
IMAGEPALETTESPRITEPTR AcquireFontAssetItem(LPCVOID asset /* TODO */, CONST U32 indx) // TODO return type
{
    if (indx < 256) // TODO
    {
        CONST U32 offset = ((U32*)asset)[indx];// *sizeof(U32); // TODO

        return (IMAGEPALETTESPRITEPTR)((ADDR)asset + offset);
    }

    if (indx < 256 * 256) // TODO
    {
        U32 offset = ((U32*)asset)[(indx >> 8) + 255];// *sizeof(U32); // TODO

        offset = ((U32*)((ADDR)asset + offset))[indx & 0xFF];// * sizeof(U32);

        return (IMAGEPALETTESPRITEPTR)((ADDR)asset + offset);
    }

    U32 offset = ((U32*)asset)[(indx >> 16) + 256 + 255];// * sizeof(U32); // TODO

    offset = ((U32*)((ADDR)asset + offset))[(indx >> 8)];// * sizeof(U32); // TODO

    offset = ((U32*)((ADDR)asset + offset))[indx & 0xFF];// * sizeof(U32); // TODO

    return (IMAGEPALETTESPRITEPTR)((ADDR)asset + offset);
}

// 0x1001f6c0
U32 AcquireFontAssetTextWidth(LPCSTR text, LPCVOID content, CONST U32 spacing)
{
    U32 result = DEFAULT_FONT_ASSET_WIDTH;

    LPSTR value = (LPSTR)text;
    for (UNICHAR item = AcquireUnicode(value); item != NULL;
        value = AcquireNextString(value), item = AcquireUnicode(value))
    {
        IMAGEPALETTESPRITEPTR image = AcquireFontAssetItem(content, item);

        result = result + image->Width + spacing;
    }

    return result;
}

// 0x10003d10
VOID CLASSCALL DrawFontAssetText(FONTASSETPTR self, CONST U32 x, CONST U32 y, LPCSTR text, CONST U32 alignment)
{
    switch (self->Type)
    {
    case FONTTYPE_BASIC:
    {
        U32 actual = x;

        if (alignment != FONTALIGNMENT_LEFT)
        {
            CONST U32 width = AcquireFontAssetTextWidth(self, text);

            actual = alignment == FONTALIGNMENT_CENTER ? (x - width / 2) : (x - width);
        }

        U32 xx = 0;
        LPSTR value = (LPSTR)text;

        while (value[xx] != NULL)
        {
            DrawFontAssetItem(self, actual, y, value[xx]);

            actual = actual + AcquireFontAssetItemWidth(self, value[xx]); xx = xx + 1;
        }

        break;
    }
    case FONTTYPE_COMPLEX:
    {
        // 0x1003a33c
        CONST U32 alignments[] = { COMPLEXFONTALIGNMENT_LEFT, COMPLEXFONTALIGNMENT_CENTER, COMPLEXFONTALIGNMENT_RIGHT };

        DrawFontAssetText(x, y + (self->Height - self->Offset), text,
            self->Font, self->Pixels, alignments[alignment + 1], DEFAULT_FONT_ASSET_SPACING); break;
    }
    }
}

// 0x10003bc0
VOID CLASSCALL DrawFontAssetItem(FONTASSETPTR self, CONST U32 x, CONST U32 y, CONST UNICHAR item)
{
    switch (self->Type)
    {
    case FONTTYPE_BASIC:
    {
        State.Renderer->Actions.DrawMainSurfacePaletteSprite(x, y, self->Pixels,
            (IMAGEPALETTESPRITEPTR)AcquireBinAssetContent(&self->Asset, item & 0xFF)); break;
    }
    case FONTTYPE_COMPLEX:
    {
        State.Renderer->Actions.DrawMainSurfacePaletteSprite(x, y + self->Height - self->Offset,
            self->Pixels, AcquireFontAssetItem(self->Font, item)); break;
    }
    }
}

// 0x1001f720
VOID DrawFontAssetText(CONST U32 x, CONST U32 y, LPCSTR text, LPCVOID asset, PIXEL* pixels, CONST U32 alignment, CONST U32 spacing)
{
    U32 offset = AcquireFontAssetItemWidth(x, AcquireFontAssetTextWidth(text, asset, spacing), alignment);

    LPSTR value = (LPSTR)text;
    for (UNICHAR item = AcquireUnicode(value); item != NULL;
        value = AcquireNextString(value), item = AcquireUnicode(value))
    {
        IMAGEPALETTESPRITEPTR sprite = AcquireFontAssetItem(asset, item);

        State.Renderer->Actions.DrawMainSurfacePaletteSprite(offset, y, pixels, sprite);

        offset = offset + sprite->Width + spacing;
    }
}

// 0x1001f6a0
U32 AcquireFontAssetItemWidth(CONST U32 x, CONST U32 width, CONST U32 alignment)
{
    if (alignment == COMPLEXFONTALIGNMENT_LEFT) { return x; }

    return (alignment == COMPLEXFONTALIGNMENT_RIGHT) ? (x - width) : (x - width / 2);
}

// 0x10003e40
VOID CLASSCALL DrawFontAssetText(FONTASSETPTR self, CONST U32 x, CONST U32 y, LPCSTR text)
{
    DrawFontAssetText(self, x, y, text, COMPLEXFONTALIGNMENT_LEFT);
}

// 0x100039f0
VOID CLASSCALL SelectFontAssetColor(FONTASSETPTR self, CONST U32 r, CONST U32 g, CONST U32 b)
{
    SelectFontAssetColor(self, ADJUSTSPRITECOLORS(r, g, b));
}

// 0x10003a60
VOID CLASSCALL SelectFontAssetColor(FONTASSETPTR self, CONST U32 color)
{
    self->Color = color;

    switch (self->Type)
    {
    case FONTTYPE_BASIC: { self->Pixels[1] = (PIXEL)color; break; }
    case FONTTYPE_COMPLEX:
    {
        for (U32 x = 0; x < MAX_FONT_ASSET_CHARACTER_COUNT; x++)
        {
            self->Pixels[x] = ADJUSTCOLOR(color & self->Palette[x]);
        }
    }
    }
}