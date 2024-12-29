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

#include "BinAsset.hxx"
#include "Control.hxx"

struct ImageControl;

typedef CONTROLTYPE(CLASSCALL* IMAGECONTROLTYPEACTION)(ImageControl* self);
typedef VOID(CLASSCALL* IMAGECONTROLINITIALIZEACTION)(ImageControl* self);
typedef VOID(CLASSCALL* IMAGECONTROLDISABLEACTION)(ImageControl* self);
typedef VOID(CLASSCALL* IMAGECONTROLTICKACTION)(ImageControl* self);
typedef U32(CLASSCALL* IMAGECONTROLACTIONACTION)(ImageControl* self);
typedef ImageControl* (CLASSCALL* IMAGECONTROLRELEASEACTION)(ImageControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ImageControlSelf
{
    IMAGECONTROLTYPEACTION          Type;
    IMAGECONTROLINITIALIZEACTION    Initialize;
    IMAGECONTROLDISABLEACTION       Disable;
    IMAGECONTROLTICKACTION          Tick;
    IMAGECONTROLACTIONACTION        Action;
    IMAGECONTROLRELEASEACTION       Release;
} IMAGECONTROLSELF, * IMAGECONTROLSELFPTR;

EXTERN IMAGECONTROLSELF ImageControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct ImageControl
{
    IMAGECONTROLSELFPTR Self;
    U8                  IsActive;
    BINASSETPTR         Asset;
    U32                 Index;
    S32                 X;
    S32                 Y;
} IMAGECONTROL, * IMAGECONTROLPTR;
#pragma pack(pop)

IMAGECONTROLPTR CLASSCALL ActivateImageControl(IMAGECONTROLPTR self, BINASSETPTR asset, CONST U32 indx);
VOID CLASSCALL TickImageControl(IMAGECONTROLPTR self);