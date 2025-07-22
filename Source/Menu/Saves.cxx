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

#include "Logger.hxx"
#include "Saves.hxx"
#include "Settings.hxx"
#include "State.hxx"
#include "Version.hxx"
#include "ZipFile.hxx"

#include <..\Text\Resources.hxx>

#define MAX_BINSAVEFILE_LENGTH          10000000
#define DEFAULT_SAVE_STATE_GAME_VALUE   (-1)

// 0x10046e48
SAVEMODULESTATECONTAINER SaveState;

// TODO move sring values to defines

// 0x10022140
U32 AcquireSaveStateValue(LPCSTR name, CONST U32 offset, LPVOID value, U32 size)
{
    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenBinFile(&file, name, BINFILEOPENTYPE_READ)) { return DEFAULT_SAVE_STATE_GAME_VALUE; }

    if (offset != 0) { PointBinFile(&file, offset, FILE_BEGIN); }

    CONST U32 result = ReadBinFile(&file, value, size);

    CloseBinFile(&file);

    return result;
}

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

// 0x10015010
// a.k.a. ssaveLoadState
BOOL LoadSaveState(LPCSTR save)
{
    CHAR path[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SAVES);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        strcpy(path, actual->Value);

        ReleaseStringValue(actual);
    }

    strcat(path, save);
    strcat(path, ".sss");

    State.Module->Game.IsNetwork = FALSE;
    State.Module->Game.Ticks = GetTickCount();
    State.Module->Game.Unk07 = 1; // TODO
    State.Module->Game.TurnDelay = DEFAULT_SAVE_STATE_TURN_DELAY;
    State.Module->Game.Unk09 = 5; // TODO
    State.Module->Game.Difficulty = GAMEDIFFICULTY_EASY;

    LogMessage("LOAD: Loading '%s' - ", save);

    if (!UnpackSaveFile(path))
    {
        LogMessage("ssaveUnpack() failed\n");

        return FALSE;
    }

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_READ))
    {
        LogMessage("cannot open '%s'\n", "XCHNG\\menu.sav");

        return FALSE;
    }

    U32 magic = 0;
    ReadBinFile(&file, &magic, sizeof(U32));

    BOOL result = FALSE;

    if (magic == SAVE_STATE_SAVE_FILE_MAGIC)
    {
        U32 version = 0;
        ReadBinFile(&file, &version, sizeof(U32));

        SaveState.Version = version;

        if (version >= MODULE_VERSION_VALUE) { ReadBinFile(&file, &SaveState.Content, sizeof(SAVEFILECONTENT)); }

        result = SaveGameState(&file);
    }
    else
    {
        PointBinFile(&file, 0, FILE_BEGIN);

        SAVEFILEDEFINITION definition;
        ReadBinFile(&file, &definition, sizeof(SAVEFILEDEFINITION));

        strcpy(SaveState.Path, definition.Name);

        SaveState.Mission = definition.Mission;
        SaveState.Map = definition.Map;
        SaveState.Version = MODULE_VERSION_VALUE;
        SaveState.Difficulty = GAMEDIFFICULTY_EASY;

        result = SaveGameState(&file);

        CloseBinFile(&file);

        if (!OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE))
        {
            LogMessage("cannot open '%s' for writing\n", "XCHNG\\menu.sav");

            return FALSE;
        }

        magic = SAVE_STATE_SAVE_FILE_MAGIC;

        WriteBinFile(&file, &magic, sizeof(U32));
        WriteBinFile(&file, &SaveState, sizeof(SAVEMODULESTATECONTAINER));
        LoadGameState(&file);
    }

    CloseBinFile(&file);

    State.Module->Game.Difficulty = SaveState.Difficulty;

    LogMessage(result ? "ok\n" : "ssaveLoadState() failed\n");

    return result;
}

