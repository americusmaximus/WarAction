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
#include "Graphics.hxx"
#include "Messages.hxx"
#include "MultiResultControl.hxx"
#include "State.hxx"
#include "Strings.hxx"

#define MAX_MULTI_RESULT_TEXT_LENGTH    256

#define STATISTICS_SCORE_INDEX          0
#define STATISTICS_INFANTRY_INDEX       1
#define STATISTICS_TANKS_INDEX          2
#define STATISTICS_ARTILLERY_INDEX      3
#define STATISTICS_VEHICLES_INDEX       4
#define STATISTICS_PLANES_INDEX         5

#define STATISTICS_COUNT                6

// 0x1003a644
MULTIRESULTCONTROLSELF MultiResultControlSelfState =
{
    (MULTIRESULTCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeMultiResultControl,
    DisableMultiResultControl,
    TickMultiResultControl,
    ActionMultiResultControl,
    (MULTIRESULTCONTROLRELEASEACTION)ReleasePanelControl
};

// 0x10012ac0
MULTIRESULTCONTROLPTR CLASSCALL ActivateMultiResultControl(MULTIRESULTCONTROLPTR self)
{
    self->Self = &MultiResultControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.MStatBK, &AssetsState.Assets.MStatBT, CONTROLACTION_MSTAT_DETAIL);

    self->Nodes = ALLOCATE(CONTROLNODE);
    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);

    return self;
}

// 0x10012b70
VOID CLASSCALL InitializeMultiResultControl(MULTIRESULTCONTROLPTR self)
{
    InitializePanelControl((PANELCONTROLPTR)self);

    self->Type = MULTIRESULTSTATISTICSTYPE_TOTAL;
    self->Kind = MULTIRESULTSTATISTICSKIND_LOSSES;

    ActionMultiResultControlButtons(self);
}

// 0x10012b90
VOID CLASSCALL DisableMultiResultControl(MULTIRESULTCONTROLPTR self)
{
    DisablePanelControl((PANELCONTROLPTR)self);
}

// 0x10012c60
VOID CLASSCALL TickMultiResultControl(MULTIRESULTCONTROLPTR self)
{
    TickPanelControl((PANELCONTROLPTR)self);

    U32 color = BLACK_PIXEL;
    CHAR text[MAX_MULTI_RESULT_TEXT_LENGTH];

    // TODO IMPLEMENT proper statistics printout
    // TODO Statistics to use/reuse parts
    // TODO need screenshots
    for (U32 x = 0; x < MAX_PLAYER_COUNT; x++)
    {
        if (State.Module->Game.Players[x].Name[0] == NULL) { break; }

        for (U32 xx = 0; xx < STATISTICS_COUNT; xx++)
        {
            LPCSTR message = StringsState.Scratch;

            U32 destroyed = 0, lost = 0;

            //if (self->Type == MULTIRESULTSTATISTICSTYPE_TOTAL)
            //{
            //    if (self->Kind == MULTIRESULTSTATISTICSKIND_LOSSES)
            //    {
            //          TODO
            //    }
            //    else if (self->Kind == MULTIRESULTSTATISTICSKIND_WINS)
            //    {

            //    }
            //}
            //else
            if (self->Type == MULTIRESULTSTATISTICSTYPE_DETAIL)
            {
                PLAYERSTATISTICSPTR losses = &State.Module->Game.Players[x].Losses;
                PLAYERSTATISTICSPTR wins = &State.Module->Game.Players[x].Wins;

                switch (xx)
                {
                case STATISTICS_SCORE_INDEX:
                case STATISTICS_INFANTRY_INDEX:
                case STATISTICS_TANKS_INDEX:
                case STATISTICS_ARTILLERY_INDEX:
                case STATISTICS_VEHICLES_INDEX:
                case STATISTICS_PLANES_INDEX:
                    // TODO
                    break;
                }
            }

            if (self->Kind == MULTIRESULTSTATISTICSKIND_LOSSES)
            {
                wsprintfA(text, "%d/%d", destroyed, lost);
            }
            else
            {
                wsprintfA(text, "%d", destroyed - lost);
            }

            //TODO
        }
    }

    CONST U32 time = (State.Module->Game.Time + 59) / 60;
    wsprintfA(text, AcquireAssetMessage(ASSET_MESSAGE_SCORE_TIME_ELAPSED), time / 60, time % 60);
    SelectFontAssetColor(&AssetsState.Fonts.Main, ADJUSTCOLORS(0x8000, 0x0, 0x0));
    DrawFontAssetText(&AssetsState.Fonts.Main, 576, 365, text, COMPLEXFONTALIGNMENT_CENTER);
}

