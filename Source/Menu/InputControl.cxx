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
#include "ControlCommand.hxx"
#include "Input.hxx"
#include "InputControl.hxx"
#include "Graphics.hxx"
#include "Settings.hxx"
#include "State.hxx"

#include <..\Text\Resources.hxx>

#include <stdio.h>
#include <stdlib.h>

// 0x1003a348
INPUTCONTROLSELF InputControlSelfState =
{
    (INPUTCONTROLTYPEACTION)AcquireControlTypeInput,
    InitializeInputControl,
    DisableInputControl,
    TickInputControl,
    ActionInputControl,
    ReleaseInputControl
};

// 0x10003e70
INPUTCONTROLPTR CLASSCALL ActivateInputControl(INPUTCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, FONTASSETPTR font, CONST S32 capacity)
{
    self->Self = &InputControlSelfState;

    self->IsActive = FALSE;

    self->Action = action;

    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;

    self->Font = font;

    self->Capacity = capacity;

    self->Alignment = FONTALIGNMENT_LEFT;
    self->Allowed.Count = 0;
    self->Invalid = NULL;
    self->Margin = 0;
    self->IsModifiable = TRUE;

    self->Value = (LPSTR)malloc(MAX_INPUT_CONTROL_VALUE_LENGTH);
    self->Text = (LPSTR)malloc(MAX_INPUT_CONTROL_TEXT_LENGTH);

    SelectInputControlValue(self, StringsState.Scratch);
    SelectInputControlInvalidValue(self, NULL);

    return self;
}

// 0x10004100
VOID CLASSCALL InitializeInputControl(INPUTCONTROLPTR self)
{
    InitializeControl((CONTROLPTR)self);

    SelectInputControlFocus(self, FALSE);

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_LIMIT_MENU_INPUT);
        AcquireStringValue(&value, "0-FFFFFF");

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        InitializeInputControlAllowItems(&self->Allowed, actual->Value);

        ReleaseStringValue(actual);
    }

    ActivateActionArea(self->X, self->Y, self->Width, self->Height,
        CONTROLCOMMANDACTION_MOUSE_LEFT_UP, self->Action, DEFAULT_ACTION_PRIORITY);
}

// 0x100041d0
VOID CLASSCALL DisableInputControl(INPUTCONTROLPTR self)
{
    DequeueControlCommand(self->Action);

    if (ControlState.Active == (CONTROLPTR)self) { RemoveInputControlFocus(self); }

    SelectInputControlInvalidValue(self, NULL);

    DisableControl((CONTROLPTR)self);
}

// 0x10004250
VOID CLASSCALL TickInputControl(INPUTCONTROLPTR self)
{
    State.Renderer->Window.X = self->X;
    State.Renderer->Window.Y = self->Y;
    State.Renderer->Window.Width = self->X + self->Width - 1;
    State.Renderer->Window.Height = self->Y + self->Height - 1;

    U32 width = 0;

    for (LPSTR value = self->Value; (ADDR)value - (ADDR)self->Value < self->Input; value = AcquireNextString(value))
    {
        width = width + AcquireFontAssetItemWidth(self->Font, AcquireUnicode(value));
    }

    if (self->Alignment == FONTALIGNMENT_LEFT)
    {
        if (self->IsFocus)
        {
            if (self->Width < width + 3 + self->Margin) { self->Margin = self->Width - width - 3; }

            if (width + 3 + self->Margin < self->Width / 2) { self->Margin = self->Margin + AcquireFontAssetItemWidth(self->Font, NULL); }

            if (0 < self->Margin) { self->Margin = 0; }
        }
    }
    else { self->Margin = (self->Width - width) / 2; }

    CONST S32 x = self->X + self->Margin;
    CONST S32 y = self->Y + (self->Height - AcquireFontAssetHeight(self->Font)) / 2;

    SelectFontAssetColor(self->Font, ADJUSTCOLOR(0xFF00));

    DrawFontAssetText(self->Font, x, y, self->Value, FONTALIGNMENT_LEFT);

    if (self->IsFocus && ((GetTickCount() / 500) & 1)) { DrawFontAssetItem(self->Font, x + width, y, '|'); }

    State.Renderer->Window.X = 0;
    State.Renderer->Window.Y = 0;
    State.Renderer->Window.Width = GRAPHICS_RESOLUTION_640 - 1;
    State.Renderer->Window.Height = GRAPHICS_RESOLUTION_480 - 1;
}

