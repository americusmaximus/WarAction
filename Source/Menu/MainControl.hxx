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

#include "BriefControl.hxx"
#include "DialControl.hxx"
#include "GreetingsControl.hxx"
#include "LoadMapControl.hxx"
#include "LoadSaveControl.hxx"
#include "MultiJoinControl.hxx"
#include "MultiJoinResultControl.hxx"
#include "MultiMapControl.hxx"
#include "MultiResultControl.hxx"
#include "MultiSelectControl.hxx"
#include "MultiStartControl.hxx"
#include "RatingsControl.hxx"
#include "SceneControl.hxx"
#include "SingleControl.hxx"
#include "SingleNewControl.hxx"
#include "SingleResultControl.hxx"
#include "VideoControl.hxx"
#include "WelcomeControl.hxx"

struct MainControl;

typedef CONTROLTYPE(CLASSCALL* MAINCONTROLTYPEACTION)(MainControl* self);
typedef VOID(CLASSCALL* MAINCONTROLINITIALIZEACTION)(MainControl* self);
typedef VOID(CLASSCALL* MAINCONTROLDISABLEACTION)(MainControl* self);
typedef VOID(CLASSCALL* MAINCONTROLTICKACTION)(MainControl* self);
typedef U32(CLASSCALL* MAINCONTROLACTIONACTION)(MainControl* self);
typedef MainControl* (CLASSCALL* MAINCONTROLRELEASEACTION)(MainControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct MainControlSelf
{
    MAINCONTROLTYPEACTION         Type;
    MAINCONTROLINITIALIZEACTION   Initialize;
    MAINCONTROLDISABLEACTION      Disable;
    MAINCONTROLTICKACTION         Tick;
    MAINCONTROLACTIONACTION       Action;
    MAINCONTROLRELEASEACTION      Release;
} MAINCONTROLSELF, * MAINCONTROLSELFPTR;

EXTERN MAINCONTROLSELF MainControlSelfState;

#pragma pack(push, 1)
// a.k.a. Skeleton
// INHERITANCE: Control
typedef struct MainControl
{
    MAINCONTROLSELFPTR              Self;
    U8                              IsActive;
    CONTROLPTR                      Active;
    U32                             Action;
    WELCOMECONTROLPTR               Welcome;
    SCENECONTROLPTR                 Multi;
    SINGLECONTROLPTR                Single;
    BRIEFCONTROLPTR                 Briefing;
    SINGLENEWCONTROLPTR             SingleNewAddon;
    SINGLENEWCONTROLPTR             SingleNewClassic;
    MULTISELECTCONTROLPTR           MultiSelect;
    MULTIJOINCONTROLPTR             MultiJoin;
    MULTIJOINRESULTCONTROLPTR       MultiJoinResult;
    RATINGSCONTROLPTR               Ratings;
    LOADSAVECONTROLPTR              LoadSave;
    LOADMAPCONTROLPTR               LoadMap;
    SINGLERESULTCONTROLPTR          SingleResult;
    DIALCONTROLPTR                  Dial;
    MULTIMAPCONTROLPTR              MultiMap;
    MULTISTARTCONTROLPTR            MultiStart;
    GREETINGSCONTROLPTR             Greetings;
    MULTIRESULTCONTROLPTR           MultiResult;
    VIDEOCONTROLPTR                 Video;
} MAINCONTROL, * MAINCONTROLPTR;
#pragma pack(pop)

MAINCONTROLPTR CLASSCALL ActivateMainControl(MAINCONTROLPTR self);
MAINCONTROLPTR CLASSCALL ReleaseMainControl(MAINCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
U32 CLASSCALL ActionMainControl(MAINCONTROLPTR self);
VOID CLASSCALL DisableMainControl(MAINCONTROLPTR self);
VOID CLASSCALL DisposeMainControl(MAINCONTROLPTR self);
VOID CLASSCALL InitializeMainControl(MAINCONTROLPTR self);
VOID CLASSCALL TickMainControl(MAINCONTROLPTR self);