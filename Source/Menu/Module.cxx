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

#include "Activate.hxx"
#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "Cursor.hxx"
#include "Graphics.hxx"
#include "Logger.hxx"
#include "MainControl.hxx"
#include "MessageControl.hxx"
#include "Messages.hxx"
#include "Module.hxx"
#include "Network.hxx"
#include "Saves.hxx"
#include "Settings.hxx"
#include "Shortcuts.hxx"
#include "Sound.hxx"
#include "State.hxx"
#include "TextAsset.hxx"
#include "Video.hxx"

#include <..\Text\Resources.hxx>

#include <Mathematics.Basic.hxx>

using namespace Mathematics;

// 0x10059b7c
CONTROLCOMMAND ModuleCommand;

// 0x10020e20
BOOL InitializeModuleAction(VOID)
{
    ClipCursor(NULL);

    State.Module = State.App->Module;
    State.Renderer = State.Module->Renderer;

    AcquireStartArguments();

    LogMessage("CA_ETEXT_Init()\n");
    AcquireIniFileState();

    LogMessage("DPLAY_Done()\n");
    ReleaseNetworkStateContainer();

    switch (AcquireGameStatus())
    {
    case STATUS_LOADING:
    {
        LogMessage("LoadSavedGame(%s)\n", State.Module->Game.SaveFile);

        if (LoadSaveState(State.Module->Game.SaveFile))
        {
            State.App->InitModule = GAME_MODULE_STATE_INDEX;

            LogMessage("return TRUE\n"); return TRUE;
        }

        LogMessage("LoadSavedGame(%s) failed.\n", State.Module->Game.SaveFile); return FALSE;
    }
    case STATUS_QUIT: { LogMessage("RK_EXITTOOS\n"); return FALSE; }
    case STATUS_RESTART: { return RestartGame() && InitializeGameState(); }
    }

    STRINGVALUE value;

    {
        STRINGVALUE name;
        AcquireSettingsValue(&name, IDS_MENU_RESOURCE);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        LogMessage("BFILE : main resources in \"%s\"\n", actual->Value);

        ReleaseStringValue(actual);
    }

    {
        STRINGVALUE name;
        AcquireSettingsValue(&name, IDS_MENU_RESOURCE);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        InitializeBinArchives(actual->Value);

        ReleaseStringValue(actual);
    }

    {
        STRINGVALUE name;
        AcquireSettingsValue(&name, IDS_BRIEFINGS);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        LogMessage("BFILE : briefings in \"%s\"\n", actual->Value);

        ReleaseStringValue(actual);
    }

    {
        STRINGVALUE name;
        AcquireSettingsValue(&name, IDS_BRIEFINGS);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        InitializeBinArchives(actual->Value);

        ReleaseStringValue(actual);
    }

    InitializeBinAsset(&AssetsState.Assets.MainBK, "main_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MainBT, "main_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single0BK, "single0_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single0BT, "single0_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single2BK, "single2_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single2BT, "single2_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.SinOld2BK, "sinold2_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.SinOld2BT, "sinold2_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single3BK, "single3_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single3BT, "single3_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single4BK, "single4_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single4BT, "single4_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single5BK, "single5_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single5BT, "single5_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.DialBK, "dial_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.DialBT, "dial_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi1BK, "multi1_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi1BT, "multi1_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi2BK, "multi2_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi2BT, "multi2_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi3BK, "multi3_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi3BT, "multi3_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4BK, "multi4_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4BT, "multi4_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.JMulti1BK, "jmulti1_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.JMulti1BT, "jmulti1_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.JMulti2BK, "jmulti2_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.JMulti2BT, "jmulti2_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.BriefBK, "brief_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.BriefBT, "brief_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MStatBK, "mstat_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MStatBT, "mstat_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.RatingBK, "rating_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.RatingBT, "rating_bt.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg0, "msg0.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg1BK, "msg1_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg1BT0, "msg1_bt0.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg1BT1, "msg1_bt1.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg1CBT0, "msg1c_bt0.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg1CBT1, "msg1c_bt1.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg2BK, "msg2_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg2BT10, "msg2_bt10.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg2BT11, "msg2_bt11.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg2BT20, "msg2_bt20.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Msg2BT21, "msg2_bt21.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.GreetingsBK, "greetings_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Rombik, "rombik.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MenuG, "menu_g.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.ScrollBar, "scrollbar.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single3SC, "single3_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single4SC, "single4_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single5TX, "single5_tx.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.DialSC, "dial_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.JMulti2SC, "jmulti2_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single0SC, "single0_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Single0Dif, "single0_dif.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.RatingSC, "rating_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi2SC, "multi2_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi3SC, "multi3_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi3OP, "multi3_op.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4ParmBK, "multi4parm_bk.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4ParmNM, "multi4parm_nm.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4ParmOP, "multi4parm_op.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MsgBox, "msgbox.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Credits, "credits.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Zvezdy, "zvezdy.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.ZVM, "zv_m.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.ZVGold, "zv_gold.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4CO, "multi4_co.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4LOP, "multi4_lop.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4NOP, "multi4_nop.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4ROP, "multi4_rop.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4TOP, "multi4_top.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4SC, "multi4_sc.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.Multi4SP, "multi4_sp.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MainRES, "main_res.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.MainOP, "main_op.pck", FALSE);
    InitializeBinAsset(&AssetsState.Assets.KartaBK, "karta_bk.pck", FALSE);

    InitializeFontAsset(&AssetsState.Fonts.Comic, "font_comic30", FONTTYPE_BASIC);
    InitializeFontAsset(&AssetsState.Fonts.Main, "font_phermes", FONTTYPE_COMPLEX);

    // NOTE.
    // The hash matches for the original greetings text asset.
    // In case the greetings asset is modified, the assignment of a value triggers
    // the display of the broken roken message every few seconds on every screen of the menu.
    {
        TEXTASSET greetings;
        ActivateTextAsset(&greetings);

        InitializeTextAsset(&greetings, "greetings");

        WindowState.Ticks = 0;
        if (AcquireTextAssetHash(&greetings) != 0x91BBE2BE) { WindowState.Ticks = GetTickCount(); }

        InitializeTextAsset(&greetings, NULL);
        DisposeTextAsset(&greetings);
    }

    LogMessage("load \"mn2_texts\"\n");

    InitializeBinAsset(&AssetsState.Assets.MN2Texts, "mn2_texts.dat", TRUE);
    AsStringBinAsset(&AssetsState.Assets.MN2Texts);

    SelectWindowInputState((WINDOWINPUTSTATE)(WINDOWINPUTSTATE_KEYBOARD | WINDOWINPUTSTATE_MOUSE));

    WindowState.IsActive = TRUE;

    LogMessage("CADraw_Reset()\n");

    State.Renderer->Actions.Initialize();

    LogMessage("CADraw_Set640x480()\n");

    if (State.Renderer->Actions.InitializeWindow(GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480))
    {
        State.Renderer->Actions.DrawMainSurfaceColorRectangle(0, 0,
            GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480, BLACK_PIXEL);
        State.Renderer->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0,
            GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480);
        State.Renderer->Actions.DrawStencilSurfaceWindowRectangle();

        AdjustBinAssetImage(&AssetsState.Assets.MainBK);
        AdjustBinAssetImage(&AssetsState.Assets.MainBT);
        AdjustBinAssetImage(&AssetsState.Assets.Single0BK);
        AdjustBinAssetImage(&AssetsState.Assets.Single0BT);
        AdjustBinAssetImage(&AssetsState.Assets.Single2BK);
        AdjustBinAssetImage(&AssetsState.Assets.Single2BT);
        AdjustBinAssetImage(&AssetsState.Assets.SinOld2BK);
        AdjustBinAssetImage(&AssetsState.Assets.SinOld2BT);
        AdjustBinAssetImage(&AssetsState.Assets.Single3BK);
        AdjustBinAssetImage(&AssetsState.Assets.Single3BT);
        AdjustBinAssetImage(&AssetsState.Assets.Single4BK);
        AdjustBinAssetImage(&AssetsState.Assets.Single4BT);
        AdjustBinAssetImage(&AssetsState.Assets.Single5BK);
        AdjustBinAssetImage(&AssetsState.Assets.Single5BT);
        AdjustBinAssetImage(&AssetsState.Assets.DialBK);
        AdjustBinAssetImage(&AssetsState.Assets.DialBT);
        AdjustBinAssetImage(&AssetsState.Assets.Multi1BK);
        AdjustBinAssetImage(&AssetsState.Assets.Multi1BT);
        AdjustBinAssetImage(&AssetsState.Assets.Multi2BK);
        AdjustBinAssetImage(&AssetsState.Assets.Multi2BT);
        AdjustBinAssetImage(&AssetsState.Assets.Multi3BK);
        AdjustBinAssetImage(&AssetsState.Assets.Multi3BT);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4BK);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4BT);
        AdjustBinAssetImage(&AssetsState.Assets.JMulti1BK);
        AdjustBinAssetImage(&AssetsState.Assets.JMulti1BT);
        AdjustBinAssetImage(&AssetsState.Assets.JMulti2BK);
        AdjustBinAssetImage(&AssetsState.Assets.JMulti2BT);
        AdjustBinAssetImage(&AssetsState.Assets.BriefBK);
        AdjustBinAssetImage(&AssetsState.Assets.BriefBT);
        AdjustBinAssetImage(&AssetsState.Assets.MStatBK);
        AdjustBinAssetImage(&AssetsState.Assets.MStatBT);
        AdjustBinAssetImage(&AssetsState.Assets.RatingBK);
        AdjustBinAssetImage(&AssetsState.Assets.RatingBT);
        AdjustBinAssetImage(&AssetsState.Assets.Msg0);
        AdjustBinAssetImage(&AssetsState.Assets.Msg1BK);
        AdjustBinAssetImage(&AssetsState.Assets.Msg1BT0);
        AdjustBinAssetImage(&AssetsState.Assets.Msg1BT1);
        AdjustBinAssetImage(&AssetsState.Assets.Msg1CBT0);
        AdjustBinAssetImage(&AssetsState.Assets.Msg1CBT1);
        AdjustBinAssetImage(&AssetsState.Assets.Msg2BK);
        AdjustBinAssetImage(&AssetsState.Assets.Msg2BT10);
        AdjustBinAssetImage(&AssetsState.Assets.Msg2BT11);
        AdjustBinAssetImage(&AssetsState.Assets.Msg2BT20);
        AdjustBinAssetImage(&AssetsState.Assets.Msg2BT21);
        AdjustBinAssetImage(&AssetsState.Assets.GreetingsBK);
        AdjustBinAssetImage(&AssetsState.Assets.Rombik);
        AdjustBinAssetImage(&AssetsState.Assets.MenuG);
        AdjustBinAssetImage(&AssetsState.Assets.ScrollBar);
        AdjustBinAssetImage(&AssetsState.Assets.Single3SC);
        AdjustBinAssetImage(&AssetsState.Assets.Single4SC);
        AdjustBinAssetImage(&AssetsState.Assets.Single5TX);
        AdjustBinAssetImage(&AssetsState.Assets.DialSC);
        AdjustBinAssetImage(&AssetsState.Assets.JMulti2SC);
        AdjustBinAssetImage(&AssetsState.Assets.Single0SC);
        AdjustBinAssetImage(&AssetsState.Assets.Single0Dif);
        AdjustBinAssetImage(&AssetsState.Assets.RatingSC);
        AdjustBinAssetImage(&AssetsState.Assets.Multi2SC);
        AdjustBinAssetImage(&AssetsState.Assets.Multi3SC);
        AdjustBinAssetImage(&AssetsState.Assets.Multi3OP);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4ParmBK);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4ParmNM);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4ParmOP);
        AdjustBinAssetImage(&AssetsState.Assets.MsgBox);
        AdjustBinAssetImage(&AssetsState.Assets.Credits);
        AdjustBinAssetImage(&AssetsState.Assets.Zvezdy);
        AdjustBinAssetImage(&AssetsState.Assets.ZVM);
        AdjustBinAssetImage(&AssetsState.Assets.ZVGold);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4CO);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4LOP);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4NOP);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4ROP);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4TOP);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4SC);
        AdjustBinAssetImage(&AssetsState.Assets.Multi4SP);
        AdjustBinAssetImage(&AssetsState.Assets.MainRES);
        AdjustBinAssetImage(&AssetsState.Assets.MainOP);
        AdjustBinAssetImage(&AssetsState.Assets.KartaBK);

        InitializeFontAsset(&AssetsState.Fonts.Comic);
        InitializeFontAsset(&AssetsState.Fonts.Main);

        AssetsState.Fonts.Main.Height = AssetsState.Fonts.Main.Height + 2;
        AssetsState.Fonts.Main.Offset = 3;

        InitializeShortcuts(&Shortcuts);

        LogMessage("SoundInit()\n");

        InitializeSoundState(&SoundState.State, State.Module->Sound);
        InitializeCursorState(&CursorState.Cursor, "menu_cur");

        State.Main = ActivateMainControl(ALLOCATE(MAINCONTROL));

        {
            STRINGVALUE name, value;
            AcquireSettingsValue(&name, IDS_BACK_WAVE_PATH);
            AcquireStringValue(&value, StringsState.Scratch);

            STRINGVALUE setting;
            STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

            InitializeSoundStateBackBuffer(&SoundState.State, actual->Value);

            ReleaseStringValue(actual);
        }

        InitializeSoundStateBackBuffer(&SoundState.State, TRUE);

        LogMessage("skeleton->init()\n");

        State.Main->Self->Initialize(State.Main);

        return TRUE;
    }

    MessageBoxA(State.Window->HWND,
        AcquireAssetMessage(ASSET_MESSAGE_UNABLE_TO_SET_VIDEO_MODE), "DDraw Error", MB_ICONHAND | MB_OK);

    return FALSE;
}

