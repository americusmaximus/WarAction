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
#include "Logger.hxx"
#include "MainControl.hxx"
#include "MenuSave.hxx"
#include "MessageControl.hxx"
#include "Messages.hxx"
#include "Module.hxx"
#include "Network.hxx"
#include "Saves.hxx"
#include "Settings.hxx"
#include "Sound.hxx"
#include "State.hxx"
#include "Strings.hxx"
#include "TextAsset.hxx"
#include "Unknown.hxx"

#include <..\Text\Resources.hxx>

#include <stdio.h>
#include <stdlib.h>

// 0x1003a65c
MAINCONTROLSELF MainControlSelfState =
{
    (MAINCONTROLTYPEACTION)AcquireControlTypeMain,
    InitializeMainControl,
    (MAINCONTROLDISABLEACTION)DisableMainControl,
    TickMainControl,
    ActionMainControl,
    ReleaseMainControl,
};

// 0x100131c0
MAINCONTROLPTR CLASSCALL ActivateMainControl(MAINCONTROLPTR self)
{
    self->Self = &MainControlSelfState;

    self->IsActive = FALSE;
    self->Active = NULL;

    self->Welcome = ActivateWelcomeControl(ALLOCATE(WELCOMECONTROL));
    self->Single = ActivateSingleControl(ALLOCATE(SINGLECONTROL));
    self->SingleNewAddon = ActivateSingleNewControl(ALLOCATE(SINGLENEWCONTROL), TRUE);
    self->SingleNewClassic = ActivateSingleNewControl(ALLOCATE(SINGLENEWCONTROL), FALSE);
    self->LoadSave = ActivateLoadSaveControl(ALLOCATE(LOADSAVECONTROL));
    self->LoadMap = ActivateLoadMapControl(ALLOCATE(LOADMAPCONTROL));
    self->SingleResult = ActivateSingleResultControl(ALLOCATE(SINGLERESULTCONTROL));
    self->Dial = ActivateDialControl(ALLOCATE(DIALCONTROL));
    self->ObjectType4x5d = ActivateObjectType4x5d(ALLOCATE(CONTROLTYPE4X5D));
    self->Greetings = ActivateGreetingsControl(ALLOCATE(GREETINGSCONTROL));
    self->Multi = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.Multi1BK, &AssetsState.Assets.Multi1BT, CONTROLACTION_MULTI1_INTERNET);
    self->MultiSelect = ActivateMultiSelectControl(ALLOCATE(MULTISELECTCONTROL));
    self->MultiJoin = ActivateMultiJoinControl(ALLOCATE(MULTIJOINCONTROL));
    self->MultiJoinResult = ActivateMultiJoinResultControl(ALLOCATE(MULTIJOINRESULTCONTROL));
    self->MultiMap = ActivateMultiMapControl(ALLOCATE(MULTIMAPCONTROL));
    self->MultiStart = ActivateMultiStartControl(ALLOCATE(MULTISTARTCONTROL));
    self->Video = ActivateVideoControl(ALLOCATE(VIDEOCONTROL));
    self->MultiResult = ActivateMultiResultControl(ALLOCATE(MULTIRESULTCONTROL));
    self->Ratings = ActivateRatingsControl(ALLOCATE(RATINGSCONTROL));

    return self;
}