// 0x1001a340
// a.k.a. ssaveLoadState
BOOL SaveGameState(BINFILEPTR save)
{
    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    U32 magic = 0;
    ReadBinFile(save, &magic, sizeof(U32));

    if (magic != SAVE_STATE_FILE_START_MAGIC) { return FALSE; }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\mapmis", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_info", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
        
        ReadBinFile(save, &length, sizeof(U32));
        
        CopyBinFile(save, &file, length);
        
        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_rhombs", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_flags", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_mini1", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_mini2", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_landnames", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_objects", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    {
        U32 length = 0;
        OpenBinFile(&file, "XCHNG\\STATE\\map_mines", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        ReadBinFile(save, &length, sizeof(U32));

        CopyBinFile(save, &file, length);

        CloseBinFile(&file);
    }

    ReadBinFile(save, &magic, sizeof(U32));
    
    return magic == SAVE_STATE_FILE_END_MAGIC;
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

// 0x1001a070
BOOL LoadGameState(BINFILEPTR save)
{
    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    CONST U32 start = SAVE_STATE_FILE_START_MAGIC;
    WriteBinFile(save, &start, sizeof(U32));

    {
        OpenBinFile(&file, "XCHNG\\STATE\\mapmis", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_info", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_rhombs", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_flags", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_mini1", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_mini2", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_landnames", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_objects", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\STATE\\map_mines", BINFILEOPENTYPE_READ);

        U32 length = AcquireBinFileSize(&file);

        WriteBinFile(save, &length, sizeof(U32));

        CopyBinFile(&file, save, length);

        CloseBinFile(&file);
    }

    CONST U32 end = SAVE_STATE_FILE_END_MAGIC;
    WriteBinFile(save, &end, sizeof(U32));

    return TRUE;
}

// 0x1001a5a0
BOOL UnpackSaveFile(LPCSTR save)
{
    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, save, ZIPFILE_OPEN_READ)) { return FALSE; }

    U32 length = 0;
    ReadZipFile(&zip, &length, sizeof(U32));
    OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
    WriteZipFile(&zip, &file, length);
    CloseBinFile(&file);

    OpenBinFile(&file, "XCHNG\\game.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
    WriteZipFile(&zip, &file, MAX_BINSAVEFILE_LENGTH);
    CloseBinFile(&file);

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10018b40
S32 AcquireCurrentGameMap(VOID) // TODO name
{
    S32 value = DEFAULT_GAME_MAP_INDEX;

    // Read value at 0 offset.
    AcquireSaveStateValue("XCHNG\\STATE\\mapmis", 0, &value, sizeof(S32));

    return value;
}

// 0x10018b70
S32 AcquireCurrentGameMission(VOID) // TODO name
{
    S32 value = DEFAULT_GAME_MISSION_INDEX;

    // Read value at 4 offset.
    AcquireSaveStateValue("XCHNG\\STATE\\mapmis", sizeof(S32), &value, sizeof(S32));

    return value;
}

// 0x10019680
BOOL WriteSaveState(LPCSTR save)
{
    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, save, ZIPFILE_OPEN_READ)) { return FALSE; }

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    {
        if (!OpenBinFile(&file, "XCHNG\\STATE\\mapmis",
            BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE))
        {
            CloseZipFile(&zip);

            return FALSE;
        }

        CONST S32 map = DEFAULT_GAME_MAP_INDEX;
        WriteBinFile(&file, &map, sizeof(S32));

        CONST S32 mission = DEFAULT_GAME_MISSION_INDEX;
        WriteBinFile(&file, &mission, sizeof(S32));

        CloseBinFile(&file);
    }

    {
        U32 magic = 0;
        ReadZipFile(&zip, &magic, sizeof(U32));

        if (magic != MAP_FILE_SINGLE_MAGIC) { CloseZipFile(&zip); return FALSE; }

        U32 version = 0;
        ReadZipFile(&zip, &version, sizeof(U32));

        if (version != MODULE_VERSION_VALUE) { CloseZipFile(&zip); return FALSE; }
    }

    U32 mapWidth = 0, mapHeight = 0; // TODO name

    {
        CHAR name[32]; // TODO
        U32 local_2cc; // TODO

        ReadZipFile(&zip, name, 0x20); // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        ReadZipFile(&zip, &mapWidth, 4); // TODO proper order
        ReadZipFile(&zip, &mapHeight, 4); // TODO proper order

        OpenBinFile(&file, "XCHNG\\TOGAME\\map_info", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteBinFile(&file, &local_2cc, 4);

        U32 local_2c8 = 0; // TODO
        WriteBinFile(&file, &local_2c8, 4); // TODO
        WriteBinFile(&file, &local_2c8, 4); // TODO
        WriteBinFile(&file, &mapWidth, 4); // TODO
        WriteBinFile(&file, &mapHeight, 4); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_desc", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        WriteZipFile(&zip, &file, local_2cc); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\map_mini", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        CONST U32 length = mapWidth <= MAX_MAP_SIZE && mapHeight <= MAX_MAP_SIZE // TODO name
            ? mapWidth * mapHeight * 2 : mapHeight * mapWidth / 2; // TODO

        WriteZipFile(&zip, &file, length);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\map_rhombs", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteZipFile(&zip, &file, mapWidth * mapHeight * 2); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\map_flags", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteZipFile(&zip, &file, mapWidth * mapHeight * 4); // TODO

        CloseBinFile(&file);
    }

    {
        CHAR message[0x200]; // TODO
        ZeroMemory(message, 0x200); // TODO

        U32 local_2b8;
        ReadZipFile(&zip, &local_2b8, 4); // TODO
        ReadZipFile(&zip, message, local_2b8 << 4); // TODO

        OpenBinFile(&file, "XCHNG\\TOGAME\\map_landnames", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteBinFile(&file, message, 0x200); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\map_objects", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        WriteZipFile(&zip, &file, local_2cc * 8); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_unitnames", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        WriteZipFile(&zip, &file, local_2cc << 4); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_groups", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteZipFile(&zip, &file, 0xa8c); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_mapunits", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        WriteZipFile(&zip, &file, local_2cc); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_players", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        WriteBinFile(&file, &local_2cc, 4); // TODO
        WriteZipFile(&zip, &file, local_2cc * 0x161); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_woofers", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, 4); // TODO
        WriteBinFile(&file, &local_2cc, 4); // TODO
        WriteZipFile(&zip, &file, local_2cc * 0x4e); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_zones", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteZipFile(&zip, &file, mapWidth * mapHeight);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_scripts", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 count; // TODO
        ReadZipFile(&zip, &count, 4); // TODO
        WriteBinFile(&file, &count, 4); // TODO

        for (U32 x = 0; x < count; x++)
        {
            U32 local_2c8; // TODO
            ReadZipFile(&zip, &local_2c8, 4); // TODO
            local_2c8 = local_2c8 - 4; // TODO

            WriteBinFile(&file, &local_2c8, 4); // TODO

            U32 local_2b8; // TODO
            ReadZipFile(&zip, &local_2b8, 4); // TODO
            WriteZipFile(&zip, &file, local_2c8); // TODO
        }

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\map_mines", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteZipFile(&zip, &file, mapWidth * mapHeight);

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_support", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, sizeof(U32)); // TODO
        WriteZipFile(&zip, &file, local_2cc); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_phrases", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        U32 local_2cc; // TODO
        ReadZipFile(&zip, &local_2cc, sizeof(U32));// TODO
        WriteZipFile(&zip, &file, local_2cc); // TODO

        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\TOGAME\\mis_objects", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

        WriteZipFile(&zip, &file, 400); // TODO

        CloseBinFile(&file);
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10018a00
BOOL SaveMapMission(CONST S32 map, CONST S32 mission)
{
    if (!SaveMapState(map)) { return FALSE; }

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    if (!OpenBinFile(&file, "XCHNG\\STATE\\mapmis", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE)) { return FALSE; }

    WriteBinFile(&file, &map, sizeof(S32));
    WriteBinFile(&file, &mission, sizeof(S32));

    CloseBinFile(&file);

    return TRUE;
}

// 0x1001a670
BOOL SaveMapState(CONST S32 map)
{
    CHAR path[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_CAMPAIGN_DIR);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        wsprintfA(path, "%s%03i.ssc", actual->Value, map);

        ReleaseStringValue(actual);
    }

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (OpenZipFile(&zip, path, ZIPFILE_OPEN_READ))
    {
        {
            U32 magic = 0;
            ReadZipFile(&zip, &magic, sizeof(U32));

            U32 version = 0;
            ReadZipFile(&zip, &version, sizeof(U32));

            if (version != MODULE_VERSION_VALUE)
            {
                CloseZipFile(&zip);

                return FALSE;
            }
        }

        U32 width = 0, height = 0;

        BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

        {
            U32 iStack_3b4; // TODO
            ReadZipFile(&zip, &iStack_3b4, 4);// TODO
            ReadZipFile(&zip, &width, sizeof(U32));
            ReadZipFile(&zip, &height, sizeof(U32));

            OpenBinFile(&file, "XCHNG\\STATE\\map_info", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteBinFile(&file, &iStack_3b4, 4);// TODO
            WriteBinFile(&file, &width, sizeof(U32));
            WriteBinFile(&file, &height, sizeof(U32));

            CloseBinFile(&file);
        }

        {
            CHAR names[512]; // TODO
            ZeroMemory(names, 512); // TODO

            U32 count = 0;
            ReadZipFile(&zip, &count, sizeof(U32));
            ReadZipFile(&zip, names, count * 16); // TODO

            OpenBinFile(&file, "XCHNG\\STATE\\map_landnames", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteBinFile(&file, names, 512); // TODO

            CloseBinFile(&file);
        }

        {
            OpenBinFile(&file, "XCHNG\\STATE\\map_rhombs", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &file, width * height * 2); // TODO

            CloseBinFile(&file);
        }

        {
            OpenBinFile(&file, "XCHNG\\STATE\\map_flags", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &file, width * height * 4); // TODO

            CloseBinFile(&file);
        }

        {
            OpenBinFile(&file, "XCHNG\\STATE\\map_mini1", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &file, width * height * 2); // TODO

            CloseBinFile(&file);
        }

        {
            OpenBinFile(&file, "XCHNG\\STATE\\map_mini2", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &file, ((width / 2) * height) / 2 << 1); // TODO

            CloseBinFile(&file);
        }

        {
            OpenBinFile(&file, "XCHNG\\STATE\\map_mines", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            ZeroMemory(State.Renderer->Surface.Back, width * height >> 2); // TODO

            WriteBinFile(&file, State.Renderer->Surface.Back, width * height); // TODO

            CloseBinFile(&file);
        }

        {
            OpenBinFile(&file, "XCHNG\\STATE\\map_objects", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            ReadZipFile(&zip, &width, 4); // TODO variable name
            WriteZipFile(&zip, &file, width * 8); // TODO

            CloseBinFile(&file);
        }

        CloseZipFile(&zip);

        return TRUE;
    }

    return FALSE;
}

// 0x10018ae0
BOOL SaveMission(CONST S32 mission)
{
    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    OpenBinFile(&file, "XCHNG\\STATE\\mapmis", BINFILEOPENTYPE_WRITE);

    PointBinFile(&file, sizeof(U32), FILE_BEGIN);
    WriteBinFile(&file, &mission, sizeof(S32));
    PointBinFile(&file, 0, FILE_END);
    CloseBinFile(&file);
    
    return TRUE;
}

// 0x10018a70
BOOL SaveMap(CONST S32 map)
{
    if (!SaveMapState(map)) { return FALSE; }

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    OpenBinFile(&file, "XCHNG\\STATE\\mapmis", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

    WriteBinFile(&file, &map, sizeof(S32));

    S32 mission = DEFAULT_GAME_MISSION_INDEX;
    WriteBinFile(&file, &mission, sizeof(S32));

    CloseBinFile(&file);

    return TRUE;
}

// 0x10018c00
BOOL FUN_10018c00(LPCSTR name) // TODO name. Move to Game.cxx ?
{
    if (AcquireCurrentGameMap() == DEFAULT_GAME_MAP_INDEX)
    {
        BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

        if (!OpenBinFile(&file, "XCHNG\\TOGAME\\mis_players", BINFILEOPENTYPE_WRITE)) { return FALSE; }

        PointBinFile(&file, sizeof(S32), FILE_BEGIN);

        CHAR value[37]; // TODO
        ReadBinFile(&file, value, 37); // TODO

        strcpy(value, name); // TODO

        PointBinFile(&file, sizeof(S32), FILE_BEGIN);
        WriteBinFile(&file, value, 37); // TODO

        PointBinFile(&file, AcquireBinFileSize(&file), FILE_BEGIN);
        CloseBinFile(&file);
    }
    else
    {
        CHAR path[MAX_FILE_NAME_LENGTH];

        {
            STRINGVALUE name, value;
            AcquireSettingsValue(&name, IDS_CAMPAIGN_DIR);
            AcquireStringValue(&value, StringsState.Scratch);

            STRINGVALUE setting;
            STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

            wsprintfA(path, "%s%03i%03i.ssc", actual->Value, AcquireCurrentGameMap(), AcquireCurrentGameMission());
            ReleaseStringValue(actual);
        }

        ZIPFILE zip;
        ZeroMemory(&zip, sizeof(ZIPFILE));

        if (!OpenZipFile(&zip, path, ZIPFILE_OPEN_READ)) { return FALSE; }

        {
            U32 magic = 0;
            ReadZipFile(&zip, &magic, sizeof(U32));

            U32 version = 0;
            ReadZipFile(&zip, &version, sizeof(U32));

            if (version != MODULE_VERSION_VALUE)
            {
                CloseZipFile(&zip);

                return FALSE;
            }
        }

        BINFILE reader = { (BFH)INVALID_BINFILE_VALUE };
        BINFILE writer = { (BFH)INVALID_BINFILE_VALUE };

        U32 iStack_3e8, iStack_3ec, iStack_3f0, uStack_3f4, iStack_3f8; // TODO

        {
            U32 auStack_33c, uStack_400; // TODO

            ReadZipFile(&zip, &iStack_3e8, 4); // TODO
            ReadZipFile(&zip, &iStack_3ec, 4); // TODO
            ReadZipFile(&zip, &uStack_3f4, 4); // TODO
            ReadZipFile(&zip, &iStack_3f8, 4); // TODO

            OpenBinFile(&reader, "XCHNG\\STATE\\map_info", BINFILEOPENTYPE_READ);

            ReadBinFile(&reader, &uStack_400, 4); // TODO
            ReadBinFile(&reader, &iStack_3f0, 4); // TODO
            ReadBinFile(&reader, &auStack_33c, 4); // TODO

            CloseBinFile(&reader);

            OpenBinFile(&writer, "XCHNG\\TOGAME\\map_info", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteBinFile(&writer, &uStack_400, 4); // TODO
            WriteBinFile(&writer, &iStack_3e8, 4); // TODO
            WriteBinFile(&writer, &iStack_3ec, 4); // TODO
            WriteBinFile(&writer, &uStack_3f4, 4); // TODO
            WriteBinFile(&writer, &iStack_3f8, 4); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&reader, "XCHNG\\STATE\\map_rhombs", BINFILEOPENTYPE_READ);
            OpenBinFile(&writer, "XCHNG\\TOGAME\\map_rhombs", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            PointBinFile(&reader, (iStack_3f0 * iStack_3ec + iStack_3e8) * 2, FILE_BEGIN); // TODO

            for (U32 x = 0; x < iStack_3f8; x++)
            {
                ReadBinFile(&reader, State.Renderer->Surface.Back, iStack_3f0 * 2); // TODO
                WriteBinFile(&writer, State.Renderer->Surface.Back, uStack_3f4 * 2); // TODO
            }

            CloseBinFile(&reader);
            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&reader, "XCHNG\\STATE\\map_flags", BINFILEOPENTYPE_READ);
            OpenBinFile(&writer, "XCHNG\\TOGAME\\map_flags", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            PointBinFile(&reader, (iStack_3f0 * iStack_3ec + iStack_3e8) * 4, FILE_BEGIN); // TODO

            for (U32 x = 0; x < iStack_3f8; x++)
            {
                ReadBinFile(&reader, State.Renderer->Surface.Back, iStack_3f0 * 4); // TODO
                WriteBinFile(&writer, State.Renderer->Surface.Back, uStack_3f4 * 4); // TODO
            }

            CloseBinFile(&reader);
            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\map_mini", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            if (uStack_3f4 <= MAX_MAP_SIZE && iStack_3f8 <= MAX_MAP_SIZE)
            {
                OpenBinFile(&reader, "XCHNG\\STATE\\map_mini1", BINFILEOPENTYPE_READ);

                PointBinFile(&reader, (iStack_3f0 * iStack_3ec + iStack_3e8) * 2, FILE_BEGIN); // TODO

                for (U32 x = 0; x < iStack_3f8; x++)
                {
                    ReadBinFile(&reader, State.Renderer->Surface.Back, iStack_3f0 * 2); // TODO
                    WriteBinFile(&writer, State.Renderer->Surface.Back, uStack_3f4 * 2); // TODO
                }
            }
            else
            {
                OpenBinFile(&reader, "XCHNG\\STATE\\map_mini2", BINFILEOPENTYPE_READ);

                PointBinFile(&reader, (((iStack_3ec / 2) * iStack_3f0) / 2 + iStack_3e8 / 2) * 2, FILE_BEGIN); // TODO

                for (U32 x = 0; x < iStack_3f8; x++)
                {
                    ReadBinFile(&reader, State.Renderer->Surface.Back, iStack_3f0 / 2 << 1); // TODO
                    WriteBinFile(&writer, State.Renderer->Surface.Back, uStack_3f4 / 2 << 1); // TODO
                }
            }

            CloseBinFile(&reader);
            CloseBinFile(&writer);
        }

        FUN_10018880("XCHNG\\STATE\\map_landnames", "XCHNG\\TOGAME\\map_landnames");

        {
            OpenBinFile(&reader, "XCHNG\\STATE\\map_objects", BINFILEOPENTYPE_READ);
            OpenBinFile(&writer, "XCHNG\\TOGAME\\map_objects", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U16 value[4]; // TODO
            while (ReadBinFile(&reader, value, 8) != 0)// TODO
            {
                value[0] = value[0] + iStack_3e8 * -0x20;// TODO
                value[1] = value[1] + iStack_3ec * -0x20;// TODO

                WriteBinFile(&writer, value, 8);// TODO
            }

            CloseBinFile(&reader);
            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_desc", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteZipFile(&zip, &writer, uStack_400); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_unitnames", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteZipFile(&zip, &writer, uStack_400 << 4); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_groups", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &writer, 0xa8c); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_mapunits", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteZipFile(&zip, &writer, uStack_400); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_players", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteBinFile(&writer, &uStack_400, 4); // TODO

            CHAR value[37];
            ReadZipFile(&zip, value, 37); // TODO

            strcpy(value, name);

            WriteBinFile(&writer, value, 37); // TODO
            WriteZipFile(&zip, &writer, uStack_400 * 0x161 - 37); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_woofers", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteBinFile(&writer, &uStack_400, 4); // TODO
            WriteZipFile(&zip, &writer, uStack_400 * 0x4e); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_zones", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &writer, iStack_3f8 * uStack_3f4); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_scripts", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 iStack_3e4; // TODO
            ReadZipFile(&zip, &iStack_3e4, 4); // TODO
            WriteBinFile(&writer, &iStack_3e4, 4); // TODO

            for (U32 x = 0; x < iStack_3e4; x++)
            {
                U32 uStack_3fc; // TODO
                ReadZipFile(&zip, &uStack_3fc, 4); // TODO

                uStack_3fc = uStack_3fc - 4; // TODO

                WriteBinFile(&writer, &uStack_3fc, 4); // TODO

                U32 local_3d8; // TODO
                ReadZipFile(&zip, &local_3d8, 4); // TODO
                WriteZipFile(&zip, &writer, uStack_3fc); // TODO
            }

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\map_mines", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
            OpenBinFile(&reader, "XCHNG\\STATE\\map_mines", BINFILEOPENTYPE_READ);

            for (U32 x = 0; x < iStack_3f8; x++)
            {
                CHAR abStack_30c[256]; // TODO
                CHAR abStack_10c[256]; // TODO

                ReadZipFile(&zip, abStack_30c, uStack_3f4);
                PointBinFile(&reader, (x + iStack_3ec) * iStack_3f0 + iStack_3e8, FILE_BEGIN);
                ReadBinFile(&reader, abStack_10c, uStack_3f4);

                for (U32 xx = 0; xx < uStack_3f4; xx++)
                {
                    abStack_30c[xx] = abStack_30c[xx] | abStack_10c[xx];
                }

                WriteBinFile(&writer, abStack_30c, uStack_3f4);
            }

            CloseBinFile(&reader);
            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_support", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteZipFile(&zip, &writer, uStack_400); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_phrases", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            U32 uStack_400; // TODO
            ReadZipFile(&zip, &uStack_400, 4); // TODO
            WriteZipFile(&zip, &writer, uStack_400); // TODO

            CloseBinFile(&writer);
        }

        {
            OpenBinFile(&writer, "XCHNG\\TOGAME\\mis_objects", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

            WriteZipFile(&zip, &writer, 400); // TODO

            CloseBinFile(&writer);
        }

        CloseZipFile(&zip);
    }

    return TRUE;
}

// 0x10018880
VOID FUN_10018880(LPCSTR source, LPCSTR destination) // TODO
{
    BINFILE reader = { (BFH)INVALID_BINFILE_VALUE };
    BINFILE writer = { (BFH)INVALID_BINFILE_VALUE };

    OpenBinFile(&reader, source, BINFILEOPENTYPE_READ);
    OpenBinFile(&writer, destination, BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);

    CopyBinFile(&reader, &writer, AcquireBinFileSize(&reader));

    CloseBinFile(&reader);
    CloseBinFile(&writer);
}

// 0x100185f0
BOOL FUN_100185f0(LPCSTR path) // TODO name
{
    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, path, ZIPFILE_OPEN_READ)) { return FALSE; }

    U32 local_14[5]; // TODO
    ReadZipFile(&zip, local_14, 20); // TODO

    BINFILE file = { (BFH)INVALID_BINFILE_VALUE };

    {
        U32 local_ac; // TODO
        ReadZipFile(&zip, &local_ac, 4); // TODO
        OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
        WriteZipFile(&zip, &file, local_ac);
        CloseBinFile(&file);
    }

    {
        OpenBinFile(&file, "XCHNG\\game.sav", BINFILEOPENTYPE_CREATE | BINFILEOPENTYPE_WRITE);
        WriteZipFile(&zip, &file, 10000000); // TODO
        CloseBinFile(&file);
    }

    CloseZipFile(&zip);

    return TRUE;
}
