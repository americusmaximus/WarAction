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
#include "Graphics.hxx"
#include "MenuSave.hxx"
#include "MultiStartControl.hxx"
#include "Network.hxx"
#include "ObjectType3x3d.hxx"
#include "Settings.hxx"
#include "State.hxx"

#include <..\Text\Resources.hxx>

#include <stdlib.h>

#define MAX_MULTIPLAYER_CHAR_MESSAGE_LENGTH 128

// 0x1003a5cc
MULTISTARTCONTROLSELF MultiStartControlSelfState =
{
    (MULTISTARTCONTROLTYPEACTION)AcquireControlTypeMultiStart,
    InitializeMultiStartControl,
    DisableMultiStartControl,
    TickMultiStartControl,
    ActionMultiStartControl,
    ReleaseMultiStartControl
};

// 0x1000fd60
MULTISTARTCONTROLPTR CLASSCALL ActivateMultiStartControl(MULTISTARTCONTROLPTR self)
{
    self->Self = &MultiStartControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.Multi4BK, &AssetsState.Assets.Multi4BT, CONTROLACTION_MULTI4_START);

    self->Colors = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_1060, &AssetsState.Assets.Multi4CO, FALSE);

    self->Team = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_1061, &AssetsState.Assets.Multi4NOP, TRUE);

    self->Nation = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_1064, &AssetsState.Assets.Multi4TOP, TRUE);

    self->ObjectType4x12_4 = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_1062, &AssetsState.Assets.Multi4LOP, TRUE);
    SelectRadioControlPosition(self->ObjectType4x12_4, 0, 0);

    self->ObjectType4x12_6 = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_1063, &AssetsState.Assets.Multi4ROP, TRUE);
    SelectRadioControlPosition(self->ObjectType4x12_6, 5, 5);

    self->Chat = ActivateChatControl(ALLOCATE(CHATCONTROL), CONTROLACTION_1065, 47, 288, 379, 80,
        ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.Multi4SC, CONTROLACTION_SCROLL), &AssetsState.Fonts.Main);

    STRINGVALUE name;
    AcquireSettingsValue(&name, IDS_STRING88);
    if (!AcquireSettingsValue(name, FALSE))
    {
        self->Input = ActivateInputControl(ALLOCATE(INPUTCONTROL),
            CONTROLACTION_JMULTI_INPUT1, 47, 380, 379, 15, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_CHAR_MESSAGE_LENGTH);
    }
    else
    {
        self->Input = ActivateInputControl(ALLOCATE(INPUTCONTROL),
            CONTROLACTION_JMULTI_INPUT1, 47, 377, 379, 18, &AssetsState.Fonts.Main, MAX_MULTIPLAYER_CHAR_MESSAGE_LENGTH);
    }

    self->Inactivity = ActivateTimeControl(ALLOCATE(TIMECONTROL),
        CONTROLACTION_MULTI4_INACTIVITY_PERIOD, 510, 205, 53, 15, &AssetsState.Fonts.Main);
    self->Capture = ActivateTimeControl(ALLOCATE(TIMECONTROL),
        CONTROLACTION_MULTI4_CAPTURE_DELAY, 510, 265, 53, 15, &AssetsState.Fonts.Main);
    self->TimeOut = ActivateTimeControl(ALLOCATE(TIMECONTROL),
        CONTROLACTION_MULTI4_TIMEOUT_DELAY, 510, 325, 53, 15, &AssetsState.Fonts.Main);

    self->Label1 = ActivateLabelControl(ALLOCATE(LABELCONTROL), 480, 117, 114, 20, NULL);
    self->Label1->IsCentered = TRUE;
    self->Label1->Font = &AssetsState.Fonts.Main;

    self->MapSize = ActivateLabelControl(ALLOCATE(LABELCONTROL), 532, 344, 78, 15, NULL);
    self->MapSize->IsCentered = TRUE;
    self->MapSize->Font = &AssetsState.Fonts.Main;
    self->MapSize->Color = ADJUSTCOLOR(0xC800);

    self->MapActors = ActivateLabelControl(ALLOCATE(LABELCONTROL), 539, 359, 57, 15, NULL);
    self->MapActors->IsCentered = TRUE;
    self->MapActors->Font = &AssetsState.Fonts.Main;
    self->MapActors->Color = ADJUSTCOLOR(0xC800);

    self->ObjectType2x39 = ActivateSwapControl(ALLOCATE(SWAPCONTROL),
        &AssetsState.Assets.Multi4ParmOP, 0, CONTROLACTION_1067);

    ControlType3x3dState = ActivateObjectType3x3d(ALLOCATE(CONTROLTYPE3X3D),
        &AssetsState.Assets.Multi4SP, 0, CONTROLACTION_1066, 0, 10, 5, 359, 45, 10, 45, 46);

    self->Map = ActivateMapControl(ALLOCATE(MAPCONTROL), 537, 277, 479, 151, 113, 119, self->MapSize, self->MapActors);

    self->Nodes = ALLOCATE(CONTROLNODE);

    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Colors);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Team);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Nation);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->ObjectType4x12_4);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->ObjectType4x12_6);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Chat);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Input);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Label1);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->MapSize);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->MapActors);
    AppendControlNode(self->Nodes, (CONTROLPTR)ControlType3x3dState);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->ObjectType2x39);

    for (U32 x = 0; x < MAX_PLAYER_COUNT; x++)
    {
        self->Players[x] = ActivateLabelControl(ALLOCATE(LABELCONTROL), 59, 86 + 15 * x, 389, 15, NULL);
        AppendControlNode(self->Nodes, (CONTROLPTR)self->Players[x]);
    }

    ((TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self->Nation, 0))->Click = "ack_ally"; // TODO
    ((TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self->Nation, 1))->Click = "ack_deu"; // TODO
    ((TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self->Nation, 2))->Click = "ack_rus"; // TODO

    return self;
}

