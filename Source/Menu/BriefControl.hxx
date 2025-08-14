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

struct BriefControl;

typedef CONTROLTYPE(CLASSCALL* BRIEFCONTROLTYPEACTION)(BriefControl* self);
typedef VOID(CLASSCALL* BRIEFCONTROLINITIALIZEACTION)(BriefControl* self);
typedef VOID(CLASSCALL* BRIEFCONTROLDISABLEACTION)(BriefControl* self);
typedef VOID(CLASSCALL* BRIEFCONTROLTICKACTION)(BriefControl* self);
typedef U32(CLASSCALL* BRIEFCONTROLACTIONACTION)(BriefControl* self);
typedef BriefControl* (CLASSCALL* BRIEFCONTROLRELEASEACTION)(BriefControl* self, CONST OBJECTRELEASETYPE mode);

typedef enum BRIEFCONTROLSPRITETYPE
{
    BRIEFCONTROLSPRITETYPE_INVALID      = 0xFFFFFFFF,
    BRIEFCONTROLSPRITETYPE_NONE         = 0,
    BRIEFCONTROLSPRITETYPE_UNKNOWN_1    = 1, // TODO
    BRIEFCONTROLSPRITETYPE_UNKNOWN_2    = 2, // TODO
    BRIEFCONTROLSPRITETYPE_FORCE_DWORD = 0x7FFFFFFF
}BRIEFCONTROLSPRITETYPE, * BRIEFCONTROLSPRITETYPEPTR;

// INHERITANCE: SceneControlSelf
typedef struct BriefControlSelf
{
    BRIEFCONTROLTYPEACTION          Type;
    BRIEFCONTROLINITIALIZEACTION    Initialize;
    BRIEFCONTROLDISABLEACTION       Disable;
    BRIEFCONTROLTICKACTION          Tick;
    BRIEFCONTROLACTIONACTION        Action;
    BRIEFCONTROLRELEASEACTION       Release;
} BRIEFCONTROLSELF, * BRIEFCONTROLSELFPTR;

EXTERN BRIEFCONTROLSELF BriefControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct BriefControl
{
    BRIEFCONTROLSELFPTR     Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    BINASSETPTR             Background;
    BINASSETPTR             Buttons;
    LPVOID                  AnimationSprite;
    LPVOID                  AnimationColors;
    U32*                    Sequences;
    LPVOID                  BkgPckFile; // TODO
    LPVOID                  BkgColFile; // TODO
    LPVOID                  ColPckFile; // TODO
    LPVOID                  ColFile; // TODO
    U32                     SequenceCount;
    U32                     SequenceStart;
    BRIEFCONTROLSPRITETYPE  AnimationSpriteType;
    BRIEFCONTROLSPRITETYPE  BackgroundSpriteType;
    U32                     Unk16; // TODO
    U32                     Unk17; // TODO
    U32                     Unk18; // TODO
    U32                     Unk19; // TODO
    U32                     Unk20; // TODO
    U32                     Unk21; // TODO
    U32                     Ticks;
    DESCRIPTIONCONTROLPTR   Description;
} BRIEFCONTROL, * BRIEFCONTROLPTR;
#pragma pack(pop)

BRIEFCONTROLPTR CLASSCALL ActivateBriefControl(BRIEFCONTROLPTR self);
VOID CLASSCALL InitializeBriefControl(BRIEFCONTROLPTR self);
VOID CLASSCALL DisableBriefControl(BRIEFCONTROLPTR self);
VOID CLASSCALL TickBriefControl(BRIEFCONTROLPTR self);
U32 CLASSCALL ActionBriefControl(BRIEFCONTROLPTR self);
BRIEFCONTROLPTR CLASSCALL ReleaseBriefControl(BRIEFCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeBriefControl(BRIEFCONTROLPTR self);

BOOL AcquireCurrentGameMapMissionNames(LPSTR mapName, LPSTR missionName);
BRIEFCONTROLSPRITETYPE InitializeBriefControlSprite(LPCSTR name, LPVOID* content);
S32 AcquireBriefControlAssetContent(LPCSTR name, LPVOID* content);