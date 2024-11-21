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
#include "MessageControl.hxx"
#include "State.hxx"

#include <stdlib.h>

// 0x1003a378
MESSAGECONTROLSELF MessageControlSelfState =
{
    (MESSAGECONTROLTYPEACTION)AcquireControlTypeMessage,
    InitializeMessageControl,
    (MESSAGECONTROLDISABLEACTION)DisableControl,
    TickMessageControl,
    ActionMessageControl,
    ReleaseMessageControl
};

// 0x10046138
MESSAGECONTROL MessageControlState;

// 0x10004d60
MESSAGECONTROLPTR CLASSCALL ActivateMessageControl(MESSAGECONTROLPTR self)
{
    self->Self = (MESSAGECONTROLSELFPTR)&MessageControlSelfState;

    self->IsActive = FALSE;

    ActivateBinAsset(&self->Assets.Ok);
    ActivateBinAsset(&self->Assets.Cancel);
    ActivateBinAsset(&self->Assets.Combined.Ok);
    ActivateBinAsset(&self->Assets.Combined.Cancel);

    self->Mode = MESSAGE_BUTTON_NONE;
    self->IsVisible = FALSE;
    self->Message[0] = NULL;

    return self;
}

// 0x10004ef0
VOID CLASSCALL InitializeMessageControl(MESSAGECONTROLPTR self)
{
    InitializeBinAsset(&self->Assets.Ok, 2);
    SelectBinAssetItem(&self->Assets.Ok, 0, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg1BT0, 0)); // TODO
    SelectBinAssetItem(&self->Assets.Ok, 1, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg1BT1, 0)); // TODO

    InitializeBinAsset(&self->Assets.Cancel, 2);
    SelectBinAssetItem(&self->Assets.Cancel, 0, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg1CBT0, 0)); // TODO
    SelectBinAssetItem(&self->Assets.Cancel, 1, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg1CBT1, 0)); // TODO

    InitializeBinAsset(&self->Assets.Combined.Ok, 2);
    SelectBinAssetItem(&self->Assets.Combined.Ok, 0, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg2BT10, 0)); // TODO
    SelectBinAssetItem(&self->Assets.Combined.Ok, 1, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg2BT11, 0)); // TODO

    InitializeBinAsset(&self->Assets.Combined.Cancel, 2);
    SelectBinAssetItem(&self->Assets.Combined.Cancel, 0, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg2BT20, 0)); // TODO
    SelectBinAssetItem(&self->Assets.Combined.Cancel, 1, *(U32*)AcquireBinAssetContent(&AssetsState.Assets.Msg2BT21, 0)); // TODO

    self->Buttons.Ok = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), &self->Assets.Ok, 0, CONTROLACTION_MSGBOX_OK);
    self->Buttons.Cancel = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), &self->Assets.Cancel, 0, CONTROLACTION_MSGBOX_CANCEL);
    self->Buttons.Combined.Ok = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), &self->Assets.Combined.Ok, 0, CONTROLACTION_MSGBOX_OK);
    self->Buttons.Combined.Cancel = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), &self->Assets.Combined.Cancel, 0, CONTROLACTION_MSGBOX_CANCEL);

    self->Buttons.Ok->Y = 0;
    self->Buttons.Combined.Ok->Y = 0;
    self->Buttons.Combined.Cancel->Y = 0;
}

// 0x100052f0
VOID CLASSCALL TickMessageControl(MESSAGECONTROLPTR self)
{
    if (!self->IsVisible) { return; }

    U32 lines = 0;

    {
        U32 x = 0;

        while (self->Message[x] != NULL)
        {
            if (self->Message[x] == '\n' && self->Message[x + 1] != NULL) { lines = lines + 1; }

            x = x + 1;
        }
    }

    U32 offset = 197 - (AcquireFontAssetHeight(&AssetsState.Fonts.Main) * (lines * 10 - 4)) / 20;

    SelectFontAssetColor(&AssetsState.Fonts.Main, ADJUSTCOLOR(0xFF00));

    State.Renderer->Actions.DrawMainSurfaceColorShadeRectangle(0, 0,
        GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480, ADJUSTCOLOR(0x0000));

    State.Renderer->Actions.DrawMainSurfaceColorShadeRectangle(104, 150, 437, 104, ADJUSTCOLORS(0x1400, 0x1400, 0xA00));

    switch (self->Mode & MESSAGE_BUTTON_MASK)
    {
    case MESSAGE_BUTTON_NONE:
    {
        State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
            (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Msg0, 0));

        break;
    }
    case MESSAGE_BUTTON_OK:
    {
        State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
            (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Msg1BK, 0));

        self->Buttons.Ok->Self->Tick(self->Buttons.Ok);

        break;
    }
    case MESSAGE_BUTTON_CANCEL:
    {
        State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
            (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Msg1BK, 0));

        self->Buttons.Cancel->Self->Tick(self->Buttons.Cancel);

        break;
    }
    case MESSAGE_BUTTON_OKCANCEL:
    {
        State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
            (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Msg2BK, 0));

        self->Buttons.Combined.Ok->Self->Tick(self->Buttons.Combined.Ok);
        self->Buttons.Combined.Cancel->Self->Tick(self->Buttons.Combined.Cancel);

        break;
    }
    }

    {
        U32 x = 0;
        CHAR message[MAX_MESSAGE_CONTROL_MESSAGE_LENTH];

        while (self->Message[x] != NULL)
        {
            U32 xx = 0;

            while (self->Message[x] != '\n' && self->Message[x] != NULL)
            {
                message[xx] = self->Message[x];

                x = x + 1;
                xx = xx + 1;
            }

            message[xx] = NULL;

            if (self->Message[x] != NULL) { x = x + 1; }

            DrawFontAssetText(&AssetsState.Fonts.Main, 325, offset, message);

            offset = offset + AcquireFontAssetHeight(&AssetsState.Fonts.Main);
        }
    }
}

