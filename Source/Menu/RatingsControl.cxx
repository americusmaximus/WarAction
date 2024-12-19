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
#include "RatingsControl.hxx"
#include "Statistics.hxx"
#include "Strings.hxx"

#include <Game.hxx>

#include <stdlib.h>

#define MAX_RATINGS_CONTROL_TEXT_LENGTH 256

// 0x1003a6ec
RATINGSCONTROLSELF RatingsControlSelfState =
{
    (RATINGSCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeRatingsControl,
    DisableRatingsControl,
    TickRatingsControl,
    ActionRatingsControl,
    ReleaseRatingsControl,
};

// 0x10015ed0
RATINGSCONTROLPTR CLASSCALL ActivateRatingsControl(RATINGSCONTROLPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self,
        &AssetsState.Assets.RatingBK, &AssetsState.Assets.RatingBT, CONTROLACTION_RATING_CAMPAIGN);

    ActivateTextAsset(&self->Text);

    self->Self = &RatingsControlSelfState;

    self->Statistics = ActivateStatisticsControl(ALLOCATE(STATISTICSCONTROL));

    AppendControlNode(self->Nodes, (CONTROLPTR)self->Statistics);

    self->SingleScroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL),
        &AssetsState.Assets.RatingSC, CONTROLACTION_SCROLL);

    self->SingleStatistics = ActivateStringList(ALLOCATE(STRINGLIST));

    self->SingleList = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 355, 100, 213, 278,
        self->SingleScroll, &AssetsState.Fonts.Main, self->SingleStatistics);

    self->SinglePanel = ActivatePanelControl(ALLOCATE(PANELCONTROL));
    AppendPanelControlNode((PANELCONTROLPTR)self->SinglePanel, (CONTROLPTR)self->SingleList);

    self->CampaignScroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL),
        &AssetsState.Assets.RatingSC, CONTROLACTION_SCROLL);

    self->CampaignStatistics = ActivateStringList(ALLOCATE(STRINGLIST));

    self->CampaignList = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 355, 100, 213, 278,
        self->CampaignScroll, &AssetsState.Fonts.Main, self->CampaignStatistics);

    self->CampaignPanel = ActivatePanelControl(ALLOCATE(PANELCONTROL));
    AppendPanelControlNode((PANELCONTROLPTR)self->CampaignPanel, (CONTROLPTR)self->CampaignList);

    return self;
}

// 0x100161b0
VOID CLASSCALL InitializeRatingsControl(RATINGSCONTROLPTR self)
{
    FUN_1000cac0(&StatisticsState.State,
        AcquireStatisticsFileExtension(SINGLE_FILE_EXTENSION, GAMEDIFFICULTY_UNKNOWN), self->SingleStatistics);

    InitializeTextAsset(&self->Text, "Campaign.seq");

    CHAR text[MAX_RATINGS_CONTROL_TEXT_LENGTH];

    for (U32 x = 0; x < self->Text.Count; x++)
    {
        AcquireTextAssetString(&self->Text, x, text);

        LPSTR brk = strchr(text, '>');
        if (brk == NULL) { break; }

        AppendStringList(self->CampaignStatistics, brk + 1);
    }

    InitializeScrollControlArea(self->SingleScroll, 10, -18, 2);
    InitializeScrollControlArea(self->CampaignScroll, 10, -18, 2);

    InitializePanelControl((PANELCONTROLPTR)self);

    self->SinglePanel->Self->Initialize(self->SinglePanel);
    self->CampaignPanel->Self->Initialize(self->CampaignPanel);

    ListControlCommandUnknown1(self->SingleList);
    ListControlCommandUnknown1(self->CampaignList);

    self->Type = RATINGSTYPE_CAMPAIGN;
}

// 0x10016280
VOID CLASSCALL DisableRatingsControl(RATINGSCONTROLPTR self)
{
    DisablePanelControl((PANELCONTROLPTR)self);

    self->SinglePanel->Self->Disable(self->SinglePanel);
    self->CampaignPanel->Self->Disable(self->CampaignPanel);

    InitializeTextAsset(&self->Text, NULL);
}

// 0x100164b0
VOID CLASSCALL TickRatingsControl(RATINGSCONTROLPTR self)
{
    TickPanelControl((PANELCONTROLPTR)self);

    if (self->Statistics->Index == INVALID_STATISTICS_ITEM_INDEX)
    {
        if (self->Type == RATINGSTYPE_SINGLE) { self->SinglePanel->Self->Tick(self->SinglePanel); }
        else if (self->Type == RATINGSTYPE_CAMPAIGN) { self->CampaignPanel->Self->Tick(self->CampaignPanel); }
    }
}

