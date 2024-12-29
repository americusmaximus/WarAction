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

#include "ButtonControl.hxx"
#include "PanelControl.hxx"
#include "RadioControl.hxx"

#define WELCOME_SINGLE_PLAYER_BUTTON_INDEX  0
#define WELCOME_MULTI_PLAYER_BUTTON_INDEX   1
#define WELCOME_INTRO_BUTTON_INDEX          2
#define WELCOME_CREDITS_BUTTON_INDEX        3
#define WELCOME_EXIT_BUTTON_INDEX           4
#define WELCOME_RATINGS_BUTTON_INDEX        5

struct WelcomeControl;

typedef CONTROLTYPE(CLASSCALL* WELCOMECONTROLTYPEACTION)(WelcomeControl* self);
typedef VOID(CLASSCALL* WELCOMECONTROLINITIALIZEACTION)(WelcomeControl* self);
typedef VOID(CLASSCALL* WELCOMECONTROLDISABLEACTION)(WelcomeControl* self);
typedef VOID(CLASSCALL* WELCOMECONTROLTICKACTION)(WelcomeControl* self);
typedef U32(CLASSCALL* WELCOMECONTROLACTIONACTION)(WelcomeControl* self);
typedef WelcomeControl* (CLASSCALL* WELCOMECONTROLRELEASEACTION)(WelcomeControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: SceneControlSelf
typedef struct WelcomeControlSelf
{
    WELCOMECONTROLTYPEACTION        Type;
    WELCOMECONTROLINITIALIZEACTION  Initialize;
    WELCOMECONTROLDISABLEACTION     Disable;
    WELCOMECONTROLTICKACTION        Tick;
    WELCOMECONTROLACTIONACTION      Action;
    WELCOMECONTROLRELEASEACTION     Release;
} WELCOMECONTROLSELF, * WELCOMECONTROLSELFPTR;

EXTERN WELCOMECONTROLSELF WelcomeControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: SceneControl
typedef struct WelcomeControl
{
    WELCOMECONTROLSELFPTR   Self;
    U8                      IsActive;
    CONTROLNODEPTR          Nodes;
    BINASSETPTR             Background;
    BINASSETPTR             Buttons;
    RADIOCONTROLPTR         Operations;
    RADIOCONTROLPTR         Resolutions;
    BUTTONCONTROLPTR        Greetings;
    SETTINGS                Settings;
    U32                     Unknown;
} WELCOMECONTROL, * WELCOMECONTROLPTR;
#pragma pack(pop)

WELCOMECONTROLPTR CLASSCALL ActivateWelcomeControl(WELCOMECONTROLPTR self);
U32 CLASSCALL ActionWelcomeControl(WELCOMECONTROLPTR self);
VOID CLASSCALL InitializeWelcomeControl(WELCOMECONTROLPTR self);
VOID CLASSCALL TickWelcomeControl(WELCOMECONTROLPTR self);