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
#include "Logger.hxx"
#include "MessageControl.hxx"
#include "Messages.hxx"
#include "MultiJoinResultControl.hxx"

// 10046cb4
STATIC U32 DAT_10046cb4; // TODO

// 0x1003a62c
MULTIJOINRESULTCONTROLSELF MultiJoinResultControlSelfState =
{
    (MULTIJOINRESULTCONTROLTYPEACTION)AcquireControlTypeMultiJoinResult,
    InitializeMultiJoinResultControl,
    (MULTIJOINRESULTCONTROLDISABLEACTION)DisablePanelControl,
    (MULTIJOINRESULTCONTROLTICKACTION)TickPanelControl,
    ActionMultiJoinResultControl,
    (MULTIJOINRESULTCONTROLRELEASEACTION)ReleasePanelControl
};

// 0x10012490
MULTIJOINRESULTCONTROLPTR CLASSCALL ActivateMultiJoinResultControl(MULTIJOINRESULTCONTROLPTR self)
{
    self->Self = &MultiJoinResultControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.JMulti2BK, &AssetsState.Assets.JMulti2BT, CONTROLACTION_JMULTI2_JOIN);

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.JMulti2SC, CONTROLACTION_SCROLL);
    
    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->List = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 49, 87, 270, 303, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Nodes = ALLOCATE(CONTROLNODE);

    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->List);

    return self;
}

// 0x10012720
VOID CLASSCALL InitializeMultiJoinResultControl(MULTIJOINRESULTCONTROLPTR self)
{
    InitializeScrollControlArea(self->Scroll, 10, -12, 4);
    InitializePanelControl((PANELCONTROLPTR)self);

    self->Ticks = GetTickCount() - 10000;
}

// 0x10012750
U32 CLASSCALL ActionMultiJoinResultControl(MULTIJOINRESULTCONTROLPTR self)
{
    // TODO NOT IMPLEMENTED
    return CONTROLACTION_NONE;
}

// 0x100125f0
BOOL CLASSCALL ObjectType15x25Unknown(MULTIJOINRESULTCONTROLPTR self) // TODO name
{
    self->Ticks = GetTickCount();
    ReleaseStringList(self->Items);

    CONST U32 result =
        AcquireGameSessions(&NetworkState.State, &self->Sessions.Items, &self->Sessions.Count);

    if (result == 0) // TODO
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_ERROR_ENUMERATING_SESSION), MESSAGE_BUTTON_OK);

        DAT_10046cb4 = 0;

        return FALSE;
    }

    if (result == 2) // TODO
    {
        if (DAT_10046cb4 == 0)
        {
            ShowMessageControl(&MessageControlState,
                AcquireAssetMessage(ASSET_MESSAGE_DIALING), MESSAGE_MODE_WAIT | MESSAGE_BUTTON_CANCEL);

            DAT_10046cb4 = 1;
        }
        else if (MessageControlState.IsVisible == 0)
        {
            DAT_10046cb4 = 0;

            return FALSE;
        }
    }
    else if (result == 3) // TODO
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_CONNECTION_FAILED), MESSAGE_BUTTON_OK);

        DAT_10046cb4 = 0;

        return FALSE;
    }

    LogMessage("JMULTI2: gamenum=%d\n", self->Sessions.Count);

    CHAR name[256]; // TODO
    for (U32 x = 0; x < self->Sessions.Count; x++)
    {
        wsprintfA(name, "%s", self->Sessions.Items[x].Name);
        AppendStringList(self->Items, name);
    }

    AdjustScrollControlListControl(self->List);

    return TRUE;
}