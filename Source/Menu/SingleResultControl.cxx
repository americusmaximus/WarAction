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
#include "Logger.hxx"
#include "Saves.hxx"
#include "SingleResultControl.hxx"
#include "State.hxx"
#include "Statistics.hxx"

// 0x1003a704
SINGLERESULTCONTROLSELF SingleResultControlSelfState =
{
    (SINGLERESULTCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeSingleResultControl,
    (SINGLERESULTCONTROLDISABLEACTION)DisablePanelControl,
    (SINGLERESULTCONTROLTICKACTION)TickPanelControl,
    (SINGLERESULTCONTROLACTIONACTION)ActionPanelControl,
    (SINGLERESULTCONTROLRELEASEACTION)ReleasePanelControl,
};

// 0x100164e0
SINGLERESULTCONTROLPTR CLASSCALL ActivateSingleResultControl(SINGLERESULTCONTROLPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self, &AssetsState.Assets.Single5BK,
        &AssetsState.Assets.Single5BT, CONTROLACTION_SINGLE5_CONTINUE);

    self->Self = &SingleResultControlSelfState;

    self->Statistics = ActivateStatisticsControl(ALLOCATE(STATISTICSCONTROL));

    AppendControlNode(self->Nodes, (CONTROLPTR)self->Statistics);

    return self;
}

// 0x10016560
VOID CLASSCALL InitializeSingleResultControl(SINGLERESULTCONTROLPTR self)
{
    InitializePanelControl((PANELCONTROLPTR)self);

    State.Module->Game.Players[0].Status = State.Module->Game.Status;
    State.Module->Game.Players[0].Time = State.Module->Game.Time;

    strcpy(State.Name, State.Module->Game.Players[0].Name);

    BINFILE file = { (BFH)INVALID_HANDLE_VALUE };
    if (!OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_READ))
    {
        LogMessage("cannot open '%s'\n", "XCHNG\\menu.sav");
        return;
    }

    U32 magic = 0;
    ReadBinFile(&file, &magic, sizeof(U32));
    ReadBinFile(&file, &SaveState, sizeof(SAVEMODULESTATECONTAINER));
    SaveGameState(&file);
    CloseBinFile(&file);

    CHAR path[MAX_FILE_NAME_LENGTH];

    if (SaveState.Path[0] == NULL)
    {
        wsprintfA(path, "%03d%03d.%s", AcquireCurrentGameMap(), AcquireCurrentGameMission(),
            AcquireStatisticsFileExtension(CAMPAIGN_FILE_EXTENSION, SaveState.Difficulty));
    }
    else
    {
        strcpy(path, SaveState.Path);

        LPSTR dot = strrchr(path, '.');
        if (dot != NULL) { dot[0] = NULL; }

        wsprintfA((LPSTR)((ADDR)path + strlen(path)), ".%s",
            AcquireStatisticsFileExtension(SINGLE_FILE_EXTENSION, SaveState.Difficulty));
    }

    LogMessage("TopTen filename: %s\n", path);
    AcquireStatisticsControlStatistics(self->Statistics, path);

    self->Statistics->Index = AcquireStatisticsPlayerIndex(&StatisticsState.State, &State.Module->Game.Players[0]);

    if (MAX_GAME_MAP_COUNT < State.Module->Game.NextMap)
    {
        BUTTONCONTROLPTR button = AcquireSceneControlButton((SCENECONTROLPTR)self, SINGLE_RESULT_CONTINUE_BUTTON_INDEX);

        button->IsClickable = FALSE;
        button->IsAction = TRUE;
    }
}