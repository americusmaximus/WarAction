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

#pragma once

#include "Control.hxx"
#include "FontAsset.hxx"
#include "Strings.hxx"

struct DescriptionControl;

typedef CONTROLTYPE(CLASSCALL* DESCRIPTIONCONTROLTYPEACTION)(DescriptionControl* self);
typedef VOID(CLASSCALL* DESCRIPTIONCONTROLINITIALIZEACTION)(DescriptionControl* self);
typedef VOID(CLASSCALL* DESCRIPTIONCONTROLDISABLEACTION)(DescriptionControl* self);
typedef VOID(CLASSCALL* DESCRIPTIONCONTROLTICKACTION)(DescriptionControl* self);
typedef U32(CLASSCALL* DESCRIPTIONCONTROLACTIONACTION)(DescriptionControl* self);
typedef DescriptionControl* (CLASSCALL* DESCRIPTIONCONTROLRELEASEACTION)(DescriptionControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct DescriptionControlSelf
{
    DESCRIPTIONCONTROLTYPEACTION        Type;
    DESCRIPTIONCONTROLINITIALIZEACTION  Initialize;
    DESCRIPTIONCONTROLDISABLEACTION     Disable;
    DESCRIPTIONCONTROLTICKACTION        Tick;
    DESCRIPTIONCONTROLACTIONACTION      Action;
    DESCRIPTIONCONTROLRELEASEACTION     Release;
} DESCRIPTIONCONTROLSELF, * DESCRIPTIONCONTROLSELFPTR;

EXTERN DESCRIPTIONCONTROLSELF DescriptionControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct DescriptionControl
{
    DESCRIPTIONCONTROLSELFPTR   Self;
    U8                          IsActive;
    S32                         X;
    S32                         Y;
    S32                         Width;
    S32                         Height;
    U32                         HorizontalOffset;
    U32                         VerticalOffset;
    U32                         Unk08; // TODO
    U32                         Unk09; // TODO
    U32                         LineCount;
    U32                         LineLength;
    U32                         MinDelay;
    U32                         MidDelay;
    U32                         MaxDelay;
    FONTASSETPTR                Font;
    LPSTR                       Text;
    LPSTR                       End;
    U32                         Ticks;
    U32                         Unk19; // TODO Ticks again!
    U32                         Color;
} DESCRIPTIONCONTROL, * DESCRIPTIONCONTROLPTR;
#pragma pack(pop)

DESCRIPTIONCONTROLPTR CLASSCALL ActivateDescriptionControl(DESCRIPTIONCONTROLPTR self, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, FONTASSETPTR font, CONST U32 xo, CONST U32 yo);
DESCRIPTIONCONTROLPTR CLASSCALL ReleaseDescriptionControl(DESCRIPTIONCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
U32 CLASSCALL ActionDescriptionControl(DESCRIPTIONCONTROLPTR self);
VOID CLASSCALL DisableDescriptionControl(DESCRIPTIONCONTROLPTR self);
VOID CLASSCALL DisposeDescriptionControl(DESCRIPTIONCONTROLPTR self);
VOID CLASSCALL DrawTextDescriptionControl(DESCRIPTIONCONTROLPTR self, LPSTR text, CONST U32 width);
VOID CLASSCALL DrawTextItemDescriptionControl(DESCRIPTIONCONTROLPTR self, CONST UNICHAR value);
VOID CLASSCALL FUN_1000e910(DESCRIPTIONCONTROLPTR self); // TODO name
VOID CLASSCALL InitializeDescriptionControl(DESCRIPTIONCONTROLPTR self);
VOID CLASSCALL SelectDescriptionControlText(DESCRIPTIONCONTROLPTR self, LPCSTR text);
VOID CLASSCALL TickDescriptionControl(DESCRIPTIONCONTROLPTR self);