// 0x10010480
VOID CLASSCALL InitializeMultiStartControl(MULTISTARTCONTROLPTR self)
{
    InitializeScrollControlArea((SCROLLCONTROLPTR)self->Chat->Scroll, 10, -5, 2);
    InitializePanelControl((PANELCONTROLPTR)self);

    self->Map->Self->Initialize(self->Map);
    self->Inactivity->Self->Initialize(self->Inactivity);
    self->Capture->Self->Initialize(self->Capture);
    self->TimeOut->Self->Initialize(self->TimeOut);

    SelectTimeControlModifiable(self->Inactivity, NetworkState.Connection.IsHost);
    SelectTimeControlModifiable(self->Capture, NetworkState.Connection.IsHost);
    SelectTimeControlModifiable(self->TimeOut, NetworkState.Connection.IsHost);

    self->Unk0x7d = 0; // TODO

    MultiStartControlUnknown2(self);
    MultiStartControlUnknown1(self);

    SlectLabelControlText(self->Label1, NULL);

    ControlType3x3dState->Unk15 = NetworkState.Connection.IsHost;

    self->Unk0x79 = 0; // TODO

    if (NetworkState.Connection.IsHost) { ControlType3x3dState->Unk14 = MultiplayerSettingsState.MultiSpeed; }

    CONST U32 count = AcquireBinAssetItemCount(&AssetsState.Assets.Rombik);

    for (U32 x = 0; x < count; x++) { ((IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Rombik, x))->X = 0; }
}

// 0x10010600
VOID CLASSCALL DisableMultiStartControl(MULTISTARTCONTROLPTR self)
{
    DisablePanelControl((PANELCONTROLPTR)self);

    STRINGVALUE name, value;
    AcquireSettingsValue(&name, IDS_MULTI_SPEED);
    AcquireStringValue(&value, "%d", ControlType3x3dState->Unk14);
    SaveSettingsValue(name, value);

    self->Map->Self->Disable(self->Map);
    self->Inactivity->Self->Disable(self->Inactivity);
    self->Capture->Self->Disable(self->Capture);
    self->TimeOut->Self->Disable(self->TimeOut);
}