// 0x10021d70
BOOL ExecuteModuleAction(VOID)
{
    if (State.App->InitModule == LAUNCHER_MODULE_STATE_INDEX)
    {
        U32 action = CONTROLACTION_NONE;

        ReleaseSoundStateInactiveSoundTracks(&SoundState.State);

        ExecuteNetwork();

        if (State.Window->IsActive
            && (!MessageControlState.IsVisible || (MessageControlState.Mode & MESSAGE_MODE_WAIT)))
        {
            State.Main->Self->Tick(State.Main);

            action = CONTROLACTION_NONE;
        }

        do
        {
            if (!MessageControlState.IsVisible || (MessageControlState.Mode & MESSAGE_MODE_WAIT))
            {
                action = action | State.Main->Self->Action(State.Main);
            }

            ActionMessageControl(&MessageControlState);
            ConsumeQueuedCommands();
        } while (DequeueControlCommand(FALSE) != NULL);

        if (State.Window->IsActive && !VideoState.IsActive)
        {
            if (MessageControlState.IsVisible)
            {
                WriteMainSurfaceMainSurfaceWindowRectangle();
                TickMessageControl(&MessageControlState);
            }

            SaveCursorPixels(&CursorState.Cursor);
            DrawCursor(&CursorState.Cursor);

            State.Renderer->Actions.WriteMainSurfaceRendererSurfaceRectangle(0, 0,
                GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480);

            RestoreCursorPixels(&CursorState.Cursor);

            if (MessageControlState.IsVisible) { WriteBackSurfaceMainSurfaceWindowRectangle(); }
        }

        if (WindowState.Ticks != 0)
        {
            CONST U32 ticks = GetTickCount();
            if (10000 < ticks - WindowState.Ticks)
            {
                WindowState.Ticks = ticks - 6000;

                ShowMessageControl(&MessageControlState, "Broken roken?", MESSAGE_BUTTON_OKCANCEL);
            }
        }

        if (action != CONTROLACTION_NONE) { State.App->InitModule = INVALID_MODULE_STATE_INDEX; }
    }

    return TRUE;
}

