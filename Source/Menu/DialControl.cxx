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

#include "Assets.hxx"
#include "Computer.hxx"
#include "ControlCommand.hxx"
#include "DialControl.hxx"
#include "Strings.hxx"

#include <stdlib.h>

#define MAX_MULTIPLAYER_JOIN_NAME_LENGTH    64
#define MAX_MULTIPLAYER_PLAYER_INFO_LENGTH  256

// 0x1003a5e4
DIALCONTROLSELF DialControlSelfState =
{
    (DIALCONTROLTYPEACTION)AcquireControlTypeDial,
    InitializeDialControl,
    DisableDialControl,
    (DIALCONTROLTICKACTION)TickPanelControl,
    ActionDialControl,
    (DIALCONTROLRELEASEACTION)ReleasePanelControl,
};

// 0x10011a10
DIALCONTROLPTR CLASSCALL ActivateDialControl(DIALCONTROLPTR self)
{
    self->Self = &DialControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.DialBK, &AssetsState.Assets.DialBT, CONTROLACTION_DIAL_DIAL);

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.DialSC, CONTROLACTION_SCROLL);

    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->List = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 351, 96, 223, 296, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Phone = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        CONTROLACTION_JMULTI_INPUT1, 49, 89, 115, 13, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_JOIN_NAME_LENGTH);

    self->Name = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        CONTROLACTION_JMULTI_INPUT2, 170, 89, 156, 13, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_JOIN_NAME_LENGTH);

    self->Nodes = ALLOCATE(CONTROLNODE);

    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->List);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Phone);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Name);

    return self;
}

// 0x10011c30
VOID CLASSCALL InitializeDialControl(DIALCONTROLPTR self)
{
    InitializeScrollControlArea(self->Scroll, 10, -17, 2);
    InitializePanelControl((PANELCONTROLPTR)self);

    ActivateComputer(&ComputerState, COMPUTER_SESSION_TYPE_PHONE);
    InitializeComputerPlayerList(&ComputerState, self->Items);
}

// 0x10011c70
VOID CLASSCALL DisableDialControl(DIALCONTROLPTR self)
{
    ReleaseComputer(&ComputerState);
    DisablePanelControl((PANELCONTROLPTR)self);
}

// 0x10011c90
U32 CLASSCALL ActionDialControl(DIALCONTROLPTR self)
{
    CONST U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    // TODO command is NULL when Joining Modem game

    if (action == CONTROLACTION_DIAL_DIAL)
    {
        if (AcquireInputControlValue(self->Phone)[0] != NULL) { return CONTROLACTION_DIAL_DIAL; }
    }
    else if (action == CONTROLACTION_DIAL_CANCEL) { return CONTROLACTION_DIAL_CANCEL; }

    if (command->Action == CONTROLACTION_DIAL_ADD && command->Parameter1 == CONTROLACTION_UI_CLICK)
    {
        if (AcquireInputControlValue(self->Name)[0] == NULL
            && AcquireInputControlValue(self->Phone)[0] == NULL) { return CONTROLACTION_NONE; }

        CHAR name[MAX_MULTIPLAYER_PLAYER_INFO_LENGTH];
        wsprintfA(name, "%s (%s)", AcquireInputControlValue(self->Name), AcquireInputControlValue(self->Phone));

        AppendComputerPlayer(&ComputerState, name);
        SortComputerPlayers(&ComputerState);
    }
    else
    {
        if (command->Action != CONTROLACTION_DIAL_REMOVE || command->Parameter1 != CONTROLACTION_UI_CLICK)
        {
            if (command->Action != CONTROLACTION_LIST_SELECT) { return CONTROLACTION_NONE; }

            if (command->Parameter1 == CONTROLACTION_UI_CHANGE && INVALID_LIST_CONTROL_INDEX != self->List->Index)
            {
                CHAR name[MAX_MULTIPLAYER_PLAYER_INFO_LENGTH];
                self->Items->Self->AcquireValue(self->Items, self->List->Index, name);

                LPSTR start = strrchr(name, '(');
                LPSTR end = strrchr(name, ')');

                if (start == NULL || end == NULL)
                {
                    SelectInputControlValue(self->Name, StringsState.Scratch);
                    SelectInputControlValue(self->Phone, StringsState.Scratch);
                }
                else
                {
                    start[0] = end[0] = NULL;

                    strcpy(name, start);
                    SelectInputControlValue(self->Name, name);
                    SelectInputControlValue(self->Phone, (LPSTR)((ADDR)end + 1));
                }
            }
            else
            {
                if (command->Parameter1 != CONTROLACTION_UI_SELECT) { return CONTROLACTION_NONE; }

                DequeueControlCommand(TRUE);

                return CONTROLACTION_DIAL_DIAL;
            }

            DequeueControlCommand(TRUE);

            return CONTROLACTION_NONE;
        }

        if (self->List->Index < 0)
        {
            DequeueControlCommand(TRUE);

            return CONTROLACTION_NONE;
        }

        RemoveComputerPlayer(&ComputerState, self->List->Index, FALSE);
    }

    InitializeComputerPlayerList(&ComputerState, self->Items);
    AdjustScrollControlListControl(self->List);

    self->List->Index = INVALID_LIST_CONTROL_INDEX;

    DequeueControlCommand(TRUE);

    return CONTROLACTION_NONE;
}
