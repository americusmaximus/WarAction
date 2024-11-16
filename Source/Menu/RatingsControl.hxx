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

#include "ListControl.hxx"
#include "SceneControl.hxx"
#include "ScrollControl.hxx"
#include "StatisticsControl.hxx"
#include "TextAsset.hxx"

#define RATINGS_CAMPAIGN_BUTTON_INDEX           0
#define RATINGS_SINGLE_MISSIONS_BUTTON_INDEX    1
#define RATINGS_CANCEL_BUTTON_INDEX             2

struct RatingsControl;

typedef CONTROLTYPE(CLASSCALL* RATINGSCONTROLTYPEACTION)(RatingsControl* self);
typedef VOID(CLASSCALL* RATINGSCONTROLINITIALIZEACTION)(RatingsControl* self);
typedef VOID(CLASSCALL* RATINGSCONTROLDISABLEACTION)(RatingsControl* self);
typedef VOID(CLASSCALL* RATINGSCONTROLTICKACTION)(RatingsControl* self);
typedef U32(CLASSCALL* RATINGSCONTROLACTIONACTION)(RatingsControl* self);
typedef RatingsControl* (CLASSCALL* RATINGSCONTROLRELEASEACTION)(RatingsControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: SceneControlSelf
typedef struct ControlType4x45Self
{
    RATINGSCONTROLTYPEACTION        Type;
    RATINGSCONTROLINITIALIZEACTION  Initialize;
    RATINGSCONTROLDISABLEACTION     Disable;
    RATINGSCONTROLTICKACTION        Tick;
    RATINGSCONTROLACTIONACTION      Action;
    RATINGSCONTROLRELEASEACTION     Release;
} RATINGSCONTROLSELF, * RATINGSCONTROLSELFPTR;

EXTERN RATINGSCONTROLSELF RatingsControlSelfState;

typedef enum RatingsType
{
    RATINGSTYPE_SINGLE      = 0,
    RATINGSTYPE_CAMPAIGN    = 1,
    RATINGSTYPE_FORCE_DWORD = 0x7FFFFF
} RATINGSTYPE, * RATINGSTYPEPTR;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct RatingsControl
{
    RATINGSCONTROLSELFPTR   Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    BINASSETPTR             Background;
    BINASSETPTR             Buttons;
    STATISTICSCONTROLPTR    Statistics;
    PANELCONTROLPTR         SinglePanel;
    PANELCONTROLPTR         CampaignPanel;
    SCROLLCONTROLPTR        SingleScroll;
    SCROLLCONTROLPTR        CampaignScroll;
    LISTCONTROLPTR          SingleList;
    LISTCONTROLPTR          CampaignList;
    STRINGLISTPTR           SingleStatistics;
    STRINGLISTPTR           CampaignStatistics;
    TEXTASSET               Text;
    RATINGSTYPE             Type;
} RATINGSCONTROL, * RATINGSCONTROLPTR;
#pragma pack(pop)

RATINGSCONTROLPTR CLASSCALL ActivateRatingsControl(RATINGSCONTROLPTR self);
VOID CLASSCALL InitializeRatingsControl(RATINGSCONTROLPTR self);
VOID CLASSCALL DisableRatingsControl(RATINGSCONTROLPTR self);
VOID CLASSCALL TickRatingsControl(RATINGSCONTROLPTR self);
U32 CLASSCALL ActionRatingsControl(RATINGSCONTROLPTR self);
RATINGSCONTROLPTR CLASSCALL ReleaseRatingsControl(RATINGSCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeRatingsControl(RATINGSCONTROLPTR self);