// 0x10021bf0
BOOL ReleaseModuleAction(VOID)
{
    LogMessage("XCHG->ToGame.difficulty=%d\n", State.Module->Game.Difficulty);

    ReleaseNetworkStateInstance();

    if (State.App->InitModule == GAME_MODULE_STATE_INDEX)
    {
        if (State.Module->Game.IsNetwork) { ReleaseNetworkStateContainer(); }
    }
    else
    {
        DeleteGameState();

        if (State.Module->Game.IsNetwork) { ReleaseNetworkStateContainer(); }
    }

    ReleaseNetworkState(&NetworkState.State);
    DisposeNetworkState(&NetworkState.State);

    if (NetworkState.State.Descriptor != NULL) { free(NetworkState.State.Descriptor); }
    if (NetworkState.State.Allocatedx20 != NULL) { free(NetworkState.State.Allocatedx20); }

    State.Renderer->Actions.ReleaseRendererSurface();

    SelectWindowInputState(WINDOWINPUTSTATE_NONE);

    ReleaseSoundState(&SoundState.State);

    ReleaseAssets();
    ReleaseStartArguments();

    if (State.Main != NULL) { State.Main->Self->Release(State.Main, OBJECTRELEASETYPE_ALLOCATED); }

    return TRUE;
}

// 0x100243c0
VOID AcquireStartArguments(VOID)
{
    U32 count = 0;
    U32 length = 0;

    AcquireStartArguments(State.Window->Args, NULL, NULL, &count, &length);

    State.Arguments.All = (LPSTR)malloc(length);
    State.Arguments.Args = (LPSTR*)malloc(count * sizeof(LPSTR));

    AcquireStartArguments(State.Window->Args, State.Arguments.Args, State.Arguments.All, &count, &length);

    State.Arguments.Count = count;
}