// 0x10005200
U32 CLASSCALL ActionMessageControl(MESSAGECONTROLPTR self)
{
    self->IsOk = FALSE;

    if (!self->IsVisible) { return CONTROLACTION_NONE; }

    switch (self->Mode & MESSAGE_BUTTON_MASK)
    {
    case MESSAGE_BUTTON_OK:
    {
        if (self->Buttons.Ok->Self->Action(self->Buttons.Ok) != CONTROLACTION_NONE)
        {
            self->IsOk = TRUE;

            HideMessageControl(self);
        }

        break;
    }
    case MESSAGE_BUTTON_CANCEL:
    {
        if (self->Buttons.Cancel->Self->Action(self->Buttons.Cancel) != CONTROLACTION_NONE)
        {
            self->IsOk = FALSE;

            HideMessageControl(self);
        }

        break;
    }
    case MESSAGE_BUTTON_OKCANCEL:
    {
        if (self->Buttons.Combined.Ok->Self->Action(self->Buttons.Combined.Ok) != CONTROLACTION_NONE)
        {
            self->IsOk = TRUE;

            HideMessageControl(self);
        }
        else if (self->Buttons.Combined.Cancel->Self->Action(self->Buttons.Combined.Cancel) != CONTROLACTION_NONE)
        {
            self->IsOk = FALSE;

            HideMessageControl(self);
        }

        break;
    }
    }

    return CONTROLACTION_NONE;
}

// 0x10004e00
MESSAGECONTROLPTR CLASSCALL ReleaseMessageControl(MESSAGECONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeMessageControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10004e20
VOID CLASSCALL DisposeMessageControl(MESSAGECONTROLPTR self)
{
    self->Self = (MESSAGECONTROLSELFPTR)&ControlSelfState;

    if (self->Buttons.Ok != NULL) { self->Buttons.Ok->Self->Release(self->Buttons.Ok, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Buttons.Cancel != NULL) { self->Buttons.Cancel->Self->Release(self->Buttons.Cancel, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Buttons.Combined.Ok != NULL) { self->Buttons.Combined.Ok->Self->Release(self->Buttons.Combined.Ok, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Buttons.Combined.Cancel != NULL) { self->Buttons.Combined.Cancel->Self->Release(self->Buttons.Combined.Cancel, OBJECTRELEASETYPE_ALLOCATED); }

    DisposeBinAsset(&self->Assets.Combined.Cancel);
    DisposeBinAsset(&self->Assets.Combined.Ok);
    DisposeBinAsset(&self->Assets.Cancel);
    DisposeBinAsset(&self->Assets.Ok);
}

// 0x100051f0
VOID CLASSCALL HideMessageControl(MESSAGECONTROLPTR self)
{
    self->IsVisible = FALSE;
    self->Mode = MESSAGE_BUTTON_NONE;
}

// 0x10005120
VOID CLASSCALL ShowMessageControl(MESSAGECONTROLPTR self, LPCSTR message, CONST U32 mode)
{
    if (!self->IsActive) { self->Self->Initialize(self); }

    strcpy(self->Message, message);

    self->IsVisible = TRUE;
    self->Mode = mode;

    if (self->Buttons.Ok->IsActive) { self->Buttons.Ok->Self->Disable(self->Buttons.Ok); }
    if (self->Buttons.Cancel->IsActive) { self->Buttons.Cancel->Self->Disable(self->Buttons.Cancel); }
    if (self->Buttons.Combined.Ok->IsActive) { self->Buttons.Combined.Ok->Self->Disable(self->Buttons.Combined.Ok); }
    if (self->Buttons.Combined.Cancel->IsActive) { self->Buttons.Combined.Cancel->Self->Disable(self->Buttons.Combined.Cancel); }

    switch (mode & MESSAGE_BUTTON_MASK)
    {
    case MESSAGE_BUTTON_OK: { self->Buttons.Ok->Self->Initialize(self->Buttons.Ok); break; }
    case MESSAGE_BUTTON_CANCEL: { self->Buttons.Cancel->Self->Initialize(self->Buttons.Cancel); break; }
    case MESSAGE_BUTTON_OKCANCEL:
    {
        self->Buttons.Combined.Ok->Self->Initialize(self->Buttons.Combined.Ok);
        self->Buttons.Combined.Cancel->Self->Initialize(self->Buttons.Combined.Cancel);

        break;
    }
    }
}