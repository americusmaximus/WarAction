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

#include "ActionArea.hxx"
#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "Graphics.hxx"
#include "Objects.hxx"
#include "State.hxx"

#include <Mathematics.Basic.hxx>

#include <stdlib.h>

#define CURSOR_LEVEL            100

#define MAX_CURSOR_NAME_LENGTH  100

CURSORSTATEMODULECONTAINER CursorState;

// 0x10003410
CURSORPTR CLASSCALL ActivateCursor(CURSORPTR self)
{
    self->Animation = NULL;
    self->Palette = NULL;
    self->Pixels = NULL;

    return self;
}

// 0x10003420
VOID CLASSCALL DisposeCursor(CURSORPTR self)
{
    if (self->Animation != NULL) { free(self->Animation); }

    if (self->Palette != NULL) { free(self->Palette); }

    free(self->Pixels);
}

// 0x10003450
VOID CLASSCALL InitializeCursorState(CURSORPTR self, LPCSTR name)
{
    CHAR path[MAX_CURSOR_NAME_LENGTH];

    // Palette
    {
        wsprintfA(path, "%s.col", name);
        CONST U32 length = AcquireAssetContent(path, (LPVOID*)&self->Palette, 0);

        if (length != 0)
        {
            CONST U32 count = ((length - 1) >> 2) + 1;

            for (U32 x = 0; x < count; x++) { self->Palette[x] = ADJUSTSPRITECOLOR(self->Palette[x]); }
        }
    }

    // Animation
    {
        wsprintfA(path, "%s.ani", name);
        AcquireAssetContent(path, (LPVOID*)&self->Animation, 0);

        CONST IMAGEPALETTESPRITEPTR image =
            (IMAGEPALETTESPRITEPTR)((ADDR)self->Animation + self->Animation->Offsets[0]);

        self->Width = image->Width;
        self->Height = image->Height;

        self->Pixels = (PIXEL*)malloc(image->Width * image->Height * sizeof(PIXEL));
    }
}

// 0x100226f0
VOID CursorMessageHandler(CONST U32 action)
{
    for (ACTIONAREAPTR area = ActionAreaState.Items; area != NULL; area = area->Next)
    {
        if (!(area->Options & CONTROLCOMMANDACTION_IGNORE)
            && area->X <= CursorState.X && area->Y <= CursorState.Y
            && CursorState.X < area->Width + area->X && CursorState.Y < area->Height + area->Y
            && (action & area->Options))
        {
            EnqueueControlCommand(area->Action, action, CursorState.X - area->X, CursorState.Y - area->Y);

            if (area->Options & CONTROLCOMMANDACTION_SCROLL) { return; }
        }
    }
}

// 0x10003740
VOID CLASSCALL SaveCursorPixels(CURSORPTR self)
{
    IMAGEPALETTESPRITEPTR image =
        (IMAGEPALETTESPRITEPTR)((ADDR)self->Animation + self->Animation->Offsets[0]);

    CopyCursorPixels(self->Pixels, self->Width, self->Height,
        State.Renderer->Surface.Main, State.Renderer->Surface.Width, State.Renderer->Surface.Height,
        0, 0, image->X + CursorState.X, image->Y + CursorState.Y, image->Width, image->Height);
}

// 0x10003540
VOID CLASSCALL DrawCursor(CURSORPTR self)
{
    CONST U32 indx = (GetTickCount() / 50) % self->Animation->Count;

    IMAGEPALETTESPRITEPTR image =
        (IMAGEPALETTESPRITEPTR)((ADDR)self->Animation + self->Animation->Offsets[indx]);

    State.Renderer->Actions.DrawMainSurfaceAnimationSpriteStencil(
        CursorState.X, CursorState.Y, CURSOR_LEVEL, (ANIMATIONPIXEL*)self->Palette, image);
}

// 0x100037a0
VOID CLASSCALL RestoreCursorPixels(CURSORPTR self)
{
    IMAGEPALETTESPRITEPTR image =
        (IMAGEPALETTESPRITEPTR)((ADDR)self->Animation + self->Animation->Offsets[0]);

    CopyCursorPixels(State.Renderer->Surface.Main, State.Renderer->Surface.Width, State.Renderer->Surface.Height,
        self->Pixels, self->Width, self->Height,
        image->X + CursorState.X, image->Y + CursorState.Y, 0, 0, image->Width, image->Height);
}

