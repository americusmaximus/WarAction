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
#include "StringList.hxx"
#include "ScrollControl.hxx"

#define INVALID_LIST_CONTROL_INDEX  (-1)

struct ListControl;

typedef CONTROLTYPE(CLASSCALL* LISTCONTROLTYPEACTION)(ListControl* self);
typedef VOID(CLASSCALL* LISTCONTROLINITIALIZEACTION)(ListControl* self);
typedef VOID(CLASSCALL* LISTCONTROLDISABLEACTION)(ListControl* self);
typedef VOID(CLASSCALL* LISTCONTROLTICKACTION)(ListControl* self);
typedef U32(CLASSCALL* LISTCONTROLACTIONACTION)(ListControl* self);
typedef ListControl* (CLASSCALL* LISTCONTROLRELEASEACTION)(ListControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ControlType7x2dSelf
{
    LISTCONTROLTYPEACTION       Type;
    LISTCONTROLINITIALIZEACTION Initialize;
    LISTCONTROLDISABLEACTION    Disable;
    LISTCONTROLTICKACTION       Tick;
    LISTCONTROLACTIONACTION     Action;
    LISTCONTROLRELEASEACTION    Release;
} LISTCONTROLSELF, * LISTCONTROLSELFPTR;

EXTERN LISTCONTROLSELF ListControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct ListControl
{
    LISTCONTROLSELFPTR  Self;
    U8                  IsActive;
    STRINGLISTPTR       Items;
    U32                 Action;
    S32                 X;
    S32                 Y;
    S32                 Width;
    S32                 Height;
    SCROLLCONTROLPTR    Scroll;
    FONTASSETPTR        Font;
    U32                 Unk10; // TODO
    S32                 Index;
} LISTCONTROL, * LISTCONTROLPTR;
#pragma pack(pop)

LISTCONTROLPTR CLASSCALL ActivateListControl(LISTCONTROLPTR self, CONST U32 action, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, SCROLLCONTROLPTR scroll, FONTASSETPTR font, STRINGLISTPTR items);
VOID CLASSCALL InitializeListControl(LISTCONTROLPTR self);
VOID CLASSCALL DisableListControl(LISTCONTROLPTR self);
VOID CLASSCALL TickListControl(LISTCONTROLPTR self);
U32 CLASSCALL ActionListControl(LISTCONTROLPTR self);
LISTCONTROLPTR CLASSCALL ReleaseListControl(LISTCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeListControl(LISTCONTROLPTR self);
BOOL CLASSCALL SelectListControlItem(LISTCONTROLPTR self, CONST S32 indx);
VOID CLASSCALL ListControlCommandUnknown1(LISTCONTROLPTR self); // TODO Name