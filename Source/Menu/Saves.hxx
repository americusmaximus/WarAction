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

#pragma once

#include "BinFile.hxx"

#include <Game.hxx>

#define DEFAULT_GAME_MAP_INDEX          (-1)
#define DEFAULT_GAME_MISSION_INDEX      (-1)

#define SAVE_STATE_FILE_START_MAGIC     0x4745434b /* GECK */ /* TODO NAME*/
#define SAVE_STATE_FILE_END_MAGIC       0x45434b47 /* ECKG */ /* TODO NAME*/

#define SAVE_STATE_SAVE_FILE_MAGIC      0x1090904  /* TODO NAME*/

#define DEFAULT_SAVE_STATE_TURN_DELAY   35

typedef struct SaveFileItem
{
    FILETIME    Time;
    CHAR        Name[MAX_FILE_NAME_LENGTH];
} SAVEFILEITEM, * SAVEFILEITEMPTR;

typedef struct SaveFileDefinition
{
    CHAR    Name[MAX_FILE_NAME_LENGTH];
    U32     Unk01; // TODO
    U32     Unk02; // TODO
    U32     Unk03; // TODO
    U32     Unk04; // TODO
    U32     Unk05; // TODO
    U32     Unk06; // TODO
    U32     Unk07; // TODO
    U32     Unk08; // TODO
    S32     Map;
    S32     Mission;
} SAVEFILEDEFINITION, * SAVEFILEDEFINITIONPTR;

typedef struct SaveFileContent
{
    BYTE Unk[300]; // TODO
} SAVEFILECONTENT, * SAVEFILECONTENTPTR;

typedef struct SaveModuleStateContainer // TODO better name
{
    U32                 Version;
    SAVEFILECONTENTPTR  Content;
    U32                 Unk02; // TODO
    U32                 Unk03; // TODO
    U32                 Unk04; // TODO
    U32                 Unk05; // TODO
    U32                 Unk06; // TODO
    U32                 Unk07; // TODO
    U32                 Unk08; // TODO
    CHAR                Path[MAX_FILE_NAME_LENGTH]; // TODO name
    S32                 Map;
    S32                 Mission;
    GAMEDIFFICULTY      Difficulty;
} SAVEMODULESTATECONTAINER, * SAVEMODULESTATECONTAINERPTR;

EXTERN SAVEMODULESTATECONTAINER SaveState;

BOOL InitializeGameState(VOID);
VOID DeleteGameState(VOID);

// TODO Improve names of the functions

BOOL LoadGameState(BINFILEPTR save);
BOOL LoadSaveState(LPCSTR save);
BOOL SaveGameState(BINFILEPTR save);
BOOL WriteSaveState(LPCSTR save);

BOOL UnpackSaveFile(LPCSTR save);

S32 AcquireCurrentGameMap(VOID);
S32 AcquireCurrentGameMission(VOID);
BOOL SaveMapMission(CONST S32 map, CONST S32 mission);
BOOL SaveMission(CONST S32 mission);
BOOL SaveMap(CONST S32 map);
BOOL SaveMapState(CONST S32 map);
BOOL FUN_10018c00(LPCSTR name); // TODO name
VOID FUN_10018880(LPCSTR source, LPCSTR destination); // TODO name
BOOL FUN_100185f0(LPCSTR path); // TODO name