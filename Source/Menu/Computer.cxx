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

#include "Computer.hxx"
#include "FileFinder.hxx"
#include "Settings.hxx"

#include <../Text/Resources.hxx>

#include <stdlib.h>

#define MAX_COMPUTER_BUFFER_LENGTH  256

// 0x10046290
COMPUTER ComputerState;

// 0x100055f0
VOID CLASSCALL ActivateComputer(COMPUTERPTR self, LPCSTR type)
{
    self->Type = type;

    {
        STRINGVALUE value;
        AcquireStringValue(&value, "%ssUpb", type);
        CONST S32 count = AcquireSettingsValue(value, 0);

        self->Count = self->Capacity = count;
    }

    if (self->Count == 0)
    {
        if (strncmp(self->Type, COMPUTER_SESSION_TYPE_PHONE, COMPUTER_SESSION_TYPE_PHONE_LENGTH) != 0) { self->Count = 1; }
    }

    self->Players = (LPSTR)malloc(self->Count * MAX_PLAYER_NAME_LENGTH);

    for (U32 x = 0; x < self->Count; x++)
    {
        CHAR player[MAX_COMPUTER_BUFFER_LENGTH];

        if (self->Count == self->Capacity)
        {
            STRINGVALUE name, value;
            AcquireStringValue(&name, "%s%d", self->Type, x);
            AcquireStringValue(&value, StringsState.Scratch);

            STRINGVALUE setting;
            STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);
            strcpy(player, actual->Value);
            ReleaseStringValue(actual);
        }
        else
        {
            DWORD length = MAX_COMPUTER_BUFFER_LENGTH;
            GetComputerNameA(player, &length);
        }

        player[MAX_PLAYER_NAME_LENGTH - 1] = NULL;

        strcpy(&self->Players[x * MAX_PLAYER_NAME_LENGTH], player);
    }
}

// 0x10005860
LPSTR CLASSCALL AcquireComputerPlayer(COMPUTERPTR self, CONST U32 indx)
{
    return &self->Players[indx * MAX_PLAYER_NAME_LENGTH];
}

// 0x10005b20
VOID CLASSCALL InitializeComputerPlayerList(COMPUTERPTR self, STRINGLISTPTR list)
{
    ReleaseStringList(list);

    for (U32 x = 0; x < self->Count; x++)
    {
        AppendStringList(list, AcquireComputerPlayer(self, x));
    }
}

// 0x10005760
VOID CLASSCALL ReleaseComputer(COMPUTERPTR self)
{
    {
        STRINGVALUE name, value;
        AcquireStringValue(&name, "%ssUpb", self->Type);
        AcquireStringValue(&value, "%d", self->Count);

        SaveSettingsValue(name, value);
    }

    for (U32 x = 0; x < self->Count; x++)
    {
        if (self->Capacity < x) { break; }

        STRINGVALUE name, value;
        AcquireStringValue(&name, "%s%d", self->Type, x);
        AcquireStringValue(&value, &self->Players[x * MAX_PLAYER_NAME_LENGTH]);

        SaveSettingsValue(name, value);
    }

    self->Capacity = self->Count = 0;

    free(self->Players);

    self->Players = NULL;
    self->Type = NULL;
}

// 0x10005aa0
VOID CLASSCALL AppendComputerPlayer(COMPUTERPTR self, LPCSTR player)
{
    self->Count = self->Count + 1;
    self->Players = (LPSTR)realloc(self->Players, self->Count * MAX_PLAYER_NAME_LENGTH);

    for (U32 x = self->Count - 1; x != 0; x--)
    {
        strcpy(AcquireComputerPlayer(self, x), AcquireComputerPlayer(self, x - 1));
    }

    strcpy(AcquireComputerPlayer(self, 0), player);
}

// 0x10005870
VOID CLASSCALL RemoveComputerPlayer(COMPUTERPTR self, CONST U32 indx, CONST BOOL remove)
{
    if (remove)
    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_BASE_SAVES);
        AcquireStringValue(&value, StringsState.Scratch);

        CHAR path[MAX_FILE_NAME_LENGTH];

        {
            STRINGVALUE setting;
            STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);
            wsprintfA(path, "%s%s\\*.*", actual->Value, AcquireComputerPlayer(self, indx));
            ReleaseStringValue(actual);
        }

        FINDFILECONTEXT context;
        HANDLE handle = FindFile(path, &context);

        {
            LPSTR slash = strrchr(path, '\\');
            if (slash != NULL) { slash[1] = NULL; }
        }

        if (handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                strncpy(path, context.Path, MAX_FILE_NAME_LENGTH);

                DeleteFileA(path);

                {
                    LPSTR slash = strrchr(path, '\\');
                    if (slash != NULL) { slash[1] = NULL; }
                }
            } while (FindFileNext(handle, &context));
        }

        FindClose(handle);
        RemoveDirectoryA(path);
    }

    for (U32 x = indx; x < self->Count - 1; x++)
    {
        strcpy(AcquireComputerPlayer(self, x), AcquireComputerPlayer(self, x + 1));
    }

    self->Count = self->Count - 1;
    self->Players = (LPSTR)realloc(self->Players, self->Count * MAX_PLAYER_NAME_LENGTH);
}

// 0x10005a30
VOID CLASSCALL RemoveComputerPlayer(COMPUTERPTR self, LPCSTR player, CONST BOOL remove)
{
    if (self->Count == 0) { return; }

    for (U32 x = 0; x < self->Count; x++)
    {
        if (strcmpi(player, AcquireComputerPlayer(self, x)) == 0)
        {
            RemoveComputerPlayer(self, x, remove); return;
        }
    }
}

// 0x100221a0
STATIC BOOL CreateFolder(LPCSTR path)
{
    CHAR dir[MAX_FILE_NAME_LENGTH];

    AcquireAnsiString(dir, MAX_FILE_NAME_LENGTH, path, -1);

    return CreateDirectoryA(dir, NULL);
}

// 0x10005c50
VOID CLASSCALL SaveComputerPlayer(COMPUTERPTR self, LPCSTR player)
{
    RemoveComputerPlayer(self, player, FALSE);
    AppendComputerPlayer(self, player);

    STRINGVALUE path;

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_BASE_SAVES);
        AcquireSettingsValue(&value, IDS_NULL);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        AcquireStringValue(&path, "%s%s\\", actual->Value, player);
        ReleaseStringValue(actual);
    }

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SAVES);
        AcquireStringValue(&value, &path);

        SaveSettingsValue(name, value);
    }

    CreateFolder(path.Value);

    ReleaseStringValue(&path);
}

// 0x10005b60
VOID CLASSCALL SortComputerPlayers(COMPUTERPTR self)
{
    for (U32 x = 0; x < self->Count; x++)
    {
        for (U32 xx = 1; xx < self->Count; xx++)
        {
            if (strcmpi(AcquireComputerPlayer(self, x), AcquireComputerPlayer(self, xx)) < 0)
            {
                CHAR player[MAX_COMPUTER_BUFFER_LENGTH];

                // TODO check order
                strcpy(player, AcquireComputerPlayer(self, x));
                strcpy(AcquireComputerPlayer(self, x), AcquireComputerPlayer(self, xx));
                strcpy(AcquireComputerPlayer(self, xx), player);
            }
        }
    }
}