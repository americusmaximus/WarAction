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
#include "Computer.hxx"
#include "ControlCommand.hxx"
#include "DialControl.hxx"
#include "MessageControl.hxx"
#include "Messages.hxx"
#include "MultiSelectControl.hxx"
#include "Settings.hxx"
#include "State.hxx"
#include "Strings.hxx"

#include <../Text/Resources.hxx>

#define MAX_MULTIPLAYER_NAME_LENGTH 30

// 0x1003a5fc
MULTISELECTCONTROLSELF MultiSelectControlSelfState =
{
    (MULTISELECTCONTROLTYPEACTION)AcquireControlTypeMultiSelect,
    InitializeMultiSelectControl,
    (MULTISELECTCONTROLDISABLEACTION)DisableDialControl,
    (MULTISELECTCONTROLTICKACTION)TickPanelControl,
    ActionMultiSelectControl,
    (MULTISELECTCONTROLRELEASEACTION)ReleasePanelControl
};

// 0x10011eb0
MULTISELECTCONTROLPTR CLASSCALL ActivateMultiSelectControl(MULTISELECTCONTROLPTR self)
{
    self->Self = &MultiSelectControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.Multi2BK, &AssetsState.Assets.Multi2BT, CONTROLACTION_MULTI2_CREATE);

    self->Input = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        CONTROLACTION_JMULTI_INPUT1, 50, 88, 242, 14, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_NAME_LENGTH);

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.Multi2SC, CONTROLACTION_SCROLL);

    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->List = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 354, 99, 212, 281, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Nodes = ALLOCATE(CONTROLNODE);

    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Input);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->List);

    return self;
}

// 0x10012060
VOID CLASSCALL InitializeMultiSelectControl(MULTISELECTCONTROLPTR self)
{
    InitializeScrollControlArea(self->Scroll, 10, -18, 2);
    InitializePanelControl((PANELCONTROLPTR)self);

    ActivateComputer(&ComputerState, COMPUTER_SESSION_TYPE_PLAYER);
    strcpy(State.Name, AcquireComputerPlayer(&ComputerState, 0));

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, IDS_INVALID_NAME_CHARS);
        SelectInputControlInvalidValue(self->Input, value.Value);
        ReleaseStringValue(&value);
    }

    SelectInputControlValue(self->Input, State.Name);
    InitializeComputerPlayerList(&ComputerState, self->Items);
    ListControlCommandUnknown1(self->List);

    self->List->Index = INVALID_LIST_CONTROL_INDEX;
}

// 0x10012140
U32 CLASSCALL ActionMultiSelectControl(MULTISELECTCONTROLPTR self)
{
    ActionPanelControl((PANELCONTROLPTR)self);

    if (self->IsMessage)
    {
        if (MessageControlState.IsOk)
        {
            RemoveComputerPlayer(&ComputerState, self->List->Index, 1);
            InitializeComputerPlayerList(&ComputerState, self->Items);
            ListControlCommandUnknown1(self->List);

            self->List->Index = INVALID_LIST_CONTROL_INDEX;
        }

        self->IsMessage = FALSE;
    }

    U32 result = CONTROLACTION_NONE;

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command == NULL) { return CONTROLACTION_NONE; }
    if (command->Command != CONTROLCOMMAND_TEXT_CONTROL) { return CONTROLACTION_NONE; }

    strcpy(State.Name, AcquireInputControlValue(self->Input));

    // TODO make it pretty
    if (command->Action == CONTROLACTION_MULTI2_CREATE && command->Parameter1 == 4) // TODO
    {
        if (AcquireInputControlValue(self->Input)[0] != NULL) { result = CONTROLACTION_MULTI2_CREATE; }
    }
    else if (command->Action == CONTROLACTION_MULTI2_JOIN && command->Parameter1 == 4) // TODO
    {
        if (AcquireInputControlValue(self->Input)[0] != NULL) { result = CONTROLACTION_MULTI2_JOIN; }
    }
    else if (command->Action == CONTROLACTION_MULTI2_CANCEL && command->Parameter1 == 4) // TODO
    {
        result = CONTROLACTION_MULTI2_CANCEL;
    }
    else
    {
        if (command->Action != CONTROLACTION_MULTI2_DELETE || command->Parameter1 != 4) goto LAB_10012260; // TDOO

        if (self->List->Index != INVALID_LIST_CONTROL_INDEX)
        {
            self->IsMessage = TRUE;
            ShowMessageControl(&MessageControlState,
                AcquireAssetMessage(ASSET_MESSAGE_DELETE_PLAYER_AND_ALL_SAVES_QUESTION), MESSAGE_BUTTON_OKCANCEL);
        }
    }

    DequeueControlCommand(TRUE);

LAB_10012260: // TDOO
    if (command->Action == CONTROLACTION_LIST_SELECT
        && command->Parameter1 == 2 && self->List->Index != INVALID_LIST_CONTROL_INDEX) // TODO
    {

        if (ControlState.Active != NULL) { RemoveInputControlFocus((INPUTCONTROLPTR)ControlState.Active); }

        SelectInputControlValue(self->Input, AcquireStringListItem(self->Items, self->List->Index));
        DequeueControlCommand(TRUE);
    }

    if (result == CONTROLACTION_MULTI2_JOIN || result == CONTROLACTION_MULTI2_CREATE)
    {
        SaveComputerPlayer(&ComputerState, AcquireInputControlValue(self->Input));
    }

    return result;
}