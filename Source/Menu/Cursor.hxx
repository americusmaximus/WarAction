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

#include <Renderer.Basic.hxx>

typedef struct Cursor
{
    ANIMATIONSPRITEHEADERPTR    Animation;
    PIXEL*                      Palette;
    U32                         Width;
    U32                         Height;
    PIXEL*                      Pixels;
} CURSOR, * CURSORPTR;

typedef struct CursorStateModuleContainer
{
    CURSOR  Cursor; // 0x10059ad8

    S32     X;      // 0x10059bbc
    S32     Y;      // 0x10059bc0
    BOOL    IsLeft; // 0x10059bc4
    BOOL    IsRight;// 0x10059bc8

    U32     AnimationIndex; // 0x10046ca8 // TODO Name
    // TODO ^^ this is briefing related (like line number), nothing related to cursor ^^
} CURSORSTATEMODULECONTAINER, * CURSORSTATEMODULECONTAINERPTR;

EXTERN CURSORSTATEMODULECONTAINER CursorState;

CURSORPTR CLASSCALL ActivateCursor(CURSORPTR self);
VOID CLASSCALL DisposeCursor(CURSORPTR self);
VOID CLASSCALL InitializeCursorState(CURSORPTR self, LPCSTR name);

VOID SelectCursorCoordinates(CONST S32 ox, CONST S32 oy, CONST S32 nx, CONST S32 ny);
VOID CursorMessageHandler(CONST U32 action);

VOID CLASSCALL CursorUnknown1(CURSORPTR self); // TODO name
VOID CLASSCALL DrawCursor(CURSORPTR self);
VOID CLASSCALL CursorUnknown3(CURSORPTR self); // TODO name

VOID FUN_10003610(PIXEL* dst, CONST S32 dw, CONST S32 dh, PIXEL* src, CONST S32 sw, CONST S32 sh, CONST S32 ox, CONST S32 oy, CONST S32 x, CONST S32 y, CONST S32 w, CONST S32 h); // TODO name
VOID FUN_10003590(CONST S32 minX, CONST S32 minY, CONST S32 maxX, CONST S32 maxY, CONST S32 width, CONST S32 height, S32* outX, S32* outY, S32* outHeight, S32* outWidth); // TODO name