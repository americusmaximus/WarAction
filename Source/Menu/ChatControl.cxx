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

#include "ChatControl.hxx"
#include "State.hxx"

#include <stdlib.h>

// 0x1003a324
CHATCONTROLSELF ChatControlSelfState =
{
    (CHATCONTROLTYPEACTION)AcquireControlTypeList,
    InitializeChatControl,
    DisableChatControl,
    TickChatControl,
    ActionChatControl,
    ReleaseChatControl,
};

// 0x10003030
CHATCONTROLPTR CLASSCALL ActivateChatControl(CHATCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, SCROLLCONTROLPTR scroll, FONTASSETPTR font)
{
    self->Self = &ChatControlSelfState;

    self->IsActive = FALSE;
    self->Action = action;

    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;

    self->Scroll = scroll;
    self->Font = font;

    self->Messages = ActivateChatControlMessages(ALLOCATE(CHATCONTROLMESSAGES));

    return self;
}

// 0x10003160
VOID CLASSCALL InitializeChatControl(CHATCONTROLPTR self)
{
    ReleaseChatControlMessages(self->Messages);

    self->Scroll->Self->Initialize(self->Scroll);
}

// 0x10003180
VOID CLASSCALL DisableChatControl(CHATCONTROLPTR self)
{
    ReleaseChatControlMessages(self->Messages);

    self->Scroll->Self->Disable(self->Scroll);
}

// 0x100031f0
VOID CLASSCALL TickChatControl(CHATCONTROLPTR self)
{
    self->Scroll->Self->Tick(self->Scroll);

    State.Renderer->Window.X = self->X;
    State.Renderer->Window.Y = self->Y;
    State.Renderer->Window.Width = self->X + self->Width - 1;
    State.Renderer->Window.Height = self->Y + self->Height - 1;

    for (S32 x = 0; x < self->Scroll->Visible; x++)
    {
        CONST S32 indx = x + self->Scroll->Current;

        if (indx < self->Messages->Count)
        {
            SelectFontAssetColor(self->Font,self->Messages->Items[indx].Color);
            DrawFontAssetText(self->Font, self->X, x * AcquireFontAssetHeight(self->Font) + self->Y,
                self->Messages->Items[indx].Text, FONTALIGNMENT_LEFT);
        }
    }

    State.Renderer->Window.X = 0;
    State.Renderer->Window.Y = 0;
    State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
    State.Renderer->Window.Height = GRAPHICS_RESOLUTION_480 - 1;
}

// 0x10003400
U32 CLASSCALL ActionChatControl(CHATCONTROLPTR self)
{
    self->Scroll->Self->Action(self->Scroll);

    return CONTROLACTION_NONE;
}

// 0x100030e0
CHATCONTROLPTR CLASSCALL ReleaseChatControl(CHATCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeChatControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10003100
VOID CLASSCALL DisposeChatControl(CHATCONTROLPTR self)
{
    self->Self = (CHATCONTROLSELFPTR)&ControlSelfState;

    free(self->Messages);

    if (self->Scroll != NULL) { self->Scroll->Self->Release(self->Scroll, OBJECTRELEASETYPE_ALLOCATED); }
}

// 0x10002ee0
CHATCONTROLMESSAGESPTR CLASSCALL ActivateChatControlMessages(CHATCONTROLMESSAGESPTR self)
{
    for (U32 x = 0; x < MAX_CHAT_CONTROL_MESSAGE_COUNT; x++) { self->Items[x].Text = NULL; }

    self->Count = 0;

    return self;
}

// 0x10002ff0
VOID CLASSCALL ReleaseChatControlMessages(CHATCONTROLMESSAGESPTR self)
{
    for (U32 x = 0; x < MAX_CHAT_CONTROL_MESSAGE_COUNT; x++) { free(self->Items[x].Text); }

    self->Count = 0;
}