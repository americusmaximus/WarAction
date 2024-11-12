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

#include "Player.hxx"

#define GAME_VERSION_MAJOR          1
#define GAME_VERSION_MINOR          21

#define CAMPAIGN_FILE_EXTENSION     'c'
#define SINGLE_FILE_EXTENSION       's'
#define MULTIPLAYER_FILE_EXTENSION  'm'
// TODO find the right place for ^^

#define MAX_PLAYER_COUNT            12

#define INVALID_GAME_NATION         (-1)
#define GAME_NATION_RUSSIAN         0
#define GAME_NATION_GERMAN          1
#define GAME_NATION_BRITISH         2
#define GAME_NATION_AMERICAN        3
// TODO enum? ^^

#define MAX_GAME_MAP_COUNT          127 /* TODO NAME */

typedef enum GameDifficulty
{
    GAMEDIFFICULTY_NONE         = 0,
    GAMEDIFFICULTY_EASY         = 1,
    GAMEDIFFICULTY_MEDIUM       = 2,
    GAMEDIFFICULTY_HARD         = 3,
    GAMEDIFFICULTY_UNKNOWN      = 0xFFFFFFFF
} GAMEDIFFICULTY, * GAMEDIFFICULTYPTR;

#define MAX_GAMEDIFFICULTY_VALUE    (U32)(GAMEDIFFICULTY_HARD)

#pragma pack(push, 1)
typedef struct Game
{
    BOOL            IsNetwork;
    GAMEDIFFICULTY  Difficulty;
    S32             Unk02; // TODO
    S32             Unk03; // TODO
    S32             Unk04; // TODO
    S32             Unk05; // TODO
    S32             Unk06; // TODO
    S32             Unk07; // TODO
    U32             TurnDelay;
    S32             Unk09; // TODO
    U32             Ticks;
    STATUS          Status;
    S32             NextMap;
    S32             NextMission;
    PLAYER          Players[MAX_PLAYER_COUNT];
    U32             Time;
    CHAR            SaveFile[MAX_FILE_NAME_LENGTH]; // TODO name
} GAME, * GAMEPTR;
#pragma pack(pop)