// 0x10024440
VOID AcquireStartArguments(LPCSTR value, LPSTR* args, LPSTR values, U32* count, U32* length)
{
    BOOL start = FALSE;
    BOOL end = FALSE;

    *length = 0;
    *count = 0;

    if (value == NULL) { return; }

    while (*value != NULL)
    {
        // Skip spaces and tabs.
        while (*value == ' ' || *value == '\t') { value = value + 1; }

        if (*value == NULL) { break; }

        if (args != NULL)
        {
            *args = values;
            args = args + 1;
        }

        *count = *count + 1;

        while (TRUE)
        {
            U32 slashes = 0;
            BOOL append = TRUE;
            CHAR current = *value;

            // Count contiguous slashes.
            while (current == '\\')
            {
                value = value + 1;
                slashes = slashes + 1;
                current = *value;
            }

            if (*value == '\"')
            {
                if ((slashes & 1) == 0)
                {
                    if (start && value[1] == '\"')
                    {
                        value = value + 1;
                    }
                    else { append = FALSE; }

                    start = !end;
                    end = start;
                }

                slashes = slashes >> 1;
            }

            for (; slashes != 0; slashes = slashes - 1)
            {
                if (values != NULL)
                {
                    *values = '\\';
                    values = values + 1;
                }

                *length = *length + 1;
            }

            current = *value;

            if (current == NULL || (!start && (current == ' ' || current == '\t'))) { break; }

            if (append)
            {
                if (values != NULL)
                {
                    *values = current;
                    values = values + 1;
                }

                *length = *length + 1;
            }

            value = value + 1;
        }

        if (values != NULL)
        {
            *values = NULL;
            values = values + 1;
        }

        *length = *length + 1;
    }
}

