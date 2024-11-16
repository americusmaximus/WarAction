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

#include "ButtonControl.hxx"

#define MESSAGE_BUTTON_NONE         0x0
#define MESSAGE_BUTTON_OK           0x1
#define MESSAGE_BUTTON_CANCEL       0x2
#define MESSAGE_BUTTON_OKCANCEL     0x3
#define MESSAGE_MODE_WAIT           0x10

#define MESSAGE_BUTTON_MASK         0xF

#define MAX_MESSAGE_CONTROL_MESSAGE_LENTH   256

struct MessageControl;

typedef CONTROLTYPE(CLASSCALL* MESSAGECONTROLTYPEACTION)(MessageControl* self);
typedef VOID(CLASSCALL* MESSAGECONTROLINITIALIZEACTION)(MessageControl* self);
typedef VOID(CLASSCALL* MESSAGECONTROLDISABLEACTION)(MessageControl* self);
typedef VOID(CLASSCALL* MESSAGECONTROLTICKACTION)(MessageControl* self);
typedef U32(CLASSCALL* MESSAGECONTROLACTIONACTION)(MessageControl* self);
typedef MessageControl* (CLASSCALL* MESSAGECONTROLRELEASEACTION)(MessageControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct MessageControlSelf
{
    MESSAGECONTROLTYPEACTION        Type;
    MESSAGECONTROLINITIALIZEACTION  Initialize;
    MESSAGECONTROLDISABLEACTION     Disable;
    MESSAGECONTROLTICKACTION        Tick;
    MESSAGECONTROLACTIONACTION      Action;
    MESSAGECONTROLRELEASEACTION     Release;
} MESSAGECONTROLSELF, * MESSAGECONTROLSELFPTR;

EXTERN MESSAGECONTROLSELF MessageControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct MessageControl
{
    MESSAGECONTROLSELFPTR       Self;
    U8                          IsActive;
    CHAR                        Message[MAX_MESSAGE_CONTROL_MESSAGE_LENTH];
    struct
    {
        BUTTONCONTROLPTR        Ok;
        BUTTONCONTROLPTR        Cancel;
        struct
        {
            BUTTONCONTROLPTR    Ok;
            BUTTONCONTROLPTR    Cancel;
        } Combined;
    } Buttons;
    struct
    {
        BINASSET                Ok;
        BINASSET                Cancel;
        struct
        {
            BINASSET            Ok;
            BINASSET            Cancel;
        } Combined;
    } Assets;
    U32                         Mode;
    BOOL                        IsVisible;
    BOOL                        IsOk;
} MESSAGECONTROL, * MESSAGECONTROLPTR;
#pragma pack(pop)

EXTERN MESSAGECONTROL MessageControlState;

MESSAGECONTROLPTR CLASSCALL ActivateMessageControl(MESSAGECONTROLPTR self);
MESSAGECONTROLPTR CLASSCALL ReleaseMessageControl(MESSAGECONTROLPTR self, CONST OBJECTRELEASETYPE mode);
U32 CLASSCALL ActionMessageControl(MESSAGECONTROLPTR self);
VOID CLASSCALL DisposeMessageControl(MESSAGECONTROLPTR self);
VOID CLASSCALL HideMessageControl(MESSAGECONTROLPTR self);
VOID CLASSCALL InitializeMessageControl(MESSAGECONTROLPTR self);
VOID CLASSCALL ShowMessageControl(MESSAGECONTROLPTR self, LPCSTR message, CONST U32 mode);
VOID CLASSCALL TickMessageControl(MESSAGECONTROLPTR self);