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

#include "Control.hxx"

struct ControlType2x39;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE2X39TYPEACTION)(ControlType2x39* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X39INITIALIZEACTION)(ControlType2x39* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X39DISABLEACTION)(ControlType2x39* self);
typedef VOID(CLASSCALL* CONTROLTYPE2X39TICKACTION)(ControlType2x39* self);
typedef U32(CLASSCALL* CONTROLTYPE2X39ACTIONACTION)(ControlType2x39* self);
typedef ControlType2x39* (CLASSCALL* CONTROLTYPE2X39RELEASEACTION)(ControlType2x39* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ControlType2x39Self
{
    CONTROLTYPE2X39TYPEACTION Type;
    CONTROLTYPE2X39INITIALIZEACTION Initialize;
    CONTROLTYPE2X39DISABLEACTION Disable;
    CONTROLTYPE2X39TICKACTION Tick;
    CONTROLTYPE2X39ACTIONACTION Action;
    CONTROLTYPE2X39RELEASEACTION Release;
} CONTROLTYPE2X39SELF, * CONTROLTYPE2X39SELFPTR;

EXTERN CONTROLTYPE2X39SELF ObjectType2x39SelfState;

#pragma pack(push, 1)
// INHERITANCE: ControlType2x38
typedef struct ControlType2x39
{
    CONTROLTYPE2X39SELFPTR  Self;
    U8                      IsActive;
    BINASSETPTR             Asset;
    U32                     Index;
    S32                     Shortcut;
    S32                     X;
    S32                     Y;
    LPCSTR                  Click;
    LPCSTR                  Unclick;
    U8                      IsAction;
    U8                      IsVisible;
    U8                      IsClickable;
    U32                     Unk12; // TODO
    U32                     Ticks;
    U32                     Unk0x2c; // TODO
    U32                     Action;
    U32                     ActualAction;
    U8                      Unk0x39; // TODO
} CONTROLTYPE2X39, * CONTROLTYPE2X39PTR;
#pragma pack(pop)

CONTROLTYPE2X39PTR CLASSCALL ActivateObjectType2x39(CONTROLTYPE2X39PTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action);
VOID CLASSCALL InitializeObjectType2x39(CONTROLTYPE2X39PTR self);
U32 CLASSCALL ActionObjectType2x39(CONTROLTYPE2X39PTR self);