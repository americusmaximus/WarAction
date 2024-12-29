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
#include "ButtonControl.hxx"
#include "PanelControl.hxx"

struct SceneControl;

typedef CONTROLTYPE(CLASSCALL* SCENECONTROLTYPEACTION)(SceneControl* self);
typedef VOID(CLASSCALL* SCENECONTROLINITIALIZEACTION)(SceneControl* self);
typedef VOID(CLASSCALL* SCENECONTROLDISABLEACTION)(SceneControl* self);
typedef VOID(CLASSCALL* SCENECONTROLTICKACTION)(SceneControl* self);
typedef U32(CLASSCALL* SCENECONTROLACTIONACTION)(SceneControl* self);
typedef SceneControl* (CLASSCALL* SCENECONTROLRELEASEACTION)(SceneControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: PanelControlSelf
typedef struct SceneControlSelf
{
    SCENECONTROLTYPEACTION       Type;
    SCENECONTROLINITIALIZEACTION Initialize;
    SCENECONTROLDISABLEACTION    Disable;
    SCENECONTROLTICKACTION       Tick;
    SCENECONTROLACTIONACTION     Action;
    SCENECONTROLRELEASEACTION    Release;
} SCENECONTROLSELF, * SCENECONTROLSELFPTR;

EXTERN SCENECONTROLSELF SceneControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: PanelControl
typedef struct SceneControl
{
    SCENECONTROLSELFPTR Self;
    U8                  IsActive;
    CONTROLNODEPTR      Nodes;
    BINASSETPTR         Background;
    BINASSETPTR         Buttons;
} SCENECONTROL, * SCENECONTROLPTR;
#pragma pack(pop)

SCENECONTROLPTR CLASSCALL ActivateSceneControl(SCENECONTROLPTR self, BINASSETPTR background, BINASSETPTR buttons, CONST U32 action);
BUTTONCONTROLPTR CLASSCALL AcquireSceneControlButton(SCENECONTROLPTR self, CONST U32 indx);
U32 CLASSCALL AcquireSceneButtonCount(SCENECONTROLPTR self);