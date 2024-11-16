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
#include "Statistics.hxx"

#define INVALID_STAR_INDEX      (-1)
#define MAX_STAR_INDEX          4

#define STAR_CONTROL_STARS_ASSET        0
#define STAR_CONTROL_STARS_GOLD_ASSET   1
#define STAR_CONTROL_STARS_MEDAL_ASSET  2

struct StarControl;

typedef CONTROLTYPE(CLASSCALL* STARCONTROLTYPEACTION)(StarControl* self);
typedef VOID(CLASSCALL* STARCONTROLINITIALIZEACTION)(StarControl* self);
typedef VOID(CLASSCALL* STARCONTROLDISABLEACTION)(StarControl* self);
typedef VOID(CLASSCALL* STARCONTROLTICKACTION)(StarControl* self);
typedef U32(CLASSCALL* STARCONTROLACTIONACTION)(StarControl* self);
typedef StarControl* (CLASSCALL* STARCONTROLRELEASEACTION)(StarControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct StarControlSelf
{
    STARCONTROLTYPEACTION       Type;
    STARCONTROLINITIALIZEACTION Initialize;
    STARCONTROLDISABLEACTION    Disable;
    STARCONTROLTICKACTION       Tick;
    STARCONTROLACTIONACTION     Action;
    STARCONTROLRELEASEACTION    Release;
} STARCONTROLSELF, * STARCONTROLSELFPTR;

EXTERN STARCONTROLSELF StarControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct StarControl
{
    STARCONTROLSELFPTR  Self;
    U8                  IsActive;
    S32                 Index;
    U32                 Width;
    U32                 Bottom;
    U32                 Asset;
    STATISTICSAREAPTR   Area;
} STARCONTROL, * STARCONTROLPTR;
#pragma pack(pop)

EXTERN STARCONTROL StarControlState;

STARCONTROLPTR CLASSCALL ActivateStarControl(STARCONTROLPTR self);
VOID CLASSCALL InitializeStarControl(STARCONTROLPTR self);
VOID CLASSCALL TickStarControl(STARCONTROLPTR self);
U32 CLASSCALL AcquireStarControlAssetHeight(STARCONTROLPTR self);
U32 CLASSCALL AcquireStarControlAssetWidth(STARCONTROLPTR self);