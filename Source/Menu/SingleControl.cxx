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
#include "MessageControl.hxx"
#include "Messages.hxx"
#include "Saves.hxx"
#include "Settings.hxx"
#include "SingleControl.hxx"
#include "State.hxx"

#include <..\Text\Resources.hxx>

#define MAX_MULTIPLAYER_NAME_LENGTH 30

// 0x1003a71c
SINGLECONTROLSELF SingleControlSelfState =
{
    (SINGLECONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeSingleControl,
    DisableSingleControl,
    TickSingleControl,
    ActionSingleControl,
    (SINGLECONTROLRELEASEACTION)ReleasePanelControl
};

// 0x10016720
SINGLECONTROLPTR CLASSCALL ActivateSingleControl(SINGLECONTROLPTR self)
{
    ActivatePanelControl((PANELCONTROLPTR)self);

    self->Self = &SingleControlSelfState;

    self->Settings.ID = IDS_DIFFICULTY;

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, IDS_DIFFICULTY);
        self->Settings.Value = (GAMEDIFFICULTY)AcquireSettingsValue(value, GAMEDIFFICULTY_EASY);
    }

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.Single0BK, &AssetsState.Assets.Single0BT, CONTROLACTION_SINGLE0_NEW);

    self->Name = ActivateInputControl(ALLOCATE(INPUTCONTROL),
        CONTROLACTION_JMULTI_INPUT1, 50, 88, 242, 14, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_NAME_LENGTH);

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.Single0SC, CONTROLACTION_SCROLL);

    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->Names = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 354, 99, 212, 281, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Difficulty = ActivateObjectType2x39(ALLOCATE(CONTROLTYPE2X39),
        &AssetsState.Assets.Single0Dif, 0, CONTROLACTION_SINGLE0_DIFFICULTY);

    self->Nodes = ALLOCATE(CONTROLNODE);

    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Name);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Names);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Difficulty);

    return self;
}

// 0x10016920
VOID CLASSCALL InitializeSingleControl(SINGLECONTROLPTR self)
{
    InitializeScrollControlArea(self->Scroll, 10, -18, 2);

    InitializePanelControl((PANELCONTROLPTR)self);

    ActivateComputer(&ComputerState, COMPUTER_SESSION_TYPE_PLAYER);
    strcpy(State.Name, AcquireComputerPlayer(&ComputerState, 0));

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, IDS_INVALID_NAME_CHARS);
        SelectInputControlInvalidValue(self->Name, value.Value);
        ReleaseStringValue(&value);
    }

    SelectInputControlInvalidValue(self->Name, NULL);
    SelectInputControlValue(self->Name, State.Name);
    InitializeComputerPlayerList(&ComputerState, self->Items);
    ListControlCommandUnknown1(self->Names);

    self->IsMessage = FALSE;
    self->Names->Index = INVALID_LIST_CONTROL_INDEX;

    // TODO:
    // 1. palyer name cannot be typed in, and not displayed at all.
    // 2. difficulty cannot be changed...
}

// 0x10016a10
VOID CLASSCALL DisableSingleControl(SINGLECONTROLPTR self)
{
    SaveState.Difficulty = (GAMEDIFFICULTY)self->Settings.Value;

    ReleaseComputer(&ComputerState);

    DisablePanelControl((PANELCONTROLPTR)self);
}

// 0x10016c10
VOID CLASSCALL TickSingleControl(SINGLECONTROLPTR self)
{
    TickPanelControl((PANELCONTROLPTR)self);
    SelectFontAssetColor(&AssetsState.Fonts.Main, 0xC0, 0xC0, 0xC0);
    DrawFontAssetText(&AssetsState.Fonts.Main, 212, 131,
        AcquireAssetMessage(ASSET_MESSAGE_EASY + self->Settings.Value));
}

// 0x10016a30
U32 CLASSCALL ActionSingleControl(SINGLECONTROLPTR self)
{
    ActionPanelControl((PANELCONTROLPTR)self);

    if (self->IsMessage)
    {
        if (MessageControlState.IsOk)
        {
            RemoveComputerPlayer(&ComputerState, self->Names->Index, TRUE);
            InitializeComputerPlayerList(&ComputerState, self->Items);
            ListControlCommandUnknown1(self->Names);

            self->Names->Index = INVALID_LIST_CONTROL_INDEX;
        }

        self->IsMessage = FALSE;
    }

    // TODO: Delete player button gets stuck when it should silently unclick
    // when any of the message box option is clicked

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command == NULL) { return CONTROLACTION_NONE; }
    if (command->Command != CONTROLCOMMAND_TEXT_CONTROL) { return CONTROLACTION_NONE; }

    strcpy(State.Name, AcquireInputControlValue(self->Name));

    if ((command->Action == CONTROLACTION_SINGLE0_NEW && command->Parameter1 == 4) // TODO
        || (command->Action == CONTROLACTION_SINGLE0_ADDON_CAMPAIGNS && command->Parameter1 == 4) // TODO
        || (command->Action == CONTROLACTION_SINGLE0_SINGLEMISSIONS && command->Parameter1 == 4) // TODO
        || (command->Action == CONTROLACTION_SINGLE0_LOAD && command->Parameter1 == 4)) // TODO
    {
        U32 action = CONTROLACTION_NONE;

        if (AcquireInputControlValue(self->Name)[0] == NULL)
        {
            ShowMessageControl(&MessageControlState,
                AcquireAssetMessage(ASSET_MESSAGE_ENTER_YOUR_NAME), MESSAGE_BUTTON_OK);
        }
        else
        {
            action = command->Action;

            strcpy(State.Name, AcquireInputControlValue(self->Name));

            SaveComputerPlayer(&ComputerState, AcquireInputControlValue(self->Name));
        }

        DequeueControlCommand(TRUE);

        return action;
    }
    else if (command->Action == CONTROLACTION_SINGLE0_CANCEL && command->Parameter1 == 4 /* TODO */)
    {
        DequeueControlCommand(TRUE);

        return CONTROLACTION_SINGLE0_CANCEL;
    }
    else if (command->Action == CONTROLACTION_SINGLE0_DELETE && command->Parameter1 != 4 /* TODO */)
    {
        if (self->Names->Index != INVALID_LIST_CONTROL_INDEX)
        {
            self->IsMessage = TRUE;

            ShowMessageControl(&MessageControlState,
                AcquireAssetMessage(ASSET_MESSAGE_DELETE_PLAYER_AND_ALL_SAVES_QUESTION), MESSAGE_BUTTON_OKCANCEL);
        }
    }
    else if (command->Action == CONTROLACTION_LIST_SELECT && command->Parameter1 == 2 /* TODO*/)
    {
        if (self->Names->Index != INVALID_LIST_CONTROL_INDEX)
        {
            if (ControlState.Active != NULL) { RemoveInputControlFocus((INPUTCONTROLPTR)ControlState.Active); }

            SelectInputControlValue(self->Name, AcquireStringListItem(self->Items, self->Names->Index));
        }
    }
    else if (command->Action == CONTROLACTION_SINGLE0_DIFFICULTY && command->Parameter1 == 2 /* TODO */)
    {
        SaveSettingsValue(&self->Settings, (self->Settings.Value + 1) % MAX_GAMEDIFFICULTY_VALUE);
    }

    DequeueControlCommand(TRUE);

    return CONTROLACTION_NONE;
}