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

#include "DescriptionControl.hxx"
#include "SceneControl.hxx"

struct ControlType4x5d;

typedef CONTROLTYPE(CLASSCALL* CONTROLTYPE4X5DTYPEACTION)(ControlType4x5d* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X5DINITIALIZEACTION)(ControlType4x5d* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X5DDISABLEACTION)(ControlType4x5d* self);
typedef VOID(CLASSCALL* CONTROLTYPE4X5DTICKACTION)(ControlType4x5d* self);
typedef U32(CLASSCALL* CONTROLTYPE4X5DACTIONACTION)(ControlType4x5d* self);
typedef ControlType4x5d* (CLASSCALL* CONTROLTYPE4X5DRELEASEACTION)(ControlType4x5d* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: SceneControlSelf
typedef struct ControlType4x5dSelf
{
    CONTROLTYPE4X5DTYPEACTION Type;
    CONTROLTYPE4X5DINITIALIZEACTION Initialize;
    CONTROLTYPE4X5DDISABLEACTION Disable;
    CONTROLTYPE4X5DTICKACTION Tick;
    CONTROLTYPE4X5DACTIONACTION Action;
    CONTROLTYPE4X5DRELEASEACTION Release;
} CONTROLTYPE4X5DSELF, * CONTROLTYPE4X5DSELFPTR;

EXTERN CONTROLTYPE4X5DSELF ObjectType4x5dSelfState;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct ControlType4x5d
{
    CONTROLTYPE4X5DSELFPTR  Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    BINASSETPTR             Background;
    BINASSETPTR             Buttons;
    LPVOID                  AniFile; // TODO
    LPVOID                  ArrColFile; // TODO
    LPVOID                  SeqFile; // TODO
    LPVOID                  BkgPckFile; // TODO
    LPVOID                  BkgColFile; // TODO
    LPVOID                  ColPckFile; // TODO
    LPVOID                  ColFile; // TODO
    U32                     SequenceCount; // TODO
    U32                     SeqFileStart; // TODO
    U32                     SpriteType; // TODO
    U32                     Unk15; // TODO
    U32                     Unk16; // TODO
    U32                     Unk17; // TODO
    U32                     Unk18; // TODO
    U32                     Unk19; // TODO
    U32                     Unk20; // TODO
    U32                     Unk21; // TODO
    U32                     Ticks;
    DESCRIPTIONCONTROLPTR   Description;
} CONTROLTYPE4X5D, * CONTROLTYPE4X5DPTR;
#pragma pack(pop)

CONTROLTYPE4X5DPTR CLASSCALL ActivateObjectType4x5d(CONTROLTYPE4X5DPTR self);
VOID CLASSCALL InitializeObjectType4x5d(CONTROLTYPE4X5DPTR self);
VOID CLASSCALL DisableObjectType4x5d(CONTROLTYPE4X5DPTR self);
VOID CLASSCALL TickObjectType4x5d(CONTROLTYPE4X5DPTR self);
U32 CLASSCALL ActionObjectType4x5d(CONTROLTYPE4X5DPTR self);
CONTROLTYPE4X5DPTR CLASSCALL ReleaseObjectType4x5d(CONTROLTYPE4X5DPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeObjectType4x5d(CONTROLTYPE4X5DPTR self);

BOOL FUN_10018ba0(LPSTR param_1, LPSTR param_2); // TODO name
U32 InitializeSprite(LPCSTR name, LPVOID* content); // TODO name, types
S32 OpenBinaryFile(LPCSTR name, LPVOID* content); // TODO name, types