// 0x100162b0
U32 CLASSCALL ActionRatingsControl(RATINGSCONTROLPTR self)
{
    switch (ActionPanelControl((PANELCONTROLPTR)self))
    {
    case CONTROLACTION_RATING_CAMPAIGN:
    {
        if (self->Type != RATINGSTYPE_CAMPAIGN)
        {
            AcquireStatisticsControlStatistics(self->Statistics, StringsState.Scratch);
            self->CampaignList->Index = INVALID_LIST_CONTROL_INDEX;
        }

        self->Type = RATINGSTYPE_CAMPAIGN;
        self->Statistics->Index = INVALID_STATISTICS_ITEM_INDEX;

        break;
    }
    case CONTROLACTION_RATING_SINGLE:
    {
        if (self->Type != RATINGSTYPE_SINGLE)
        {
            AcquireStatisticsControlStatistics(self->Statistics, StringsState.Scratch);
            self->SingleList->Index = INVALID_LIST_CONTROL_INDEX;
        }

        self->Type = RATINGSTYPE_SINGLE;
        self->Statistics->Index = INVALID_STATISTICS_ITEM_INDEX;

        break;
    }
    case CONTROLACTION_RATING_CANCEL: { return CONTROLACTION_RATING_CANCEL; }
    }

    if (self->Statistics->Index == INVALID_STATISTICS_ITEM_INDEX)
    {
        if (self->Type == RATINGSTYPE_SINGLE) { self->SinglePanel->Self->Action(self->SinglePanel); }
        else if (self->Type == RATINGSTYPE_CAMPAIGN) { self->CampaignPanel->Self->Action(self->CampaignPanel); }
    }

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_TEXT_CONTROL
        && command->Action == CONTROLACTION_LIST_SELECT && command->Parameter1 == 2 /* TODO*/)
    {
        CONST S32 indx = self->Type == RATINGSTYPE_SINGLE ? self->SingleList->Index : self->CampaignList->Index;

        if (indx != INVALID_LIST_CONTROL_INDEX)
        {
            CHAR text[MAX_RATINGS_CONTROL_TEXT_LENGTH];
            STRINGVALUE value;

            if (self->Type == RATINGSTYPE_SINGLE)
            {
                self->SingleStatistics->Self->AcquireValue(self->SingleStatistics, indx, text);

                LPSTR colon = strchr(text, ':');
                if (colon != NULL) { colon[0] = NULL; }

                AcquireStatisticsControlStatistics(self->Statistics,
                    AcquireStringValue(&value, "%s.%s", text,
                        AcquireStatisticsFileExtension(SINGLE_FILE_EXTENSION, GAMEDIFFICULTY_UNKNOWN))->Value);
            }
            else
            {
                AcquireTextAssetString(&self->Text, indx, text);

                if (text[0] == '>')
                {
                    AcquireStatisticsControlStatistics(self->Statistics, StringsState.Scratch);

                    return CONTROLACTION_NONE;
                }

                LPSTR space = strchr(text, ' ');
                if (space != NULL) { space[0] = NULL; }

                AcquireStatisticsControlStatistics(self->Statistics,
                    AcquireStringValue(&value, "%s.%s", text,
                        AcquireStatisticsFileExtension(CAMPAIGN_FILE_EXTENSION, GAMEDIFFICULTY_UNKNOWN))->Value);
            }

            ReleaseStringValue(&value);
        }
    }

    return CONTROLACTION_NONE;
}

// 0x10016100
RATINGSCONTROLPTR CLASSCALL ReleaseRatingsControl(RATINGSCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeRatingsControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x10016130
VOID CLASSCALL DisposeRatingsControl(RATINGSCONTROLPTR self)
{
    self->Self = &RatingsControlSelfState;

    if (self->CampaignPanel != NULL) { self->CampaignPanel->Self->Release(self->CampaignPanel, OBJECTRELEASETYPE_ALLOCATED); }
    if (self->SinglePanel != NULL) { self->SinglePanel->Self->Release(self->SinglePanel, OBJECTRELEASETYPE_ALLOCATED); }

    DisposeTextAsset(&self->Text);

    DisposePanelControl((PANELCONTROLPTR)self);
}