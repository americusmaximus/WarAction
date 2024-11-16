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

#include "ActionArea.hxx"
#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "FontAsset.hxx"
#include "Messages.hxx"
#include "StarControl.hxx"
#include "Statistics.hxx"
#include "StatisticsControl.hxx"
#include "Strings.hxx"
#include "VideoControl.hxx"

#define STATISTICS_CONTROL_ACTION_PRIORITY              0x8000 /* TODO consolidate priorities */

#define MAX_STATISTICS_CONTROL_FONT_SIZE                14
#define MAX_STATISTICS_CONTROL_STATISTICS_CLASS_COUNT   2
#define MAX_STATISTICS_CONTROL_STATISTICS_TYPE_COUNT    5
#define MAX_STATISTICS_CONTROL_TEXT_LENGTH              256

#define STATISTICS_INFANTRY_INDEX                       0
#define STATISTICS_TANKS_INDEX                          1
#define STATISTICS_ARTILLERY_INDEX                      2
#define STATISTICS_VEHICLES_INDEX                       3
#define STATISTICS_PLANES_INDEX                         4

// 0x1003a6d4
STATISTICSCONTROLSELF StatisticsControlSelfState =
{
    (STATISTICSCONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeStatisticsControl,
    DisableStatisticsControl,
    TickStatisticsControl,
    ActionStatisticsControl,
    (STATISTICSCONTROLRELEASEACTION)ReleaseVideoControl
};

// NOTE: Inlined.
STATISTICSCONTROLPTR CLASSCALL ActivateStatisticsControl(STATISTICSCONTROLPTR self)
{
    self->Self = &StatisticsControlSelfState;

    self->IsActive = FALSE;

    return self;
}

// 0x10016d60
VOID CLASSCALL InitializeStatisticsControl(STATISTICSCONTROLPTR self)
{
    for (U32 x = 0; x < MAX_STATISTICS_PLAYERS_COUNT; x++)
    {
        RECT rect;
        AcquireStatisticsControlActionRectangle(x, &rect);

        ActivateActionArea(rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1,
            8 /* TODO */, x + CONTROLACTION_STATISTICS_BASE, STATISTICS_CONTROL_ACTION_PRIORITY);
    }

    ZeroMemory(StatisticsState.Areas, MAX_STATISTICS_PLAYERS_COUNT * sizeof(STATISTICSAREA));

    AcquireStatisticsControlStatistics(self, StringsState.Scratch);
}

// 0x10016e00
VOID CLASSCALL DisableStatisticsControl(STATISTICSCONTROLPTR self)
{
    SaveStatistics(&StatisticsState.State);

    for (U32 x = 0; x < MAX_STATISTICS_PLAYERS_COUNT; x++)
    {
        DequeueControlCommand(CONTROLACTION_STATISTICS_BASE + x);
    }
}

// 0x10016e80
VOID CLASSCALL TickStatisticsControl(STATISTICSCONTROLPTR self)
{
    if (INVALID_STATISTICS_ITEM_INDEX < self->Index)
    {
        // Status
        {
            SelectFontAssetColor(&AssetsState.Fonts.Main, 0xFF, 0x00, 0x00);

            CONST STATUS status = StatisticsState.State.Players[self->Index].Status;

            U32 message = ASSET_MESSAGE_MISSION_DRAW;
            if (status == STATUS_VICTORY) { message = ASSET_MESSAGE_MISSION_ACCOMPLISHED; }
            else if (status == STATUS_DEFEAT) { message = ASSET_MESSAGE_MISSION_FAILED; }

            DrawFontAssetText(&AssetsState.Fonts.Main, 460, 92, AcquireAssetMessage(message));
        }

        // Score
        {
            SelectFontAssetColor(&AssetsState.Fonts.Main, 0xB9, 0xB9, 0x00);

            CHAR message[MAX_STATISTICS_CONTROL_TEXT_LENGTH];
            wsprintfA(message, AcquireAssetMessage(ASSET_MESSAGE_SCORE),
                StatisticsState.State.Players[self->Index].Score);

            DrawFontAssetText(&AssetsState.Fonts.Main, 460, 113, message, FONTALIGNMENT_CENTER);
        }

        DrawFontAssetText(&AssetsState.Fonts.Main, 358, 146,
            AcquireAssetMessage(ASSET_MESSAGE_SCORE_UNITS_DESTROYED), FONTALIGNMENT_LEFT);

        DrawFontAssetText(&AssetsState.Fonts.Main, 358, 246,
            AcquireAssetMessage(ASSET_MESSAGE_SCORE_UNITS_LOST), FONTALIGNMENT_LEFT);

        // Iterate 2 times:
        //  1. Destroyed unites
        //  2. Lost units
        for (U32 x = 0; x < MAX_STATISTICS_CONTROL_STATISTICS_CLASS_COUNT; x++)
        {
            SelectFontAssetColor(&AssetsState.Fonts.Main, 0x7A, 0x7A, 0x00);

            CONST U32 offset = (x == 0 ? 5 : 12) * MAX_STATISTICS_CONTROL_FONT_SIZE;

            DrawFontAssetText(&AssetsState.Fonts.Main, 409, offset + 92,
                AcquireAssetMessage(ASSET_MESSAGE_SCORE_INFANTRY), FONTALIGNMENT_LEFT);
            DrawFontAssetText(&AssetsState.Fonts.Main, 409, offset + 106,
                AcquireAssetMessage(ASSET_MESSAGE_SCORE_TANKS), FONTALIGNMENT_LEFT);
            DrawFontAssetText(&AssetsState.Fonts.Main, 409, offset + 120,
                AcquireAssetMessage(ASSET_MESSAGE_SCORE_ARTILLERY), FONTALIGNMENT_LEFT);
            DrawFontAssetText(&AssetsState.Fonts.Main, 409, offset + 134,
                AcquireAssetMessage(ASSET_MESSAGE_SCORE_VEHICLES), FONTALIGNMENT_LEFT);
            DrawFontAssetText(&AssetsState.Fonts.Main, 409, offset + 148,
                AcquireAssetMessage(ASSET_MESSAGE_SCORE_PLANES), FONTALIGNMENT_LEFT);
        }

        // Iterate 2 times:
        //  1. Destroyed unites
        //  2. Lost units
        for (U32 x = 0; x < MAX_STATISTICS_CONTROL_STATISTICS_CLASS_COUNT; x++)
        {
            SelectFontAssetColor(&AssetsState.Fonts.Main, 0xFF, 0xFF, 0x00);

            // Iterate 5 times:
            //  1. Infantry
            //  2. Tanks
            //  3. Artillery
            //  4. Vehicles
            //  5. Planes
            for (U32 xx = 0; xx < MAX_STATISTICS_CONTROL_STATISTICS_TYPE_COUNT; xx++)
            {
                U32 count = 0;
                PLAYERSTATISTICSPTR stats = x == 0
                    ? &StatisticsState.State.Players[self->Index].Wins
                    : &StatisticsState.State.Players[self->Index].Losses;

                switch (xx)
                {
                case STATISTICS_INFANTRY_INDEX: { count = stats->Infantry; break; }
                case STATISTICS_TANKS_INDEX: { count = stats->Tanks; break; }
                case STATISTICS_ARTILLERY_INDEX: { count = stats->Artillery; break; }
                case STATISTICS_VEHICLES_INDEX: { count = stats->Vehicles; break; }
                case STATISTICS_PLANES_INDEX: { count = stats->Planes; break; }
                }

                CHAR message[MAX_STATISTICS_CONTROL_TEXT_LENGTH];
                wsprintfA(message, "%d", count);

                CONST U32 offset = (x == 0 ? 5 : 12) * MAX_STATISTICS_CONTROL_FONT_SIZE;
                DrawFontAssetText(&AssetsState.Fonts.Main, 563, offset + 92, message, FONTALIGNMENT_RIGHT);
            }
        }

        // Time Elapsed
        {
            SelectFontAssetColor(&AssetsState.Fonts.Main, 0x00, 0x96, 0x00);

            CONST U32 time = (StatisticsState.State.Players[self->Index].Time + 59) / 60;

            CHAR message[MAX_STATISTICS_CONTROL_TEXT_LENGTH];
            wsprintfA(message, AcquireAssetMessage(ASSET_MESSAGE_SCORE_TIME_ELAPSED), time / 60, time % 60);
            DrawFontAssetText(&AssetsState.Fonts.Main, 460, 358, message, FONTALIGNMENT_CENTER);
        }
    }

    CONST U32 offsets[4] = { 0, 0, 0, 0 };

    // Names
    for (U32 x = 0; x < MAX_STATISTICS_PLAYERS_COUNT; x++)
    {
        if (StatisticsState.State.Players[x].Name[0] != NULL)
        {
            RECT rect = { 358, 92, 563, 0 };

            AcquireStatisticsControlActionRectangle(x, &rect);

            StarControlState.Bottom = AcquireStarControlAssetHeight(&StarControlState) / 2 + rect.top;
            StarControlState.Index = StatisticsState.State.Players[x].Index;

            CONST U32 stars = StatisticsState.State.Players[x].Stars - offsets[StatisticsState.State.Players[x].Stars];

            for (U32 xx = 0; xx < stars; xx++)
            {
                StarControlState.Width = rect.right -
                    ((AcquireStarControlAssetWidth(&StarControlState) + 4) * (9 - 2 * xx)) / 2; // TODO

                StarControlState.Asset = xx < stars ? STAR_CONTROL_STARS_GOLD_ASSET : STAR_CONTROL_STARS_ASSET;
                StarControlState.Area = &StatisticsState.Areas[x];

                TickStarControl(&StarControlState);
            }

            U32 r = 0xFF, g = 0xFF, b = 0xFF;

            if (x != self->Index) { r = 0x9B;  g = 0x9B; b = 0x9B; }

            SelectFontAssetColor(&AssetsState.Fonts.Main, r, g, b);

            CHAR message[MAX_STATISTICS_CONTROL_TEXT_LENGTH];
            wsprintfA(message, "%d. %s", x + 1, StatisticsState.State.Players[x].Name);
            DrawFontAssetText(&AssetsState.Fonts.Main, rect.left, rect.top, message, FONTALIGNMENT_LEFT);
        }
    }
}

// 0x10016e30
U32 CLASSCALL ActionStatisticsControl(STATISTICSCONTROLPTR self)
{
    CONTROLCOMMAND command;
    if (DequeueControlCommand(&command, FALSE))
    {
        CONST U32 indx = command.Command - CONTROLACTION_STATISTICS_BASE;

        if (INVALID_STATISTICS_ITEM_INDEX < indx && indx < MAX_STATISTICS_PLAYERS_COUNT)
        {
            if (StatisticsState.State.Players[indx].Name[0] != NULL) { self->Index = indx; }

            DequeueControlCommand(TRUE);
        }
    }

    return CONTROLACTION_NONE;
}

// 0x10016de0
VOID CLASSCALL AcquireStatisticsControlStatistics(STATISTICSCONTROLPTR self, LPCSTR path)
{
    AcquireStatistics(&StatisticsState.State, path);

    self->Index = INVALID_STATISTICS_ITEM_INDEX;
}

// 0x10016ca0
VOID AcquireStatisticsControlActionRectangle(CONST U32 indx, LPRECT rect)
{
    if (!StatisticsState.IsActive)
    {
        StatisticsState.IsActive = TRUE;

        StatisticsState.Rectangle.left = 72;
        StatisticsState.Rectangle.top = 97;
        StatisticsState.Rectangle.right = 258;
        StatisticsState.Rectangle.bottom = 307;

        atexit(ReleaseStatisticsControlActionRectangleAction);
    }

    CONST U32 height = AcquireStarControlAssetHeight(&StarControlState);

    U32 offset = (height + 4) * indx + StatisticsState.Rectangle.top;

    if (indx == (MAX_STATISTICS_PLAYERS_COUNT - 1)) { offset = offset + 4; }

    rect->left = StatisticsState.Rectangle.left;
    rect->top = offset;
    rect->right = StatisticsState.Rectangle.right;
    rect->bottom = offset + height + 4;
}

// 0x10016d50
VOID ReleaseStatisticsControlActionRectangleAction(VOID) { }