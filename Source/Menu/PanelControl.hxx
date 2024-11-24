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
#include "ControlNode.hxx"

struct PanelControl;

typedef CONTROLTYPE(CLASSCALL* PANELCONTROLTYPEACTION)(PanelControl* self);
typedef VOID(CLASSCALL* PANELCONTROLINITIALIZEACTION)(PanelControl* self);
typedef VOID(CLASSCALL* PANELCONTROLDISABLEACTION)(PanelControl* self);
typedef VOID(CLASSCALL* PANELCONTROLTICKACTION)(PanelControl* self);
typedef U32(CLASSCALL* PANELCONTROLACTIONACTION)(PanelControl* self);
typedef PanelControl* (CLASSCALL* PANELCONTROLRELEASEACTION)(PanelControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct PanelControlSelf
{
    PANELCONTROLTYPEACTION          Type;
    PANELCONTROLINITIALIZEACTION    Initialize;
    PANELCONTROLDISABLEACTION       Disable;
    PANELCONTROLTICKACTION          Tick;
    PANELCONTROLACTIONACTION        Action;
    PANELCONTROLRELEASEACTION       Release;
} PANELCONTROLSELF, * PANELCONTROLSELFPTR;

EXTERN PANELCONTROLSELF PanelControlSelfState;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct PanelControl
{
    PANELCONTROLSELFPTR Self;
    U8                  IsActive;
    CONTROLNODEPTR      Nodes;
} PANELCONTROL, * PANELCONTROLPTR;
#pragma pack(pop)

PANELCONTROLPTR CLASSCALL ActivatePanelControl(PANELCONTROLPTR self);
VOID CLASSCALL InitializePanelControl(PANELCONTROLPTR self);
VOID CLASSCALL DisablePanelControl(PANELCONTROLPTR self);
VOID CLASSCALL TickPanelControl(PANELCONTROLPTR self);
U32 CLASSCALL ActionPanelControl(PANELCONTROLPTR self);
PANELCONTROLPTR CLASSCALL ReleasePanelControl(PANELCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposePanelControl(PANELCONTROLPTR self);
VOID CLASSCALL AppendPanelControlNode(PANELCONTROLPTR self, CONTROLPTR node);

CONTROLPTR CLASSCALL AcquirePanelControlNode(PANELCONTROLPTR self, CONST S32 indx);