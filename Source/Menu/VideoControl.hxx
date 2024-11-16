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

struct VideoControl;

typedef CONTROLTYPE(CLASSCALL* VIDEOCONTROLTYPEACTION)(VideoControl* self);
typedef VOID(CLASSCALL* VIDEOCONTROLINITIALIZEACTION)(VideoControl* self);
typedef VOID(CLASSCALL* VIDEOCONTROLDISABLEACTION)(VideoControl* self);
typedef VOID(CLASSCALL* VIDEOCONTROLTICKACTION)(VideoControl* self);
typedef U32(CLASSCALL* VIDEOCONTROLACTIONACTION)(VideoControl* self);
typedef VideoControl* (CLASSCALL* VIDEOCONTROLRELEASEACTION)(VideoControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct VideoControlSelf
{
    VIDEOCONTROLTYPEACTION          Type;
    VIDEOCONTROLINITIALIZEACTION    Initialize;
    VIDEOCONTROLDISABLEACTION       Disable;
    VIDEOCONTROLTICKACTION          Tick;
    VIDEOCONTROLACTIONACTION        Action;
    VIDEOCONTROLRELEASEACTION       Release;
} VIDEOCONTROLSELF, * VIDEOCONTROLSELFPTR;

EXTERN VIDEOCONTROLSELF VideoControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct VideoControl
{
    VIDEOCONTROLSELFPTR Self;
    U8                  IsActive;
    CHAR                Name[MAX_FILE_NAME_LENGTH];
    U32                 Max;
    U32                 Current;
    U32                 Unk05; // TODO
} VIDEOCONTROL, * VIDEOCONTROLPTR;
#pragma pack(pop)

BOOL CLASSCALL PlayVideoControl(VIDEOCONTROLPTR self, CONST S32 map, CONST S32 mission, CONST U32 param);
BOOL CLASSCALL PlayVideoControl(VIDEOCONTROLPTR self, LPCSTR name);
U32 CLASSCALL ActionVideoControl(VIDEOCONTROLPTR self);
VIDEOCONTROLPTR CLASSCALL ActivateVideoControl(VIDEOCONTROLPTR self);
VIDEOCONTROLPTR CLASSCALL ReleaseVideoControl(VIDEOCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisableVideoControl(VIDEOCONTROLPTR self);
VOID CLASSCALL InitializeVideoControl(VIDEOCONTROLPTR self);