// 0x10004440
U32 CLASSCALL ActionInputControl(INPUTCONTROLPTR self)
{
    ActionControl((CONTROLPTR)self);

    if (!self->IsModifiable) { return CONTROLACTION_NONE; }

    CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    while (command != NULL)
    {
        if (command->Command == self->Action)
        {
            if ((command->Action & CONTROLCOMMANDACTION_MOUSE_LEFT_UP) && !self->IsFocus) { SelectInputControlFocus(self); }
        }
        else
        {
            if (command->Command != CONTROLCOMMAND_UTF) { return CONTROLACTION_NONE; }
            if (!self->IsFocus) { return CONTROLACTION_NONE; }

            CONST U32 action = command->Action;

            if (!(action & CONTROLCOMMANDACTION_UTF_KEY_DOWN))
            {
                if (!(action & CONTROLCOMMANDACTION_UTF_KEY_ANY)
                    && IsInputControlInputAllowed(self, action)
                    && IsInputControlAllowItemsAllowed(&self->Allowed, action))
                {
                    CONST U32 length = AcquireUnicodeLength(action);

                    if (length + 1 + self->Length <= self->Capacity)
                    {
                        MoveMemory((LPSTR)((ADDR)self->Value + self->Input + length),
                            (LPCSTR)((ADDR)self->Value + self->Input),
                            strlen((LPCSTR)((ADDR)self->Value + self->Input)) + 1);

                        {
                            CONST U32 count = AppendUnicode((LPSTR)((ADDR)self->Value + self->Input), action);

                            self->Length = self->Length + count;
                            self->Input = self->Input + count;
                        }
                    }
                }
            }
            else
            {
                switch (action & VK_INPUT)
                {
                case VK_BACK:
                {
                    if (self->Input != 0)
                    {
                        self->Input = (ADDR)AcquirePriorString((LPSTR)((ADDR)self->Value + self->Input)) - (ADDR)self->Value;

                        if (self->Input < self->Length)
                        {
                            CONST U32 length = AcquireAnsiLength((LPSTR)((ADDR)self->Value + self->Input));

                            MoveMemory((LPSTR)((ADDR)self->Value + self->Input),
                                (LPSTR)((ADDR)self->Value + self->Input + length),
                                strlen((LPSTR)((ADDR)self->Value + self->Input + length)) + 1);

                            self->Length = self->Length - length;
                        }
                    }

                    break;
                }
                case VK_RETURN:
                {
                    EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
                        CONTROLACTION_UI_COMMIT, DEFAULT_CONTROLACTION_UI_VALUE);

                    RemoveInputControlFocus(self);

                    break;
                }
                case VK_ESCAPE:
                {
                    strcpy(self->Value, self->Text);

                    RemoveInputControlFocus(self);

                    break;
                }
                case VK_LEFT:
                {
                    if (self->Input != 0)
                    {
                        self->Input = (ADDR)AcquirePriorString((LPSTR)((ADDR)self->Value + self->Input)) - (ADDR)self->Value;
                    }

                    break;
                }
                case VK_RIGHT:
                {
                    if (self->Input < self->Length)
                    {
                        self->Input = (ADDR)AcquireNextString((LPSTR)((ADDR)self->Value + self->Input)) - (ADDR)self->Value;
                    }

                    break;
                }
                case VK_INSERT:
                {
                    if ((InputState.State[DIK_LSHIFT] || InputState.State[DIK_RSHIFT])
                        && IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(NULL))
                    {
                        HANDLE handle = GetClipboardData(CF_TEXT);

                        if (handle != NULL)
                        {
                            LPSTR input = (LPSTR)GlobalLock(handle);

                            if (input != NULL)
                            {
                                CONST U32 size = strlen(input);
                                CONST U32 length = AcquireUnicodeString(NULL, 0, input, size);

                                LPSTR content = (LPSTR)malloc(length + 1);

                                AcquireUnicodeString(content, length + 1, input, size);

                                if (length != 0)
                                {
                                    U32 count = length;

                                    while (count >= 0)
                                    {
                                        if (self->Length + count <= self->Capacity) { break; }

                                        count = (ADDR)AcquirePriorString((LPSTR)((ADDR)content + count)) - (ADDR)content;
                                    }

                                    if (count > 0)
                                    {
                                        MoveMemory((LPSTR)((ADDR)self->Value + self->Input + count),
                                            (LPSTR)((ADDR)self->Value + self->Input),
                                            strlen((LPSTR)((ADDR)self->Value + self->Input)) + 1);

                                        CopyMemory((LPSTR)((ADDR)self->Value + self->Input), content, count);

                                        self->Input = self->Input + count;
                                        self->Length = self->Length + count;
                                    }
                                }

                                free(content);

                                GlobalUnlock(handle);
                            }
                        }

                        CloseClipboard();
                    }

                    break;
                }
                case VK_DELETE:
                {
                    if (self->Input < self->Length)
                    {
                        CONST U32 length = AcquireAnsiLength((LPSTR)((ADDR)self->Value + self->Input));

                        MoveMemory((LPSTR)((ADDR)self->Value + self->Input),
                            (LPSTR)((ADDR)self->Value + self->Input + length),
                            strlen((LPSTR)((ADDR)self->Value + self->Input + length)) + 1);

                        self->Length = self->Length - length;
                    }

                    break;
                }
                }
            }
        }

        DequeueControlCommand(TRUE);

        command = DequeueControlCommand(FALSE);
    }

    return CONTROLACTION_NONE;
}

