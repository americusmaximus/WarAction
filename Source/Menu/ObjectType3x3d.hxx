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

struct ControlType3x3d;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE3X3DTYPEACTION)(ControlType3x3d* self);
typedef VOID(CLASSCALL* CONTROLTYPE3X3DINITIALIZEACTION)(ControlType3x3d* self);
typedef VOID(CLASSCALL* CONTROLTYPE3X3DDISABLEACTION)(ControlType3x3d* self);
typedef VOID(CLASSCALL* CONTROLTYPE3X3DTICKACTION)(ControlType3x3d* self);
typedef U32(CLASSCALL* CONTROLTYPE3X3DACTIONACTION)(ControlType3x3d* self);
typedef ControlType3x3d* (CLASSCALL* CONTROLTYPE3X3DRELEASEACTION)(ControlType3x3d* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct ControlType3x3dSelf
{
    CONTROLTYPE3X3DTYPEACTION Type;
    CONTROLTYPE3X3DINITIALIZEACTION Initialize;
    CONTROLTYPE3X3DDISABLEACTION Disable;
    CONTROLTYPE3X3DTICKACTION Tick;
    CONTROLTYPE3X3DACTIONACTION Action;
    CONTROLTYPE3X3DRELEASEACTION Release;
} CONTROLTYPE3X3DSELF, * CONTROLTYPE3X3DSELFPTR;

EXTERN CONTROLTYPE3X3DSELF ObjectType3x3dSelfState;

#pragma pack(push, 1)
// INHERITANCE: ImageControl
typedef struct ControlType3x3d
{
    CONTROLTYPE3X3DSELFPTR  Self;
    U8                      IsActive;
    BINASSETPTR             Asset;
    U32                     Index;
    U32                     X;
    U32                     Y;
    U32                     Action;
    U32 Unk07; // TODO
    U32 Unk08; // TODO
    U32 Unk09; // TODO
    U32 Unk10; // TODO
    U32 Unk11; // TODO
    U32 Unk12; // TODO
    U32 Unk13; // TODO
    U32 Unk14; // TODO MultiSpeed
    U32 Unk15; // TODO
} CONTROLTYPE3X3D, * CONTROLTYPE3X3DPTR;
#pragma pack(pop)

EXTERN CONTROLTYPE3X3DPTR ControlType3x3dState; // TODO

CONTROLTYPE3X3DPTR CLASSCALL ActivateObjectType3x3d(CONTROLTYPE3X3DPTR self, BINASSETPTR asset, CONST U32 indx, CONST U32 action, U32 param_5, U32 param_6, U32 param_7, U32 param_8, U32 param_9, U32 param_10, U32 param_11, U32 param_12); // TODO

VOID CLASSCALL InitializeObjectType3x3d(CONTROLTYPE3X3DPTR self);
VOID CLASSCALL DisableObjectType3x3d(CONTROLTYPE3X3DPTR self);
VOID CLASSCALL TickObjectType3x3d(CONTROLTYPE3X3DPTR self);
U32 CLASSCALL ActionObjectType3x3d(CONTROLTYPE3X3DPTR self);