// 0x10024530
VOID ReleaseStartArguments(VOID)
{
    if (State.Arguments.Args != NULL) { free(State.Arguments.Args); }
    if (State.Arguments.All != NULL) { free(State.Arguments.All); }
}

// 0x10017490
// a.k.a. CA_ETEXT_Init
VOID AcquireIniFileState(VOID)
{
    State.IsIniActive = GetPrivateProfileIntA("debug", "useinifile", FALSE, State.App->Ini);
}

// 0x10021cc0
BOOL ConsumeQueuedCommands(VOID)
{
    BOOL result = FALSE;

    {
        CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

        if (command != NULL)
        {
            if (command->Command == ModuleCommand.Command && command->Action == ModuleCommand.Action
                && command->Parameter1 == ModuleCommand.Parameter1 && command->Parameter2 == ModuleCommand.Parameter2)
            {
                DequeueControlCommand(TRUE);

                result = TRUE;
            }
        }
    }

    {
        CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

        if (command != NULL)
        {
            ModuleCommand.Command = command->Command;
            ModuleCommand.Action = command->Action;
            ModuleCommand.Parameter1 = command->Parameter1;
            ModuleCommand.Parameter2 = command->Parameter2;

            return result;
        }
    }

    ModuleCommand.Action = CONTROLACTION_UNKNOWN;

    return result;
}

// 0x10024560
// NOTE. Contains additional checks for better startup arguments parsing.
BOOL AcquireStartArguments(LPCSTR name, LPSTR value, CONST U32 length)
{
    for (U32 x = 0; x < State.Arguments.Count; x++)
    {
        LPSTR current = strrchr(State.Arguments.Args[x], '=');

        if (current != NULL)
        {
            CONST ADDR size = (ADDR)current - (ADDR)State.Arguments.Args[x];

            if (size != 0 && strnicmp(State.Arguments.Args[x], name, size) == 0)
            {
                current++;

                strncpy(value, current, Min<size_t>(length, strlen(current) + 1));

                return TRUE;
            }
        }
    }

    return FALSE;
}