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

#include "Saves.hxx"
#include "Settings.hxx"
#include "Statistics.hxx"
#include "Strings.hxx"
#include "ZipFile.hxx"

#include <../Text/Resources.hxx>

// 0x1003f88c
STATIC CHAR StatisticsFileExtension[] = "rts";

// 0x1003f8b4
STATIC CHAR StatisticsFileExtensionDifficulties[] = "eth";

STATISTICSMODULECONTAINER StatisticsState;

// 0x1000c7a0
STATISTICSPTR CLASSCALL ActivateStatistics(STATISTICSPTR self)
{
    self->Name[0] = NULL;

    return self;
}

// 0x1000c920
VOID CLASSCALL AcquireStatistics(STATISTICSPTR self, LPCSTR path)
{
    SaveStatistics(self);

    strcpy(self->Path, path);
    ZeroMemory(&self->Players, MAX_STATISTICS_PLAYERS_COUNT * sizeof(PLAYER));

    CONST BOOL single = path[strlen(path) - 2] == SINGLE_FILE_EXTENSION;

    if (single) { strcpy(self->Name, SaveState.Path); } // TODO is this correct?

    STRINGVALUE name, value;
    AcquireSettingsValue(&name, IDS_RATINGS_PATH);
    AcquireStringValue(&value, StringsState.Scratch);

    STRINGVALUE setting; // TODO name
    STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value); // TODO name

    STRINGVALUE full; // TODO name
    STRINGVALUEPTR todo = AcquireStringValue(&full, "%s%s", actual->Value, self->Name); // TODO name

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));
    CONST BOOL success = OpenZipFile(&zip, AcquireStringValueValue(todo), ZIPFILE_OPEN_WRITE_MODIFY);

    ReleaseStringValue(todo);
    ReleaseStringValue(actual);

    if (success)
    {
        ReadZipFile(&zip, &self->Players, MAX_PLAYER_COUNT * sizeof(PLAYER));

        if (single) { ReadZipFile(&zip, self->Path, MAX_FILE_NAME_LENGTH); }

        CloseZipFile(&zip);
    }
}

// 0x1000cd10
VOID CLASSCALL SaveStatistics(STATISTICSPTR self)
{
    if (!self->Name[0] == NULL) { return; }

    STRINGVALUE name, value;
    AcquireSettingsValue(&name, IDS_RATINGS_PATH);
    AcquireStringValue(&value, StringsState.Scratch);

    STRINGVALUE setting; // TODO name
    STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value); // TODO name

    STRINGVALUE path; // TODO name
    STRINGVALUEPTR todo = AcquireStringValue(&path, "%s%s", actual->Value, self->Name); // TODO name

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));
    CONST BOOL success = OpenZipFile(&zip, AcquireStringValueValue(todo), ZIPFILE_OPEN_WRITE_MODIFY);

    ReleaseStringValue(todo);
    ReleaseStringValue(actual);

    if (success)
    {
        WriteZipFile(&zip, &self->Players, MAX_PLAYER_COUNT * sizeof(PLAYER));
        CloseZipFile(&zip);
    }

    self->Name[0] = NULL;

    ZeroMemory(&self->Players, MAX_STATISTICS_PLAYERS_COUNT * sizeof(PLAYER));
}

// 0x1000ced0
LPCSTR AcquireStatisticsFileExtension(CONST CHAR game, CONST GAMEDIFFICULTY difficulty)
{
    if (difficulty == GAMEDIFFICULTY_UNKNOWN)
    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, IDS_DIFFICULTY);
        wsprintfA(StatisticsFileExtension, "r%c%c",
            StatisticsFileExtensionDifficulties[AcquireSettingsValue(value, GAMEDIFFICULTY_EASY)], game);
    }
    else { wsprintfA(StatisticsFileExtension, "r%c%c", StatisticsFileExtensionDifficulties[difficulty], game); }

    return StatisticsFileExtension;
}

// 0x1000ce50
U32 CLASSCALL AcquireStatisticsPlayerIndex(STATISTICSPTR self, PLAYERPTR player)
{
    U32 indx = MAX_PLAYER_COUNT;

    if (player->Status == STATUS_VICTORY)
    {
        for (indx = MAX_PLAYER_COUNT - 1; indx != 0; indx--)
        {
            CONST PLAYERPTR current = &self->Players[indx];

            if (indx == 0 || player->Score <= current->Score) { break; }

            CopyMemory(&self->Players[indx + 1], current, sizeof(PLAYER));
        }
    }

    CopyMemory(&self->Players[indx], player, sizeof(PLAYER));

    if (self->Players[indx].Status != STATUS_VICTORY) { self->Players[indx].Stars = 0; }

    return indx;
}

// 0x1000cac0
VOID CLASSCALL FUN_1000cac0(STATISTICSPTR self, LPCSTR path, STRINGLISTPTR list)
{
    // TODO Not implemented
}