// 0x10003610
VOID CopyCursorPixels(PIXEL* dst, CONST S32 dw, CONST S32 dh, PIXEL* src, CONST S32 sw, CONST S32 sh, CONST S32 ox, CONST S32 oy, CONST S32 x, CONST S32 y, CONST S32 w, CONST S32 h)
{
    S32 srcx, srcy, srch, srcw;
    AcquireCursorCoordinates(x, y, w, h, sw, sh, &srcx, &srcy, &srch, &srcw);

    CONST S32 offx = ox + srcx;
    CONST S32 offy = oy + srcy;

    CONST S32 ax = x + srcx;
    CONST S32 ay = y + srcy;

    S32 dstx, dsty, dsth, dstw;
    AcquireCursorCoordinates(offx, offy, w + srcw, h + srch, dw, dh, &dstx, &dsty, &dsth, &dstw);

    CONST S32 height = h + srch + dsth;
    CONST S32 width = w + srcw + dstw;

    if (width != 0 && height != 0)
    {
        PIXEL* destination = dst + (dsty + offy) * dw + dstx + offx;
        PIXEL* source = src + (dsty + ay) * sw + dstx + ax;

        for (U32 xx = 0; xx < height; xx++)
        {
            CopyMemory(destination, source, width * sizeof(PIXEL));

            destination = destination + dw;
            source = source + sw;
        }
    }
}

// 0x10003590
VOID AcquireCursorCoordinates(CONST S32 minX, CONST S32 minY, CONST S32 maxX, CONST S32 maxY, CONST S32 width, CONST S32 height, S32* outX, S32* outY, S32* outHeight, S32* outWidth)
{
    CONST S32 x = Mathematics::Max<S32>(0, minX);
    CONST S32 y = Mathematics::Max<S32>(0, minY);

    S32 w = maxX + minX;

    if (width < maxX + minX) { w = width; }

    S32 h = maxY + minY;

    if (height < maxY + minY) { h = height; }

    w = w - x;
    h = h - y;

    if (w < 0) { w = 0; }
    if (h < 0) { h = 0; }

    *outX = x - minX;
    *outY = y - minY;

    *outHeight = h - maxY;
    *outWidth = w - maxX;
}

// 0x10022780
VOID SelectCursorCoordinates(CONST S32 ox, CONST S32 oy, CONST S32 nx, CONST S32 ny)
{
    for (ACTIONAREAPTR area = ActionAreaState.Items; area != NULL; area = area->Next)
    {
        if (!(area->Options & CONTROLCOMMANDACTION_IGNORE)
            && (nx < area->X || ny < area->Y || area->Width + area->X <= nx || area->Height + area->Y <= ny)
            && area->X <= ox && area->Y <= oy && ox < area->Width + area->X && oy < area->Height + area->Y
            && (area->Options & CONTROLCOMMANDACTION_MOUSE_LEAVE))
        {
            EnqueueControlCommand(area->Action, CONTROLCOMMANDACTION_MOUSE_LEAVE, nx - area->X, ny - area->Y);
        }
    }

    BOOL accept = TRUE;
    for (ACTIONAREAPTR area = ActionAreaState.Items; area != NULL; area = area->Next)
    {
        if (!(area->Options & CONTROLCOMMANDACTION_IGNORE)
            && area->X <= nx && area->Y <= ny && nx < area->Width + area->X && ny < area->Height + area->Y)
        {
            if ((area->Options & CONTROLCOMMANDACTION_MOUSE_ENTER)
                && (ox < area->X || oy < area->Y || area->Width + area->X <= ox || area->Height + area->Y <= oy))
            {
                EnqueueControlCommand(area->Action, CONTROLCOMMANDACTION_MOUSE_ENTER, nx - area->X, ny - area->Y);
            }

            if (accept && (area->Options & CONTROLCOMMANDACTION_MOUSE_HOVER))
            {
                EnqueueControlCommand(area->Action, CONTROLCOMMANDACTION_MOUSE_HOVER, nx - area->X, ny - area->Y);

                if (area->Options & CONTROLCOMMANDACTION_SCROLL) { accept = FALSE; }
            }
        }
    }
}