// 0x10010fb0
VOID CLASSCALL TickMultiStartControl(MULTISTARTCONTROLPTR self)
{
    // TODO NOT IMPLEMENTED
    /*
    FUN_100106a0(self);
    FUN_10010710(self);
    FUN_10010730(self);

    TickPanelControl((PANELCONTROLPTR)self);

    State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
        (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Multi4ParmBK, self->ObjectType2x39->Unk0x39));
    State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
        (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Multi4ParmNM, self->ObjectType2x39->Unk0x39));

    for (U32 x = 0; x < MAX_PLAYER_COUNT; x++)
    {
        TODO
    }


    TODO
    */
}

// 0x100107b0
U32 CLASSCALL ActionMultiStartControl(MULTISTARTCONTROLPTR self)
{
    // TODO NOT IMPLEMENTED

    return 0;
}

// 0x10010460
MULTISTARTCONTROLPTR CLASSCALL ReleaseMultiStartControl(MULTISTARTCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeMultiStartControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10010570
VOID CLASSCALL DisposeMultiStartControl(MULTISTARTCONTROLPTR self)
{
    self->Self = &MultiStartControlSelfState;

    if (self->Map != NULL) { self->Map->Self->Release(self->Map, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Inactivity != NULL) { self->Inactivity->Self->Release(self->Inactivity, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Capture != NULL) { self->Capture->Self->Release(self->Capture, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->TimeOut != NULL) { self->TimeOut->Self->Release(self->TimeOut, OBJECTRELEASETYPE_ALLOCATED); }

    DisposePanelControl((PANELCONTROLPTR)self);
}

// 0x10010760
VOID CLASSCALL MultiStartControlUnknown1(MULTISTARTCONTROLPTR self)
{
    for (U32 x = 0; x < MAX_PLAYER_COUNT; x++) // TODO Is this correct?
    {
        TOGGLECONTROLPTR control =
            (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self->ObjectType4x12_4, x);

        control->IsAction = FALSE;
        control->IsClickable = !NetworkState.Connection.IsHost || MenuSaveState.Save.Unk1A4 != 0 ? 0 : 1; // TODO
    }
}

// 0x100115a0
VOID CLASSCALL MultiStartControlUnknown2(MULTISTARTCONTROLPTR self)
{
    BUTTONCONTROLPTR b1 = AcquireSceneControlButton(self->Scene, 0); // TODO names
    BUTTONCONTROLPTR b2 = AcquireSceneControlButton(self->Scene, 1); // TODO names
    BUTTONCONTROLPTR b3 = AcquireSceneControlButton(self->Scene, 2); // TODO names
    BUTTONCONTROLPTR b4 = AcquireSceneControlButton(self->Scene, 3); // TODO names

    SelectButtonControlVisibility(b2, TRUE);

    // TODO NOT IMPLEMENTED
    /*

    if (NetworkState.Connection.IsHost
        &&
        (State.Players.Players[NetworkPlayerState0x20Count ? ].Unk114 != '\0')) {
        piVar2 = &NetworkPlayerState0x20Count ? ;
        iVar3 = 0x10;
        bVar1 = true;
        do {
            if ((NetworkPlayer_ARRAY_100483d1[*piVar2].ID != 0) &&
                (State.Players.Players[*piVar2].Unk114 == '\0')) {
                bVar1 = false;
            }
            piVar2 = piVar2 + 1;
            iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        SelectButtonControlVisibility(b3, !bVar1);
        iVar3 = FUN_1001c550();
        SelectButtonControlVisibility(b1, 0 < iVar3);
        SelectButtonControlVisibility(b4, FALSE);
    }
    else
    {
        SelectButtonControlVisibility(b4, TRUE);
        b4->Unk11 = TRUE;

        SelectButtonControlVisibility(b3, FALSE);
        SelectButtonControlVisibility(b1, FALSE);

        if (DAT_1004834c._3_1_ != State.Players.Players[NetworkPlayerState0x20Count ? ].Unk114) {
            b4->IsAction = TRUE;
            b4->Unk11 = FALSE;
            DAT_10046cb0 = 1;
        }
        else  if (DAT_10046cb0 != '\0')
        {
            b4->Unk11 = TRUE;
            b4->IsAction = FALSE;
            DAT_10046cb0 = '\0';
        }
    }
    */
}