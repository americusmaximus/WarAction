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
#include "Game.hxx"
#include "Logger.hxx"
#include "Saves.hxx"
#include "Settings.hxx"
#include "State.hxx"
#include "Strings.hxx"
#include "Version.hxx"

#include <..\Text\Resources.hxx>

// 0x10013090
BOOL InitializeGameState(VOID)
{
    State.Module->Game.IsNetwork = FALSE;
    State.Module->Game.Ticks = GetTickCount();
    State.Module->Game.Unk07 = 0; // TODO
    State.Module->Game.Difficulty = SaveState.Difficulty;

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, IDS_TURN_DELAY);

        State.Module->Game.TurnDelay = AcquireSettingsValue(value, DEFAULT_SAVE_STATE_TURN_DELAY);
    }

    State.Module->Game.Unk09 = 4; // TODO

    SaveState.Version = MODULE_VERSION_VALUE;

    strcpy(SaveState.Path, State.Name);

    SaveState.Map = AcquireCurrentGameMap();
    SaveState.Mission = AcquireCurrentGameMission();

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };
    OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

    U32 magic = SAVE_STATE_SAVE_FILE_MAGIC;
    WriteBinFile(&file, &magic, sizeof(U32));
    WriteBinFile(&file, &SaveState, sizeof(SAVEMODULESTATECONTAINER));
    LoadGameState(&file);
    CloseBinFile(&file);

    if (FUN_10018c00(State.Name))
    {
        State.App->InitModule = GAME_MODULE_STATE_INDEX;

        return TRUE;
    }

    return FALSE;
}

// 0x10015bf0
BOOL InitializeSingleGame(VOID)
{
    CHAR path[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SINGLE_MAP_DIR);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        strcpy(path, actual->Value);

        ReleaseStringValue(actual);
    }

    LPSTR divider = strchr(State.Map.Name, ':');
    if (divider != NULL) { divider[0] = NULL; }

    strcpy(SaveState.Path, State.Map.Name);

    strcat(path, State.Map.Name);
    strcat(path, ".ssm");

    return WriteSaveState(path);
}

// 0x100188e0
STATUS AcquireGameStatus(VOID)
{
    return State.Module->Game.Status;
}

// 0x100188f0
VOID DeleteGameState(VOID)
{
    DeleteFileA("XCHNG\\STATE\\mapmis");
    DeleteFileA("XCHNG\\STATE\\map_info");
    DeleteFileA("XCHNG\\STATE\\map_rhombs");
    DeleteFileA("XCHNG\\STATE\\map_flags");
    DeleteFileA("XCHNG\\STATE\\map_mini1");
    DeleteFileA("XCHNG\\STATE\\map_mini2");
    DeleteFileA("XCHNG\\STATE\\map_landnames");
    DeleteFileA("XCHNG\\STATE\\map_objects");
    DeleteFileA("XCHNG\\STATE\\map_mines");
    DeleteFileA("XCHNG\\TOGAME\\map_info");
    DeleteFileA("XCHNG\\TOGAME\\map_rhombs");
    DeleteFileA("XCHNG\\TOGAME\\map_flags");
    DeleteFileA("XCHNG\\TOGAME\\map_mini");
    DeleteFileA("XCHNG\\TOGAME\\map_landnames");
    DeleteFileA("XCHNG\\TOGAME\\map_objects");
    DeleteFileA("XCHNG\\TOGAME\\map_mines");
    DeleteFileA("XCHNG\\TOGAME\\mis_unitnames");
    DeleteFileA("XCHNG\\TOGAME\\mis_groups");
    DeleteFileA("XCHNG\\TOGAME\\mis_players");
    DeleteFileA("XCHNG\\TOGAME\\mis_mapunits");
    DeleteFileA("XCHNG\\TOGAME\\mis_woofers");
    DeleteFileA("XCHNG\\TOGAME\\mis_zones");
    DeleteFileA("XCHNG\\TOGAME\\mis_scripts");
    DeleteFileA("XCHNG\\TOGAME\\mis_support");
    DeleteFileA("XCHNG\\TOGAME\\mis_desc");
    DeleteFileA("XCHNG\\TOGAME\\mis_phrases");
    DeleteFileA("XCHNG\\TOGAME\\mis_objects");
    DeleteFileA("XCHNG\\TOGAME\\mplay_info");
    DeleteFileA("XCHNG\\FROMGAME\\mis_finishinfo");
    DeleteFileA("XCHNG\\FROMGAME\\map_info");
    DeleteFileA("XCHNG\\FROMGAME\\map_flags");
    DeleteFileA("XCHNG\\FROMGAME\\map_landnames");
    DeleteFileA("XCHNG\\FROMGAME\\map_objects");
    DeleteFileA("XCHNG\\FROMGAME\\map_mines");
    DeleteFileA("XCHNG\\menu.sav");
    DeleteFileA("XCHNG\\game.sav");
}

// 0x10020c40
BOOL RestartGame(VOID)
{
    LogMessage("RK_RESTART\n");

    BINFILE save = { (BFH)INVALID_BINFILE_VALUE };
    if (!OpenBinFile(&save, "XCHNG\\menu.sav", BINFILEOPENTYPE_READ))
    {
        LogMessage("cannot open '%s'\n", "XCHNG\\menu.sav");

        return FALSE;
    }

    BINARCHIVEHEADER header;
    ZeroMemory(&header, sizeof(BINARCHIVEHEADER));

    ReadBinFile(&save, &header, sizeof(BINARCHIVEHEADER));
    ReadBinFile(&save, &SaveState, sizeof(SAVEMODULESTATECONTAINER));

    LogMessage("Geck.ssaveLoadState()\n");

    SaveGameState(&save);

    CloseBinFile(&save);

    if (SaveState.Path[0] != NULL)
    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SINGLE_MAP_DIR);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        CHAR path[MAX_FILE_NAME_LENGTH];
        wsprintfA(path, "%s%s", actual->Value, SaveState.Path);

        ReleaseStringValue(actual);

        LogMessage("Geck.singleBuildNewSingle(%s)\n", path);

        WriteSaveState(path);
    }

    strcpy(State.Name, State.Module->Game.Players[0].Name);

    LogMessage("return TRUE\n");

    return TRUE;
}