// 0x100136f0
VOID CLASSCALL InitializeMainControl(MAINCONTROLPTR self)
{
    InitializeControl((CONTROLPTR)self);

    ControlActionQueueState.Count = 0;
    EnqueueControlActionQueue(CONTROLACTION_NONE);

    if (AcquireGameStatus() == STATUS_NONE)
    {
        DeleteGameState();

        if (InitializeNetworkModuleState(&NetworkState.State) != 0 /* TODO */)
        {
            LogMessage("LOBBY: I am lobbied!\n");

            {
                COMPUTER computer;
                ZeroMemory(&computer, sizeof(COMPUTER));

                ActivateComputer(&computer, COMPUTER_SESSION_TYPE_PLAYER);
                RemoveComputerPlayer(&computer, State.Name, FALSE);
                AppendComputerPlayer(&computer, State.Name);
                SaveComputerPlayer(&computer, State.Name);
                ReleaseComputer(&computer);
            }

            DAT_1003fba4 = 4; // TODO

            if (!MustCreateNetworkModuleStateSession(&NetworkState.State))
            {
                DAT_10046f7c = 0; // TODO

                self->Action = CONTROLACTION_INITIALIZE_NETWORK;

                return;
            }

            DAT_10046f7c = 1; // TODO

            self->Action = CONTROLACTION_MULTI3_OK;

            return;
        }

        LogMessage("LOBBY: I am NOT lobbied!\n");

        CHAR ip[256]; // TODO
        if (!AcquireStartArguments("ip", ip, 256)) { self->Action = CONTROLACTION_MAIN_SINGLE; }
        else
        {
            LogMessage("Command line IP specified!\n");

            SelectMultiJoinControlInputValue(self->MultiJoin, ip);

            EnqueueControlActionQueue(CONTROLACTION_MAIN_SINGLE);

            self->Action = CONTROLACTION_MULTI2_CREATE;

            DAT_1003fba4 = 0; // TODO
        }

        if (!PlayVideoControl(self->Video, "@ShortIntro")) { return; }

        EnqueueControlActionQueue(CONTROLACTION_PLAY_SHORT_INTRO2);
    }
    else
    {
        if (State.Module->Game.IsNetwork)
        {
            EnqueueControlActionQueue(CONTROLACTION_MAIN_SINGLE);

            self->Action = CONTROLACTION_MSTAT_DETAIL;

            return;
        }

        if (AcquireGameStatus() == STATUS_LOADING) { return; }

        LogMessage("GECK: curMap=%d, curMis=%d, nextMap=%d, nextMis=%d\n",
            AcquireCurrentGameMap(), AcquireCurrentGameMission(),
            State.Module->Game.NextMap, State.Module->Game.NextMission);

        self->Action = CONTROLACTION_SINGLE5_CONTINUE;

        if (!PlayVideoControl(self->Video,
            AcquireCurrentGameMap(), AcquireCurrentGameMission(),
            AcquireGameStatus() == STATUS_VICTORY ? VICTORY_VIDEO_TEXT_ASSET_PARAM : DEFEAT_VIDEO_TEXT_ASSET_PARAM))
        {
            return;
        }

        EnqueueControlActionQueue(self->Action);
    }

    self->Action = CONTROLACTION_PLAY_COMPLETED;
}

// 0x10013950
VOID CLASSCALL DisableMainControl(MAINCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);
}

// 0x10013930
VOID CLASSCALL TickMainControl(MAINCONTROLPTR self)
{
    if (self->Active != NULL) { self->Active->Self->Tick(self->Active); }
}

