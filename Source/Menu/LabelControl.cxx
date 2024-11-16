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
#include "Graphics.hxx"
#include "LabelControl.hxx"
#include "State.hxx"

#include <stdlib.h>

// 0x1003a390
LABELCONTROLSELF LabelControlSelfState =
{
    (CONTROLTYPE0X29TYPEACTION)AcquireControlTypeControl,
    (CONTROLTYPE0X29INITIALIZEACTION)InitializeControl,
    (CONTROLTYPE0X29DISABLEACTION)DisableControl,
    TickLabelControl,
    (CONTROLTYPE0X29ACTIONACTION)ActionControl,
    ReleaseLabelControl
};

// 0x10005d50
LABELCONTROLPTR CLASSCALL ActivateLabelControl(LABELCONTROLPTR self, CONST U32 x, CONST U32 y, CONST U32 width, CONST U32 height, LPCSTR text)
{
    self->Self = &LabelControlSelfState;

    self->IsActive = FALSE;

    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;

    self->IsVisible = TRUE;

    self->Text = NULL;

    self->IsCentered = FALSE;

    SlectLabelControlText(self, text);

    self->Color = ADJUSTCOLOR(0xFF00);

    self->Font = &AssetsState.Fonts.Main;

    return self;
}

// 0x10005f50
VOID CLASSCALL TickLabelControl(LABELCONTROLPTR self)
{
    if (self->Text == NULL || !self->IsVisible) { return; }

    State.Renderer->Window.X = self->X;
    State.Renderer->Window.Y = self->Y;
    State.Renderer->Window.Width = self->X - 1 + self->Width;
    State.Renderer->Window.Height = self->Y - 1 + self->Height;

    U32 x = 0;
    U32 y = 0;
    CONST U32 width = AcquireFontAssetTextWidth(self->Font, self->Text);
    CONST U32 height = AcquireFontAssetHeight(self->Font);

    if (width < self->Width && self->IsCentered) { x = (self->Width - width) / 2; }
    if (self->Height < height || self->IsCentered) { y = (self->Height - height) / 2; }

    SelectFontAssetColor(self->Font, self->Color);
    DrawFontAssetText(self->Font, self->X + x, self->Y + y, self->Text, FONTALIGNMENT_LEFT);

    State.Renderer->Window.X = 0;
    State.Renderer->Window.Y = 0;
    State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
    State.Renderer->Window.Height = GRAPHICS_RESOLUTION_480 - 1;
}

// 0x10005e30
LABELCONTROLPTR CLASSCALL ReleaseLabelControl(LABELCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeLabelControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10005e50
VOID CLASSCALL DisposeLabelControl(LABELCONTROLPTR self)
{
    self->Self = (LABELCONTROLSELFPTR)&ControlSelfState;

    if (self->Text != NULL) { free(self->Text); }
}

// 0x10005eb0
VOID CLASSCALL SlectLabelControlText(LABELCONTROLPTR self, LPCSTR text)
{
    if (text != NULL && self->Text != NULL) { if (strcmp(self->Text, text) == 0) { return; } }

    if (self->Text != NULL) { free(self->Text); }

    if (text == NULL) { self->Text = NULL; return; }

    self->Text = (LPSTR)malloc(strlen(text) + 1);

    strcpy(self->Text, text);
}