// 0x10013000
U32 CLASSCALL ActionMultiResultControl(MULTIRESULTCONTROLPTR self)
{
    CONST U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    switch (action)
    {
    case CONTROLACTION_MSTAT_DETAIL:
    {
        self->Type = MULTIRESULTSTATISTICSTYPE_DETAIL;
        ActionMultiResultControlButtons(self);

        return CONTROLACTION_NONE;
    }
    case CONTROLACTION_MSTAT_TOTAL:
    {
        self->Type = MULTIRESULTSTATISTICSTYPE_TOTAL;
        ActionMultiResultControlButtons(self);

        return CONTROLACTION_NONE;
    }
    case CONTROLACTION_MSTAT_DESTROYED:
    {
        self->Kind = MULTIRESULTSTATISTICSKIND_WINS;
        ActionMultiResultControlButtons(self);

        return CONTROLACTION_NONE;
    }
    case CONTROLACTION_MSTAT_LOST:
    {
        self->Kind = MULTIRESULTSTATISTICSKIND_LOSSES;
        ActionMultiResultControlButtons(self);

        return CONTROLACTION_NONE;
    }
    }

    return action;
}

// 0x10012ba0
VOID CLASSCALL ActionMultiResultControlButtons(MULTIRESULTCONTROLPTR self)
{
    {
        BUTTONCONTROLPTR score =
            (BUTTONCONTROLPTR)AcquireSceneControlButton(self->Scene, MULTI_RESULT_TOTAL_SCORE_BUTTON_INDEX);

        score->IsActive = self->Type == MULTIRESULTSTATISTICSTYPE_DETAIL;
    }

    {
        BUTTONCONTROLPTR details =
            (BUTTONCONTROLPTR)AcquireSceneControlButton(self->Scene, MULTI_RESULT_DETAILS_BUTTON_INDEX);

        details->IsVisible = self->Type == MULTIRESULTSTATISTICSTYPE_TOTAL;

    }

    {
        BUTTONCONTROLPTR casualties =
            (BUTTONCONTROLPTR)AcquireSceneControlButton(self->Scene, MULTI_RESULT_DESTROYED_LOST_BUTTON_INDEX);

        casualties->IsVisible = self->Type == MULTIRESULTSTATISTICSTYPE_DETAIL
            && self->Kind == MULTIRESULTSTATISTICSKIND_WINS;
    }

    {
        BUTTONCONTROLPTR balance =
            (BUTTONCONTROLPTR)AcquireSceneControlButton(self->Scene, MULTI_RESULT_BALANCE_BUTTON_INDEX);

        balance->IsVisible = self->Type == MULTIRESULTSTATISTICSTYPE_DETAIL
            && self->Kind == MULTIRESULTSTATISTICSKIND_LOSSES;
    }

    {
        BUTTONCONTROLPTR label =
            (BUTTONCONTROLPTR)AcquireSceneControlButton(self->Scene, MULTI_RESULT_DESTROYED_LOST_BALANCE_LABEL_INDEX);

        label->Unk11 = 0; // TODO
        label->IsVisible = self->Type == MULTIRESULTSTATISTICSTYPE_DETAIL;
        label->IsAction = self->Kind == MULTIRESULTSTATISTICSKIND_WINS;
    }
}