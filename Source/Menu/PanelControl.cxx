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

#include "ButtonControl.hxx"
#include "PanelControl.hxx"

#include <stdlib.h>

// 0x1003a2c0
PANELCONTROLSELF PanelControlSelfState =
{
    (PANELCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializePanelControl,
    DisablePanelControl,
    TickPanelControl,
    ActionPanelControl,
    ReleasePanelControl
};

// NOTE: Inlined.
PANELCONTROLPTR CLASSCALL ActivatePanelControl(PANELCONTROLPTR self)
{
    self->Self = &PanelControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    return self;
}

// 0x10002060
VOID CLASSCALL InitializePanelControl(PANELCONTROLPTR self)
{
    InitializeControl((CONTROLPTR)self);

    for (CONTROLNODEPTR node = self->Nodes; node != NULL; node = node->Next)
    {
        node->Value->Self->Initialize(node->Value);
    }
}

// 0x10002080
VOID CLASSCALL DisablePanelControl(PANELCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);

    for (CONTROLNODEPTR node = self->Nodes; node != NULL; node = node->Next)
    {
        node->Value->Self->Disable(node->Value);
    }
}

// 0x100020a0
VOID CLASSCALL TickPanelControl(PANELCONTROLPTR self)
{
    for (CONTROLNODEPTR node = self->Nodes; node != NULL; node = node->Next)
    {
        node->Value->Self->Tick(node->Value);
    }
}

// 0x100020c0
U32 CLASSCALL ActionPanelControl(PANELCONTROLPTR self)
{
    U32 result = CONTROLACTION_NONE;

    for (CONTROLNODEPTR node = self->Nodes; node != NULL; node = node->Next)
    {
        CONST U32 action = node->Value->Self->Action(node->Value);

        if (action != CONTROLACTION_NONE)
        {
            CONST CONTROLTYPE type = node->Value->Self->Type(node->Value);

            if (type == CONTROLTYPE_BUTTON) { result = ((BUTTONCONTROLPTR)node->Value)->Action; }
            else if (type == CONTROLTYPE_PANEL) { result = action; }
        }
    }

    return result;
}

// 0x10002180
// 0x10011c10
PANELCONTROLPTR CLASSCALL ReleasePanelControl(PANELCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposePanelControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10002110
VOID CLASSCALL DisposePanelControl(PANELCONTROLPTR self)
{
    self->Self = (PANELCONTROLSELFPTR)&ControlSelfState;

    if (self->Nodes != NULL)
    {
        DisposeControlNode(self->Nodes);

        free(self->Nodes);
    }
}

// 0x100021a0
VOID CLASSCALL AppendPanelControlNode(PANELCONTROLPTR self, CONTROLPTR node)
{
    if (self->Nodes != NULL)
    {
        AppendControlNode(self->Nodes, node);
        return;
    }

    self->Nodes = ALLOCATE(CONTROLNODE);
    InitControlNode(self->Nodes, node);
}

// 0x100021e0
CONTROLPTR CLASSCALL AcquirePanelControlNode(PANELCONTROLPTR self, CONST U32 indx)
{
    CONTROLNODEPTR node = self->Nodes;

    for (U32 x = 0; x < indx; x++) { node = node->Next; }

    return node->Value;
}