// 0x10013960
U32 CLASSCALL ActionMainControl(MAINCONTROLPTR self)
{
    ActionControl((CONTROLPTR)self);

    U32 action = self->Action;

    switch (action)
    {
    case CONTROLACTION_NONE:
    case CONTROLACTION_BRIEF:
    case CONTROLACTION_INITIALIZE_NETWORK:
    case CONTROLACTION_1087:
    case CONTROLACTION_1088:
    case CONTROLACTION_1089:
    case CONTROLACTION_1090:
    case CONTROLACTION_1091:
    case CONTROLACTION_1092:
    case CONTROLACTION_PLAY_SHORT_INTRO2:
    case CONTROLACTION_PLAY_SHORT_INTRO3:
    case CONTROLACTION_1119: { self->Active = NULL; action = self->Action; break; }
    default:
    {
        switch (action)
        {
        case CONTROLACTION_MAIN_SINGLE: { self->Active = (CONTROLPTR)self->Welcome; break; }
        case CONTROLACTION_SINGLE0_NEW: { self->Active = (CONTROLPTR)self->Single; break; }
        case CONTROLACTION_PLAY_COMPLETED: { self->Active = (CONTROLPTR)self->Video; break; }
        case CONTROLACTION_1016: { self->Active = (CONTROLPTR)self->SingleNewClassic; break; }
        case CONTROLACTION_SINGLE2_BRITISH: { self->Active = (CONTROLPTR)self->SingleNewAddon; break; }
        case CONTROLACTION_SINGLE3_LOAD: { self->Active = (CONTROLPTR)self->LoadSave; break; }
        case CONTROLACTION_SINGLE4_LOAD: { self->Active = (CONTROLPTR)self->LoadMap; break; }
        case CONTROLACTION_SINGLE5_CONTINUE: { self->Active = (CONTROLPTR)self->SingleResult; break; }
        case CONTROLACTION_DIAL_DIAL: { self->Active = (CONTROLPTR)self->Dial; break; }
        case CONTROLACTION_MULTI1_INTERNET: { self->Active = (CONTROLPTR)self->Multi; break; }
        case CONTROLACTION_MULTI2_CREATE: { self->Active = (CONTROLPTR)self->MultiSelect; break; }
        case CONTROLACTION_JMULTI1_OK: { self->Active = (CONTROLPTR)self->MultiJoin; break; }
        case CONTROLACTION_JMULTI2_JOIN: { self->Active = (CONTROLPTR)self->MultiJoinResult; break; }
        case CONTROLACTION_MULTI3_OK: { self->Active = (CONTROLPTR)self->MultiMap; break; }
        case CONTROLACTION_MULTI4_START: { self->Active = (CONTROLPTR)self->MultiStart; break; }
        case CONTROLACTION_GREETINGS: { self->Active = (CONTROLPTR)self->Greetings; break; }
        case CONTROLACTION_RATING_CAMPAIGN: { self->Active = (CONTROLPTR)self->Ratings; break; }
        case CONTROLACTION_BRIEF_OK: { self->Active = (CONTROLPTR)self->ObjectType4x5d; break; }
        case CONTROLACTION_MSTAT_DETAIL: { self->Active = (CONTROLPTR)self->MultiResult; break; }
        }

        if (self->Active != NULL)
        {
            if (!self->Active->IsActive)
            {
                ReleaseSoundStateTracks(&SoundState.State, FALSE);

                self->Active->Self->Initialize(self->Active);

                InitializeSoundStateBackBuffer(&SoundState.State, FALSE);
            }

            action = self->Active->Self->Action(self->Active);
        }
    }
    }

    BOOL check1 = FALSE; // TODO
    BOOL check2 = FALSE; // TODO

    switch (action)
    {
    case CONTROLACTION_NONE:
    {
        if (State.App->InitModule != LAUNCHER_MODULE_STATE_INDEX)
        {
            if (self->Active != NULL) { self->Active->Self->Disable(self->Active); }

            self->Active = NULL;
        }

        return self->Action == CONTROLACTION_NONE && !MessageControlState.IsVisible;
    }
    case CONTROLACTION_MAIN_SINGLE: { action = CONTROLACTION_SINGLE0_NEW; break; }
    case CONTROLACTION_MAIN_MULTI: { action = CONTROLACTION_MULTI1_INTERNET; PlaySoundStateSound(&SoundState.State, "mult"); break; }
    case CONTROLACTION_MAIN_INTRO: { if (PlayVideoControl(self->Video, "@LongIntro")) { action = CONTROLACTION_PLAY_COMPLETED; } break; }
    case CONTROLACTION_MAIN_CREDITS: { if (PlayVideoControl(self->Video, "@Credits")) { action = CONTROLACTION_PLAY_COMPLETED; } break; }
    case CONTROLACTION_MAIN_EXIT:
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_EXIT_GAME_QUESTION), MESSAGE_BUTTON_OKCANCEL);

        action = CONTROLACTION_1119; break;
    }
    case CONTROLACTION_MAIN_RATING: { action = CONTROLACTION_RATING_CAMPAIGN; break; }
    case CONTROLACTION_MAIN_GREETINGS:
    {
        ReleaseSoundStateTracks(&SoundState.State, TRUE);

        action = CONTROLACTION_GREETINGS; break;
    }
    case CONTROLACTION_SINGLE0_NEW: { action = CONTROLACTION_1016; break; }
    case CONTROLACTION_SINGLE0_SINGLEMISSIONS: { action = CONTROLACTION_SINGLE4_LOAD; break; }
    case CONTROLACTION_SINGLE0_LOAD: { action = CONTROLACTION_SINGLE3_LOAD; break; }
    case CONTROLACTION_SINGLE0_CANCEL:
    case CONTROLACTION_SINGLE2_CANCEL:
    case CONTROLACTION_SINGLE3_CANCEL:
    case CONTROLACTION_SINGLE4_CANCEL:
    case CONTROLACTION_DIAL_CANCEL:
    case CONTROLACTION_MULTI1_CANCEL:
    case CONTROLACTION_MULTI2_CANCEL:
    case CONTROLACTION_JMULTI1_CANCEL:
    case CONTROLACTION_MULTI3_CANCEL:
    case CONTROLACTION_RATING_CANCEL:
    case CONTROLACTION_BRIEF_MENU:
    case CONTROLACTION_MSTAT_EXIT: { action = CONTROLACTION_UNKNOWN; break; }
    case CONTROLACTION_SINGLE0_ADDON_CAMPAIGNS: { action = CONTROLACTION_SINGLE2_BRITISH; break; }
    case CONTROLACTION_SINGLE2_BRITISH:
    case CONTROLACTION_SINGLE2_GERMAN:
    case CONTROLACTION_SINGLE2_RUSSIAN:
    case CONTROLACTION_SINGLE2_AMERICAN: { action = SaveSingleNewControlState((SINGLENEWCONTROLPTR)self->Active) ? CONTROLACTION_BRIEF : CONTROLACTION_1092; break; }
    case CONTROLACTION_SINGLE3_LOAD: { State.App->InitModule = GAME_MODULE_STATE_INDEX; break; }
    case CONTROLACTION_SINGLE4_LOAD: { action = FUN_10015bf0() ? CONTROLACTION_BRIEF : CONTROLACTION_1092; break; }
    case CONTROLACTION_SINGLE5_CONTINUE:
    {
        LogMessage("GECK: curMap=%d, curMis=%d, nextMap=%d, nextMis=%d\n",
            AcquireCurrentGameMap(), AcquireCurrentGameMission(),
            State.Module->Game.NextMap, State.Module->Game.NextMission);

        if (State.Module->Game.NextMap == AcquireCurrentGameMap()) { FUN_10019d10(); }
        else { SaveMap(State.Module->Game.NextMap); }

        SaveMission(State.Module->Game.NextMission);

        action = CONTROLACTION_BRIEF; break;
    }
    case CONTROLACTION_SINGLE5_REPLAY: { action = RestartModule() ? CONTROLACTION_BRIEF : CONTROLACTION_1092; break; }
    case CONTROLACTION_SINGLE5_EXIT: { action = CONTROLACTION_MAIN_SINGLE; check1 = TRUE; break; }
    case CONTROLACTION_DIAL_DIAL: { check1 = TRUE; break; }
    case CONTROLACTION_JMULTI1_OK: { action = CONTROLACTION_INITIALIZE_NETWORK; break; }
    case CONTROLACTION_MULTI1_INTERNET: { DAT_1003fba4 = 0; action = CONTROLACTION_MULTI2_CREATE; break; } // TODO
    case CONTROLACTION_MULTI1_LAN: { DAT_1003fba4 = 1; action = CONTROLACTION_MULTI2_CREATE; break; } // TODO
    case CONTROLACTION_MULTI1_MODEM: { DAT_1003fba4 = 2; action = CONTROLACTION_MULTI2_CREATE; break; } // TODO
    case CONTROLACTION_MULTI2_CREATE:
    {
        DAT_10046f7c = 1; // TODO

        strcpy(MenuSaveItem_10048880.Name, "demo");

        PlaySoundStateSound(&SoundState.State, "accept");

        action = CONTROLACTION_MULTI3_OK; check1 = FALSE; break;
    }
    case CONTROLACTION_MULTI2_JOIN:
    {
        DAT_10046f7c = 0; // TODO

        PlaySoundStateSound(&SoundState.State, "accept");

        strcpy(MenuSaveItem_10048880.Name, "???");

        // TODO SWITCH
        if (DAT_1003fba4 == 0) { action = CONTROLACTION_JMULTI1_OK; } // TODO
        else
        {
            if (DAT_1003fba4 == 1) { action = CONTROLACTION_INITIALIZE_NETWORK; } // TODO
            if (DAT_1003fba4 == 2) { action = CONTROLACTION_DIAL_DIAL; } // TODO
        }

        break;
    }
    case CONTROLACTION_JMULTI2_JOIN:
    {
        if (!InitializeNetworkClient())
        {
            ShowMessageControl(&MessageControlState,
                AcquireAssetMessage(ASSET_MESSAGE_UNABLE_TO_JOIN), MESSAGE_BUTTON_OK);
        }
        else { action = CONTROLACTION_MULTI4_START; check1 = TRUE; }

        break;
    }
    case CONTROLACTION_JMULTI2_CANCEL: { action = CONTROLACTION_UNKNOWN; ReleaseNetworkStateContainer(); break; }
    case CONTROLACTION_MULTI3_OK:
    {
        LogMessage("MULTI3_OK -> BT_CONNECT\n");

        action = CONTROLACTION_INITIALIZE_NETWORK; check1 = TRUE; break;
    }
    case CONTROLACTION_MULTI4_START:
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_STARTING_THE_GAME), MESSAGE_MODE_WAIT);

        action = CONTROLACTION_1088; check1 = TRUE; break;
    }
    case CONTROLACTION_MULTI4_CANCEL:
    {
        ReleaseNetworkStateContainer();

        if (!MessageControlState.IsVisible || (MessageControlState.Mode & MESSAGE_MODE_WAIT))
        {
            HideMessageControl(&MessageControlState);
        }

        action = CONTROLACTION_MULTI2_CREATE; check2 = TRUE; break;
    }
    case CONTROLACTION_GREETINGS_EXIT:
    case CONTROLACTION_PLAY_COMPLETED:
    case CONTROLACTION_1116:
    {
        InitializeSoundStateBackBuffer(&SoundState.State, TRUE);

        action = CONTROLACTION_UNKNOWN; break;
    }
    case CONTROLACTION_BRIEF:
    {
        EnqueueControlActionQueue(CONTROLACTION_BRIEF_OK);

        check1 = TRUE;
        action = CONTROLACTION_UNKNOWN;

        CONST S32 map = AcquireCurrentGameMap();
        CONST S32 mission = AcquireCurrentGameMission();

        if (map != DEFAULT_GAME_MAP_INDEX && mission != DEFAULT_GAME_MISSION_INDEX
            && PlayVideoControl(self->Video, map, mission, START_VIDEO_TEXT_ASSET_PARAM)) { action = CONTROLACTION_PLAY_COMPLETED; }

        break;
    }
    case CONTROLACTION_BRIEF_OK: { action = CONTROLACTION_1091; check1 = TRUE; break; }
    case CONTROLACTION_INITIALIZE_NETWORK:
    {
        if (NetworkState.State.IsActive) { CloseNetworkModuleStateConnection(&NetworkState.State); }

        if (InitializeNetwork())
        {
            BOOL BVar7 = FALSE; // TODO

            switch (DAT_1003fba4) // TODO
            {
            case 0: // TODO
                if (DAT_10046f7c == 0) // TODO
                {
                    BVar7 = NetworkModuleStateContainerUnknown1(&NetworkState.State,
                        AcquireMultiJoinControlInputValue(self->MultiJoin));
                }
                else {
                    BVar7 = NetworkModuleStateContainerUnknown1(&NetworkState.State, StringsState.Scratch);
                }
                break;
            case 1: // TODO
                BVar7 = FUN_1001e950(&NetworkState.State);
                break;
            case 2: // TODO
                if (DAT_10046f7c == 0) {
                    BVar7 = NetworkModuleStateContainerUnknown2(&NetworkState.State,
                        AcquireInputControlValue(self->Dial->Phone));
                }
                else {
                    BVar7 = NetworkModuleStateContainerUnknown2(&NetworkState.State, StringsState.Scratch);
                }
                break;
            default:
                BVar7 = 0; // TODO
                break;
            case 4:
                BVar7 = 1; // TODO
            }

            LogMessage("SKELETON: rez = %d\n", BVar7);

            if (BVar7 != 0) // TODO
            {
                if (DAT_10046f7c == 0) { action = CONTROLACTION_JMULTI2_JOIN; check1 = TRUE; break; } // TODO

                if (InitializeNetworkServer())
                {
                    switch (DAT_1003fba4) // TODO
                    {
                    case 0: // TODO
                    case 4: // TODO
                    {
                        STRINGVALUE name;
                        AcquireSettingsValue(name, IDS_PING_TCP_IP);
                        MenuSaveState.Save.MaxDelay = AcquireSettingsValue(name, 800 /* TODO */); break;
                    }
                    case 1: // TODO
                    {
                        STRINGVALUE name;
                        AcquireSettingsValue(name, IDS_PING_LAN);
                        MenuSaveState.Save.MaxDelay = AcquireSettingsValue(name, 100 /* TODO */); break;
                    }
                    case 2: // TODO
                    {
                        STRINGVALUE name;
                        AcquireSettingsValue(name, IDS_PING_MODEM);
                        MenuSaveState.Save.MaxDelay = AcquireSettingsValue(name, 400 /* TODO */); break;
                    }
                    default: { MenuSaveState.Save.MaxDelay = 500; /* TODO */ break; }
                    }

                    MenuSaveState.Save.Ticks = GetTickCount();

                    if (InitializeNetworkClient()) { action = CONTROLACTION_1087; check1 = TRUE; break; }
                }
            }
        }

        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_UNABLE_TO_INITIALIZE_NETWORK), MESSAGE_BUTTON_OK);

        action = CONTROLACTION_UNKNOWN; break;
    }
    case CONTROLACTION_1087: // TODO
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_RECEIVING_GAME_INFORMATION), MESSAGE_MODE_WAIT);

        if (FUN_1001d7c0())
        {
            HideMessageControl(&MessageControlState);

            action = CONTROLACTION_MULTI4_START;
        }

        check1 = TRUE; break;
    }
    case CONTROLACTION_1088:
    {
        CHAR message[100]; /* TODO */

        BOOL bVar26 = FALSE; // TODO

        if (!NetworkState.Connection.IsHost)
        {
            S32 time = (DAT_10048ba8 - GetTickCount()) + 2000; /* TODO */

            if (time < 0) { time = 0; }

            sprintf(message, AcquireAssetMessage(ASSET_MESSAGE_STARTING_IN_N_MILLISECONDS), time);

            bVar26 = time == 0;
        }
        else
        {
            if ((NetworkState.State.Descriptor->dwFlags & 0x20) == 0) // TODO
            {
                NetworkState.State.Descriptor->dwFlags = NetworkState.State.Descriptor->dwFlags | 0x21; // TODO
                FUN_1001e260(&NetworkState.State);
            }

            if (!NetworkState.Connection.IsHost)
            {
                S32 ms = (DAT_10048ba8 - GetTickCount()) + 2000;

                if (ms < 0) { ms = 0; }

                sprintf(message, AcquireAssetMessage(ASSET_MESSAGE_STARTING_IN_N_MILLISECONDS), ms);

                bVar26 = ms == 0;
            }
            else
            {
                sprintf(message, AcquireAssetMessage(ASSET_MESSAGE_WAITING_FOR_OTHERS_TO_RESPOND),
                    0xc - DAT_100470c8); // TODO max player count (12) - current player count?

                bVar26 = DAT_100470c8 == 0xc; // TODO max player count (12) ?
            }
        }

        if (bVar26) { action = CONTROLACTION_1089; }

        ShowMessageControl(&MessageControlState, message, MESSAGE_MODE_WAIT);

        check1 = TRUE; break;
    }
    case CONTROLACTION_1089:
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_WAITING_FOR_OTHERS_TO_RESPOND), MESSAGE_MODE_WAIT);

        // TODO NOT IMPLEMENTED
        /*
        puVar17 = auStack_2f4;
        for (iVar20 = 0xba; iVar20 != 0; iVar20 = iVar20 + -1) {
            *puVar17 = 0;
            puVar17 = puVar17 + 1;
        }

        iStack_368 = 0;
        uStack_1f0 = 0;
        puStack_364 = auStack_1cc;

        check1 = TRUE;
        iStack_1f4 = -1;
        BStack_370.Value = &DAT_100483d1;
        puVar23 = auStack_1ec;
        pbVar24 = &DAT_100480fd;

        do {

            if (*BStack_370.Value == 0) {
                iVar20 = iStack_368;
                if (DAT_100487c5 != 0) {
                    *puStack_364 = 0xffffffff;
                    puVar23 = puVar23 + 0x28;
                    puStack_364 = puStack_364 + 10;
                    iVar20 = iStack_368 + 1;
                }
            }
            else {
                puStack_364 = puStack_364 + 10;
                *(int*)(puVar23 + 0x20) = *(int*)((int)BStack_370.Value + 8);
                uVar18 = (uint)*pbVar24;
                puVar22 = puVar23 + 0x28;
                bVar2 = pbVar24[1];
                *(ushort*)(puVar23 + 0x26) =
                    (ushort)((((byte)(&DAT_1003f3e4)[uVar18 * 3] & 0xfff8) << 5 |
                        (uint)((&DAT_1003f3e5)[uVar18 * 3] & 0xfc)) << 3) |
                    (ushort)((byte)(&DAT_1003f3e6)[uVar18 * 3] >> 3);
                bVar3 = pbVar24[-1];
                puVar23[0x25] = bVar2;
                pbVar11 = pbVar24 + -0x21;
                puVar23[0x24] = bVar3;
                iVar20 = (int)puVar23 - (int)pbVar11;
                do {
                    bVar2 = *pbVar11;
                    pbVar11[iVar20] = bVar2;
                    pbVar11 = pbVar11 + 1;
                } while (bVar2 != 0);
                piVar1 = (int*)(puVar23 + 0x20);
                puVar23 = puVar22;
                iVar20 = iStack_368 + 1;
                if (*piVar1 == NetworkState.Client.ID) {
                    iStack_1f4 = iStack_368;
                }
            }
            iStack_368 = iVar20;
            if (iStack_368 == 0xc) goto LAB_1001427c; // TODO
            BStack_370.Value = BStack_370.Value + 0x20;
            pbVar24 = pbVar24 + 0x25;
        } while ((int)pbVar24 < 0x1004834d);
        if (iStack_368 < 0xc) {
            puVar17 = auStack_1cc + iStack_368 * 10;
            iVar20 = 0xc - iStack_368;
            do {
                *puVar17 = 0xffffffff;
                puVar17 = puVar17 + 10;
                iVar20 = iVar20 + -1;
            } while (iVar20 != 0);
        }

    LAB_1001427c:
        State.Module->Game.IsNetwork = TRUE;
        State.Module->Game.Unk07 = (uint)DAT_100487c5;
        State.Module->Game.Ticks = MenuSaveState.Save.Ticks;
        State.Module->Game.TurnDelay = (0xf - MenuSaveState.Save.Unk00) * 4;
        State.Module->Game.Unk09 = ((MenuSaveState.Save.MaxDelay - 1) + State.Module->Game.TurnDelay) / State.Module->Game.TurnDelay;

        puVar17 = &DAT_100485f9;
        puVar21 = &State.Module->Game.Unk02;
        for (SVar27.Value = &DAT_00000005; SVar27.Value != (char*)0x0; SVar27.Value = SVar27.Value + -1
            ) {
            *puVar21 = *puVar17;
            puVar17 = puVar17 + 1;
            puVar21 = puVar21 + 1;
        }

        {
            STRINGVALUE name, value;
            AcquireSettingsValue(&name, IDS_MULTI_MAP_DIR);
            AcquireStringValue(&value, StringsState.Scratch);

            STRINGVALUE setting;
            STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

            strcpy(auStack_2f4, actual->Value);

            ReleaseStringValue(actual);
        }

        uVar18 = 0xffffffff;
        pcVar14 = &DAT_10047fd4;
        do {
            pcVar10 = pcVar14;
            if (uVar18 == 0) break;
            uVar18 = uVar18 - 1;
            pcVar10 = pcVar14 + 1;
            cVar4 = *pcVar14;
            pcVar14 = pcVar10;
        } while (cVar4 != '\0');
        uVar18 = ~uVar18;
        iVar20 = -1;
        puVar17 = auStack_2f4;
        do {
            puVar21 = puVar17;
            if (iVar20 == 0) break;
            iVar20 = iVar20 + -1;
            puVar21 = (undefined4*)((int)puVar17 + 1);
            cVar4 = *(char*)puVar17;
            puVar17 = puVar21;
        } while (cVar4 != '\0');
        puVar17 = (undefined4*)(pcVar10 + -uVar18);
        puVar21 = (undefined4*)((int)puVar21 + -1);
        for (uVar19 = uVar18 >> 2; uVar19 != 0; uVar19 = uVar19 - 1) {
            *puVar21 = *puVar17;
            puVar17 = puVar17 + 1;
            puVar21 = puVar21 + 1;
        }
        for (uVar18 = uVar18 & 3; uVar18 != 0; uVar18 = uVar18 - 1) {
            *(undefined*)puVar21 = *(undefined*)puVar17;
            puVar17 = (undefined4*)((int)puVar17 + 1);
            puVar21 = (undefined4*)((int)puVar21 + 1);
        }

        LogMessage("SKELETON: Statring mission \"%s\"", (char*)auStack_2f4);

        iVar20 = DAT_10048391 * 0x25;
        DAT_10048643 = (&DAT_100480fd)[iVar20];
        DAT_10048642 = (&DAT_100480fc)[iVar20];
        DAT_10048644 = (&DAT_100480fe)[iVar20];
        DAT_10048641 = (undefined)DAT_10048391;
        DAT_10048621 = DAT_100485f5;
        puVar17 = &DAT_100480dc;
        puVar21 = &DAT_10048645;
        do {
            puVar13 = (undefined4*)((int)puVar17 + 0x25);
            puVar25 = puVar21;
            for (iVar20 = 8; iVar20 != 0; iVar20 = iVar20 + -1) {
                *puVar25 = *puVar17;
                puVar17 = puVar17 + 1;
                puVar25 = puVar25 + 1;
            }
            puVar17 = puVar13;
            puVar21 = puVar21 + 8;
        } while ((int)puVar13 < 0x10048298);

        {
            BINFILE file = { INVALID_BINARY_FILE_VALUE };
            OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
            WriteBinFile(&file, &MenuSaveState.Save, 0x1a4); // TODO doesn't match struct size. substruct?
            CloseBinFile(&file);
        }

        if (DAT_100487c5 == 0) {
            iVar20 = FUN_100177e0((char*)auStack_2f4);
        }
        else {
            iVar20 = FUN_100186f0((int)auStack_2f4);
        }

        if (iVar20 == 0)
        {
            action = CONTROLACTION_1092;

            ReleaseNetworkStateContainer();

            check1 = TRUE;
        }
        else { State.App->InitModule = GAME_MODULE_STATE_INDEX; }

        */
        break;
    }
    case CONTROLACTION_1091: { if (!InitializeGameState()) { action = CONTROLACTION_1092; check1 = TRUE; } break; }
    case CONTROLACTION_1092:
    {
        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_ERROR_EXPORTING_GAME), MESSAGE_BUTTON_OK);

        action = CONTROLACTION_UNKNOWN; break;
    }
    case CONTROLACTION_PLAY_SHORT_INTRO2:
    {
        check1 = TRUE;
        action = CONTROLACTION_MAIN_SINGLE;

        if (PlayVideoControl(self->Video, "@ShortIntro2"))
        {
            EnqueueControlActionQueue(CONTROLACTION_PLAY_SHORT_INTRO3);

            action = CONTROLACTION_PLAY_COMPLETED;
        }

        break;
    }
    case CONTROLACTION_PLAY_SHORT_INTRO3:
    {
        check1 = TRUE;
        action = CONTROLACTION_MAIN_SINGLE;

        if (PlayVideoControl(self->Video, "@ShortIntro3"))
        {
            EnqueueControlActionQueue(CONTROLACTION_MAIN_SINGLE);

            action = CONTROLACTION_PLAY_COMPLETED;
        }

        break;
    }
    case CONTROLACTION_1119:
    {
        check1 = TRUE;

        if (!MessageControlState.IsVisible)
        {
            if (!MessageControlState.IsOk) { action = CONTROLACTION_UNKNOWN; break; }

            State.App->InitModule = INVALID_MODULE_STATE_INDEX;
        }

        break;
    }
    }

    if (action != self->Action)
    {
        if (!MessageControlState.IsVisible || (MessageControlState.Mode & MESSAGE_MODE_WAIT)) { HideMessageControl(&MessageControlState); }

        if (self->Active != NULL) { self->Active->Self->Disable(self->Active); }

        self->Active = NULL;

        CommandControlState.ReadIndex = 0;
        CommandControlState.WriteIndex = 0;

        if (action == CONTROLACTION_UNKNOWN) { self->Action = DequeueControlActionQueue(); }
        else if (check2)
        {
            while (self->Action != action && ControlActionQueueState.Count != 0)
            {
                self->Action = DequeueControlActionQueue();
            }
        }
        else
        {
            if (!check1) { EnqueueControlActionQueue(self->Action); }

            self->Action = action;
        }
    }

    if (State.App->InitModule != LAUNCHER_MODULE_STATE_INDEX)
    {
        if (self->Active != NULL) { self->Active->Self->Disable(self->Active); }

        self->Active = NULL;
    }

    return self->Action == CONTROLACTION_NONE && !MessageControlState.IsVisible;
}

