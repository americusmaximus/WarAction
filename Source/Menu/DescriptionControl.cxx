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

#include "DescriptionControl.hxx"
#include "Graphics.hxx"
#include "Settings.hxx"
#include "State.hxx"

#include <../Text/Resources.hxx>

#include <malloc.h>
#include <stdlib.h>

#define DEFAULT_MIN_DELAY           50
#define DEFAULT_MED_DELAY           250
#define DEFAULT_MAX_DELAY           5000

#define MAX_DESCRIPTION_CONTROL_TICK_TEXT_LENGTH    4096

typedef struct DescriptionControlTick
{
    U32     Unk0; // TODO
    LPSTR   Unk4; // TODO
    U32     Unk8; // TODO
    S32     Width;
    CHAR    Text[MAX_DESCRIPTION_CONTROL_TICK_TEXT_LENGTH];
} DESCRIPTIONCONTROLTICK, * DESCRIPTIONCONTROLTICKPTR;

// 0x1003a568
DESCRIPTIONCONTROLSELF DescriptionControlSelfState =
{
    (DESCRIPTIONCONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeDescriptionControl,
    DisableDescriptionControl,
    TickDescriptionControl,
    ActionDescriptionControl,
    ReleaseDescriptionControl
};

// 0x1000e2a0
DESCRIPTIONCONTROLPTR CLASSCALL ActivateDescriptionControl(DESCRIPTIONCONTROLPTR self, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, FONTASSETPTR font, U32 param_7, U32 param_8)
{
    self->Self = &DescriptionControlSelfState;

    self->IsActive = FALSE;

    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;

    self->Font = font;

    self->Unk06 = param_7; // TODO
    self->Unk07 = param_8; // TODO
    self->Unk08 = 0; // TODO
    self->Text = NULL;
    self->Color = WHITE_COLOR;

    STRINGVALUE name;
    AcquireSettingsValue(&name, IDS_BRIEF_DELAY_1);
    self->MinDelay = AcquireSettingsValue(name, DEFAULT_MIN_DELAY);

    AcquireSettingsValue(&name, IDS_BRIEF_DELAY_2);
    self->MidDelay = AcquireSettingsValue(name, DEFAULT_MED_DELAY);

    AcquireSettingsValue(&name, IDS_BRIEF_DELAY_3);
    self->MaxDelay = AcquireSettingsValue(name, DEFAULT_MAX_DELAY);

    return self;
}

// 0x1000e460
VOID CLASSCALL InitializeDescriptionControl(DESCRIPTIONCONTROLPTR self)
{
    if (self->Text == NULL) { SelectDescriptionControlText(self, StringsState.Scratch); }

    if (self->Color == WHITE_COLOR) { self->Color = ADJUSTCOLOR(0xFF00); }

    self->LineCount = 0;
    self->LineLength = 0;
    self->Unk09 = 0; // TODO
    self->Unk19 = 0; // TODO
    self->Ticks = GetTickCount();
    self->End = self->Text;
}

// 0x1000e500
VOID CLASSCALL DisableDescriptionControl(DESCRIPTIONCONTROLPTR self)
{
    if (self->Text != NULL)
    {
        free(self->Text);

        self->Text = NULL;
    }
}

// 0x1000e610
VOID CLASSCALL TickDescriptionControl(DESCRIPTIONCONTROLPTR self)
{
    DESCRIPTIONCONTROLTICKPTR tick = (DESCRIPTIONCONTROLTICKPTR)alloca(sizeof(DESCRIPTIONCONTROLTICK));

    tick->Unk4 = 0; // TODO

    self->LineLength = 0; // TODO
    self->LineCount = 0; // TODO

    State.Renderer->Window.X = self->X;
    State.Renderer->Window.Y = self->Y;
    State.Renderer->Window.Width = self->X + self->Width - 1;
    State.Renderer->Window.Height = self->Y + self->Height - 1;

    SelectFontAssetColor(self->Font, self->Color);

    BOOL uVar8 = FALSE; // TODO name
    LPSTR text = self->Text;

    while (AcquireUnicode(text) != NULL && !uVar8)
    {
        S32 width = 0; // TODO optimize out?
        LPSTR next = tick->Text;

        tick->Width = 0;
        tick->Unk4 = next;

        // Calculate the width of the string in pixels that will fit into the control's width
        while (' ' < AcquireUnicode(text) && width < self->Width)
        {
            if (AcquireUnicode(text) != '~')
            {
                CopyMemory(next, text, AcquireAnsiLength(text));

                width = width + AcquireFontAssetItemWidth(self->Font, AcquireUnicode(text));

                tick->Width = width;
                next = tick->Unk4;
            }

            if (self->End <= text)
            {
                next[0] = NULL;
                uVar8 = TRUE; // TODO
            }

            if (AcquireUnicode(text) != '~')
            {
                next = AcquireNextString(next);
                tick->Unk4 = next;
            }

            text = AcquireNextString(text);
        }

        // Draw the text that fits into the width, if any
        next[0] = NULL;
        if (tick->Text[0] != NULL)
        {
            tick->Unk8 = 0; // TODO
            DrawDescriptionControlText(self, tick->Text, width);
        }

        UNICHAR item = NULL;
        uVar8 = uVar8 | (text == self->End); // TODO ???

        // Skip the drawn text, if needed, and draw line termination characters
        if (!uVar8 && (item = AcquireUnicode(text)) != NULL)
        {
            if (item == '\n')
            {
                if (tick->Unk8 == 0) { item = ' '; } else { item = '\n'; } // TODO

                DrawDescriptionControlCharacter(self, item);
                tick->Unk8 = 1; // TODO
            }
            else if (item != '\r' && item != '~') { DrawDescriptionControlCharacter(self, item); }

            text = AcquireNextString(text);
        }
    }

    State.Renderer->Window.X = 0;
    State.Renderer->Window.Y = 0;
    State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
    State.Renderer->Window.Height = GRAPHICS_RESOLUTION_480 - 1;
}

// 0x1000e840
U32 CLASSCALL ActionDescriptionControl(DESCRIPTIONCONTROLPTR self)
{
    CONST DWORD ticks = GetTickCount();

    if (self->Unk19 == 0) // TODO
    {
        CONST S32 h = AcquireFontAssetHeight(self->Font);
        CONST S32 height = (h + self->Unk08) * (self->LineCount + 1) + self->Unk07 + self->Y + self->Unk09;

        if (self->Height + self->Y - 1 < height)
        {
            self->Unk09 = self->Unk09 - 1;
            self->Ticks = GetTickCount();

            return CONTROLACTION_NONE;
        }

        if (self->MinDelay + self->Ticks < ticks)
        {
            if (self->End[0] == NULL)
            {
                self->Self->Initialize(self);

                return CONTROLACTION_NONE;
            }

            LPSTR ch = NULL;
            self->End = *AcquireNextStringValue(&self->End, &ch, 1);
            self->Ticks = ticks;

            if (self->End[0] == '~')
            {
                self->Ticks = self->MidDelay + ticks;
                return CONTROLACTION_NONE;
            }

            if (self->End[0] == NULL) { self->Ticks = self->MaxDelay + ticks; }
        }
    }

    return CONTROLACTION_NONE;
}

// 0x1000e390
DESCRIPTIONCONTROLPTR CLASSCALL ReleaseDescriptionControl(DESCRIPTIONCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeDescriptionControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x1000e3b0
VOID CLASSCALL DisposeDescriptionControl(DESCRIPTIONCONTROLPTR self)
{
    self->Self = &DescriptionControlSelfState;

    if (self->Text != NULL) { free(self->Text); }

    self->Self = (DESCRIPTIONCONTROLSELFPTR)&ControlSelfState;
}

// 0x1000e410
VOID CLASSCALL SelectDescriptionControlText(DESCRIPTIONCONTROLPTR self, LPCSTR text)
{
    if (self->Text != NULL) { free(self->Text); }

    self->Text = (LPSTR)malloc(strlen(text) + 1);

    strcpy(self->Text, text);
}

// 0x1000e5a0
VOID CLASSCALL DrawDescriptionControlText(DESCRIPTIONCONTROLPTR self, LPSTR text, CONST U32 width)
{
    if (self->Width < self->Unk06 + self->LineLength + width && self->LineLength != 0)
    {
        DrawDescriptionControlCharacter(self, '\n');
    }

    UNICHAR item = NULL;
    LPSTR value = text;

    while ((item = AcquireUnicode(value)) != NULL)
    {
        value = AcquireNextString(text);
        DrawDescriptionControlCharacter(self, item);
    }
}

// 0x1000e520
VOID CLASSCALL DrawDescriptionControlCharacter(DESCRIPTIONCONTROLPTR self, CONST UNICHAR value) // TODO better name
{
    if (value == '\n')
    {
        self->LineLength = 0;
        self->LineCount = self->LineCount + 1;
    }

    CONST S32 height = AcquireFontAssetHeight(self->Font);
    CONST S32 offset = (height + self->Unk08) * self->LineCount + self->Unk09 + self->Unk07 + self->Y;

    if (self->Y <= height + offset)
    {
        DrawFontAssetItem(self->Font, self->LineLength + self->Unk06 + self->X, offset, value);
    }

    self->LineLength = self->LineLength + AcquireFontAssetItemWidth(self->Font, value);
}

// 0x1000e910
VOID CLASSCALL FUN_1000e910(DESCRIPTIONCONTROLPTR self) // TODO name
{
    if (self->Unk19 == 0) { self->Unk19 = GetTickCount(); }
    else
    {
        self->Ticks = self->Ticks + (GetTickCount() - self->Unk19);
        self->Unk19 = 0;
    }
}