// 0x10003f60
INPUTCONTROLPTR CLASSCALL ReleaseInputControl(INPUTCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeInputControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10003f80
VOID CLASSCALL DisposeInputControl(INPUTCONTROLPTR self)
{
    self->Self = (INPUTCONTROLSELFPTR)&ControlSelfState;

    if (self->Value != NULL)
    {
        free(self->Value);

        self->Value = NULL;
    }

    if (self->Text != NULL)
    {
        free(self->Text);

        self->Text = NULL;
    }
}

// 0x10004050
VOID CLASSCALL SelectInputControlValue(INPUTCONTROLPTR self, LPCSTR value)
{
    self->Length = strlen(value);

    strcpy(self->Value, value);

    self->Input = self->Length;
    strcpy(self->Text, self->Value);
}

// 0x100040a0
VOID CLASSCALL SelectInputControlInvalidValue(INPUTCONTROLPTR self, LPCSTR value)
{
    if (self->Invalid != NULL)
    {
        free(self->Invalid);

        self->Invalid = NULL;
    }

    if (value != NULL)
    {
        self->Invalid = (LPSTR)malloc(strlen(value) + 1);

        strcpy(self->Invalid, value);
    }
}

// 0x100040f0
LPCSTR CLASSCALL AcquireInputControlValue(INPUTCONTROLPTR self)
{
    return self->Value;
}

// 0x10004210
VOID CLASSCALL SelectInputControlFocus(INPUTCONTROLPTR self, CONST BOOL focus)
{
    if (focus != self->IsFocus)
    {
        EnqueueControlCommand(CONTROLCOMMAND_UI, self->Action,
            focus ? CONTROLACTION_UI_FOCUS : CONTROLACTION_UI_NONE, DEFAULT_CONTROLACTION_UI_VALUE);
    }

    self->IsFocus = focus;

    self->Margin = 0;
}

// 0x10004890
VOID CLASSCALL InitializeInputControlAllowItems(INPUTCONTROLPTR self, LPCSTR restrictions)
{
    InitializeInputControlAllowItems(&self->Allowed, restrictions);
}

// 0x10004000
VOID CLASSCALL SelectInputControlFocus(INPUTCONTROLPTR self)
{
    if (ControlState.Active != NULL) { RemoveInputControlFocus((INPUTCONTROLPTR)ControlState.Active); }

    ControlState.Active = (CONTROLPTR)self;

    SelectInputControlFocus(self, TRUE);
}

// 0x10004030
VOID CLASSCALL RemoveInputControlFocus(INPUTCONTROLPTR self)
{
    SelectInputControlFocus(self, FALSE);

    ControlState.Active = NULL;
}

// 0x10004820
BOOL CLASSCALL IsInputControlInputAllowed(INPUTCONTROLPTR self, CONST U32 action)
{
    if (action < ' ' || 0xFFFF < action) { return FALSE; }

    if (self->Invalid != NULL)
    {
        LPSTR list = self->Invalid;

        for (UNICHAR code = AcquireUnicode(list); code != NULL;
            list = AcquireNextString(list), code = AcquireUnicode(list))
        {
            if (code == action) { return FALSE; }
        }
    }

    return TRUE;
}

// 0x10023790
VOID CLASSCALL InitializeInputControlAllowItems(INPUTCONTROLALLOWITEMSPTR self, LPCSTR restrictions)
{
    self->Count = 0;

    LPSTR value = (LPSTR)restrictions;

    while (value[0] != NULL)
    {
        // Skip spaces and tabs.
        while (value[0] == ' ' || value[0] == '\t') { value = (LPSTR)((ADDR)value + 1); }

        if (value[0] == NULL) { return; }

        U32 min = 0;
        U32 max = 0;

        U32 count = sscanf(value, "%x-%x", &min, &max);

        if (count < 2) { max = min; }

        if (count != 0)
        {
            self->Items[self->Count].Min = min;
            self->Items[self->Count].Max = max;

            self->Count = self->Count + 1;
        }

        while (value[0] != NULL)
        {
            if (value[0] == ',') { value = (LPSTR)((ADDR)value + 1); break; }

            value = (LPSTR)((ADDR)value + 1);
        }
    }
}

// 0x10023820
BOOL CLASSCALL IsInputControlAllowItemsAllowed(INPUTCONTROLALLOWITEMSPTR self, CONST U32 action)
{
    for (U32 x = 0; x < self->Count; x++)
    {
        if (self->Items[x].Min <= action && action <= self->Items[x].Max) { return TRUE; }
    }

    return FALSE;
}