// 0x10013540
MAINCONTROLPTR CLASSCALL ReleaseMainControl(MAINCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeMainControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10013560
VOID CLASSCALL DisposeMainControl(MAINCONTROLPTR self)
{
    self->Self = &MainControlSelfState;

    if (self->Welcome != NULL) { self->Welcome->Self->Release(self->Welcome, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Single != NULL) { self->Single->Self->Release(self->Single, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->SingleNewAddon != NULL) { self->SingleNewAddon->Self->Release(self->SingleNewAddon, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->SingleNewClassic != NULL) { self->SingleNewClassic->Self->Release(self->SingleNewClassic, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->LoadSave != NULL) { self->LoadSave->Self->Release(self->LoadSave, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->LoadMap != NULL) { self->LoadMap->Self->Release(self->LoadMap, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->SingleResult != NULL) { self->SingleResult->Self->Release(self->SingleResult, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Dial != NULL) { self->Dial->Self->Release(self->Dial, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->ObjectType4x5d != NULL) { self->ObjectType4x5d->Self->Release(self->ObjectType4x5d, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Greetings != NULL) { self->Greetings->Self->Release(self->Greetings, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Multi != NULL) { self->Multi->Self->Release(self->Multi, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->MultiSelect != NULL) { self->MultiSelect->Self->Release(self->MultiSelect, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->MultiJoin != NULL) { self->MultiJoin->Self->Release(self->MultiJoin, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->MultiJoinResult != NULL) { self->MultiJoinResult->Self->Release(self->MultiJoinResult, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->MultiMap != NULL) { self->MultiMap->Self->Release(self->MultiMap, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->MultiStart != NULL) { self->MultiStart->Self->Release(self->MultiStart, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Video != NULL) { self->Video->Self->Release(self->Video, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->MultiResult != NULL) { self->MultiResult->Self->Release(self->MultiResult, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->Ratings != NULL) { self->Ratings->Self->Release(self->Ratings, OBJECTRELEASETYPE_ALLOCATED); }

    self->Self = (MAINCONTROLSELFPTR)&ControlSelfState;
}