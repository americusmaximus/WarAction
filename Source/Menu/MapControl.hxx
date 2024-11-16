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

#include "DescriptionControl.hxx"
#include "LabelControl.hxx"
#include "Map.hxx"

struct MapControl;

typedef CONTROLTYPE(CLASSCALL* MAPCONTROLTYPEACTION)(MapControl* self);
typedef VOID(CLASSCALL* MAPCONTROLINITIALIZEACTION)(MapControl* self);
typedef VOID(CLASSCALL* MAPCONTROLDISABLEACTION)(MapControl* self);
typedef VOID(CLASSCALL* MAPCONTROLTICKACTION)(MapControl* self);
typedef U32(CLASSCALL* MAPCONTROLACTIONACTION)(MapControl* self);
typedef MapControl* (CLASSCALL* MAPCONTROLRELEASEACTION)(MapControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct MapControlSelf
{
    MAPCONTROLTYPEACTION        Type;
    MAPCONTROLINITIALIZEACTION  Initialize;
    MAPCONTROLDISABLEACTION     Disable;
    MAPCONTROLTICKACTION        Tick;
    MAPCONTROLACTIONACTION      Action;
    MAPCONTROLRELEASEACTION     Release;
} MAPCONTROLSELF, * MAPCONTROLSELFPTR;

EXTERN MAPCONTROLSELF MapControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct MapControl
{
    MAPCONTROLSELFPTR       Self;
    U8                      IsActive;
    MAP                     Map;
    S32                     X;
    S32                     Y;
    DESCRIPTIONCONTROLPTR   Description;
    LABELCONTROLPTR         Size;
    LABELCONTROLPTR         Actors;
    BOOL                    IsHover;
} MAPCONTROL, * MAPCONTROLPTR;
#pragma pack(pop)

MAPCONTROLPTR CLASSCALL ActivateMapControl(MAPCONTROLPTR self, CONST S32 cx, CONST S32 cy, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, LABELCONTROLPTR size, LABELCONTROLPTR actors);
MAPCONTROLPTR CLASSCALL ReleaseMapControl(MAPCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
U32 CLASSCALL ActionMapControl(MAPCONTROLPTR self);
VOID CLASSCALL DisableMapControl(MAPCONTROLPTR self);
VOID CLASSCALL DisposeMapControl(MAPCONTROLPTR self);
VOID CLASSCALL InitializeMapControl(MAPCONTROLPTR self);
VOID CLASSCALL InitializeMapMapControl(MAPCONTROLPTR self, LPCSTR name);
VOID CLASSCALL TickMapControl(MAPCONTROLPTR self);