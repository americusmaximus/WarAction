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
#include "FontAsset.hxx"

struct LabelControl;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE0X29TYPEACTION)(LabelControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE0X29INITIALIZEACTION)(LabelControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE0X29DISABLEACTION)(LabelControl* self);
typedef VOID(CLASSCALL* CONTROLTYPE0X29TICKACTION)(LabelControl* self);
typedef U32(CLASSCALL* CONTROLTYPE0X29ACTIONACTION)(LabelControl* self);
typedef LabelControl* (CLASSCALL* CONTROLTYPE0X29RELEASEACTION)(LabelControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct LabelControlSelf
{
    CONTROLTYPE0X29TYPEACTION       Type;
    CONTROLTYPE0X29INITIALIZEACTION Initialize;
    CONTROLTYPE0X29DISABLEACTION    Disable;
    CONTROLTYPE0X29TICKACTION       Tick;
    CONTROLTYPE0X29ACTIONACTION     Action;
    CONTROLTYPE0X29RELEASEACTION    Release;
} LABELCONTROLSELF, * LABELCONTROLSELFPTR;

EXTERN LABELCONTROLSELF LabelControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct LabelControl
{
    LABELCONTROLSELFPTR Self;
    U8                  IsActive;
    U32                 X;
    U32                 Y;
    U32                 Width;
    U32                 Height;
    BOOL                IsVisible;
    FONTASSETPTR        Font;
    LPSTR               Text;
    U32                 Color;
    BOOL                IsCentered;
} LABELCONTROL, * LABELCONTROLPTR;
#pragma pack(pop)

LABELCONTROLPTR CLASSCALL ActivateLabelControl(LABELCONTROLPTR self, CONST U32 x, CONST U32 y, CONST U32 width, CONST U32 height, LPCSTR text);
LABELCONTROLPTR CLASSCALL ReleaseLabelControl(LABELCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeLabelControl(LABELCONTROLPTR self);
VOID CLASSCALL SlectLabelControlText(LABELCONTROLPTR self, LPCSTR text);
VOID CLASSCALL TickLabelControl(LABELCONTROLPTR self);