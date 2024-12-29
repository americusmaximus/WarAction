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

#include "FontAsset.hxx"
#include "ScrollControl.hxx"

struct ChatControl;

typedef CONTROLTYPE(CLASSCALL* CHATCONTROLTYPEACTION)(ChatControl* self);
typedef VOID(CLASSCALL* CHATCONTROLINITIALIZEACTION)(ChatControl* self);
typedef VOID(CLASSCALL* CHATCONTROLDISABLEACTION)(ChatControl* self);
typedef VOID(CLASSCALL* CHATCONTROLTICKACTION)(ChatControl* self);
typedef U32(CLASSCALL* CHATCONTROLACTIONACTION)(ChatControl* self);
typedef ChatControl* (CLASSCALL* CHATCONTROLRELEASEACTION)(ChatControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ChatControlSelf
{
    CHATCONTROLTYPEACTION       Type;
    CHATCONTROLINITIALIZEACTION Initialize;
    CHATCONTROLDISABLEACTION    Disable;
    CHATCONTROLTICKACTION       Tick;
    CHATCONTROLACTIONACTION     Action;
    CHATCONTROLRELEASEACTION    Release;
} CHATCONTROLSELF, * CHATCONTROLSELFPTR;

EXTERN CHATCONTROLSELF ChatControlSelfState;

#define MAX_CHAT_CONTROL_MESSAGE_COUNT  100

typedef struct ChatControlMessage
{
    U32                 Color;
    LPSTR               Text;
} CHATCONTROLMESSAGE, * CHATCONTROLMESSAGEPTR;

typedef struct ChatControlMessages
{
    CHATCONTROLMESSAGE  Items[MAX_CHAT_CONTROL_MESSAGE_COUNT];
    U32                 Count;
} CHATCONTROLMESSAGES, * CHATCONTROLMESSAGESPTR;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct ChatControl
{
    CHATCONTROLSELFPTR      Self;
    U8                      IsActive;
    CHATCONTROLMESSAGESPTR  Messages;
    U32                     Action;
    S32                     X;
    S32                     Y;
    S32                     Width;
    S32                     Height;
    FONTASSETPTR            Font;
    SCROLLCONTROLPTR        Scroll;
} CHATCONTROL, * CHATCONTROLPTR;
#pragma pack(pop)

CHATCONTROLPTR CLASSCALL ActivateChatControl(CHATCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, SCROLLCONTROLPTR scroll, FONTASSETPTR font);
CHATCONTROLPTR CLASSCALL ReleaseChatControl(CHATCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
CHATCONTROLMESSAGESPTR CLASSCALL ActivateChatControlMessages(CHATCONTROLMESSAGESPTR self);
U32 CLASSCALL ActionChatControl(CHATCONTROLPTR self);
VOID CLASSCALL DisableChatControl(CHATCONTROLPTR self);
VOID CLASSCALL DisposeChatControl(CHATCONTROLPTR self);
VOID CLASSCALL InitializeChatControl(CHATCONTROLPTR self);
VOID CLASSCALL ReleaseChatControlMessages(CHATCONTROLMESSAGESPTR self);
VOID CLASSCALL TickChatControl(CHATCONTROLPTR self);