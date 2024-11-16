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

#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "MultiJoinControl.hxx"

#define MAX_MULTIPLAYER_JOIN_NAME_LENGTH 64

// 0x1003a614
MULTIJOINCONTROLSELF MultiJoinControlSelfState =
{
    (MULTIJOINCONTROLTYPEACTION)AcquireControlTypeMultiJoin,
    (MULTIJOINCONTROLINITIALIZEACTION)InitializePanelControl,
    (MULTIJOINCONTROLDISABLEACTION)DisablePanelControl,
    (MULTIJOINCONTROLTICKACTION)TickPanelControl,
    ActionMultiJoinControl,
    (MULTIJOINCONTROLRELEASEACTION)ReleasePanelControl
};

// 0x100122f0
MULTIJOINCONTROLPTR CLASSCALL ActivateMultiJoinControl(MULTIJOINCONTROLPTR self)
{
    self->Self = &MultiJoinControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->SceneControl = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.JMulti1BK, &AssetsState.Assets.JMulti1BT, CONTROLACTION_JMULTI1_OK);

    self->Input = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        CONTROLACTION_JMULTI_INPUT1, 60, 88, 242, 14, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_JOIN_NAME_LENGTH);

    self->Nodes = ALLOCATE(CONTROLNODE);
    InitControlNode(self->Nodes, (CONTROLPTR)self->SceneControl);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Input);

    return self;
}

// 0x10012420
U32 CLASSCALL ActionMultiJoinControl(MULTIJOINCONTROLPTR self)
{
    ActionPanelControl((PANELCONTROLPTR)self);

    CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_TEXT_CONTROL)
    {
        if (command->Action == CONTROLACTION_JMULTI1_OK && command->Parameter1 == 4 /* TODO */)
        {
            DequeueControlCommand(TRUE);

            return CONTROLACTION_JMULTI1_OK;
        }

        if (command->Action == CONTROLACTION_JMULTI1_CANCEL && command->Parameter1 == 4 /* TODO */)
        {
            DequeueControlCommand(TRUE);

            return CONTROLACTION_JMULTI1_CANCEL;
        }
    }

    return CONTROLACTION_NONE;
}

// 0x100123f0
LPCSTR CLASSCALL AcquireMultiJoinControlInputValue(MULTIJOINCONTROLPTR self)
{
    return AcquireInputControlValue(self->Input);
}

// 0x10012400
VOID CLASSCALL SelectMultiJoinControlInputValue(MULTIJOINCONTROLPTR self, LPCSTR value)
{
    SelectInputControlValue(self->Input, value);
}