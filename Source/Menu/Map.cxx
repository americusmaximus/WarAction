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

#include "Map.hxx"
#include "Strings.hxx"
#include "Version.hxx"

#include <stdlib.h>

// 0x10017e00
MAPPTR CLASSCALL ActivateMap(MAPPTR self)
{
    self->Header.Unk00 = 0; // TODO
    self->Header.Unk01 = 0; // TODO
    self->Header.Unk02 = 0; // TODO
    self->Header.Unk03 = 0; // TODO

    self->Descriptor.Actors.Min_Players = 0;
    self->Descriptor.Actors.Max_Players = 0;
    self->Descriptor.Actors.Unk02 = 0; // TODO
    self->Descriptor.Actors.Unk03 = 0; // TODO

    self->Descriptor.Details.Type = MAPTYPE_SUMMER;
    self->Descriptor.Details.Width = 0;
    self->Descriptor.Details.Height = 0;

    self->Description = NULL;
    self->Pixels = NULL;
    self->MissionDescription = NULL;

    return self;
}

// 0x10017e50
VOID CLASSCALL DisposeMap(MAPPTR self)
{
    if (self->Description != NULL)
    {
        free(self->Description);

        self->Description = NULL;
    }

    if (self->MissionDescription != NULL)
    {
        free(self->MissionDescription);

        self->MissionDescription = NULL;
    }

    if (self->Pixels != NULL)
    {
        free(self->Pixels);

        self->Pixels = NULL;
    }
}

// 0x1001aaa0
BOOL InitializeSingleMap(LPCSTR name, MAPPTR map)
{
    DisposeMap(map);

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, ZIPFILE_OPEN_READ)) { return FALSE; }

    {
        U32 magic = 0;
        ReadZipFile(&zip, &magic, sizeof(U32));

        if (magic != MAP_FILE_SINGLE_MAGIC)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    {
        U32 version = 0;
        ReadZipFile(&zip, &version, sizeof(U32));

        if (version != MODULE_VERSION_VALUE)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    ReadZipFile(&zip, &map->Header, sizeof(MAPHEADER));
    ReadZipFile(&zip, &map->Descriptor, sizeof(MAPDESCRIPTOR));

    map->Descriptor.Actors.Min_Players = 1;
    map->Descriptor.Actors.Max_Players = 1;
    map->Descriptor.Actors.Unk02 = 1; // TODO
    map->Descriptor.Actors.Unk03 = 1; // TODO

    {
        U32 length = 0;
        ReadZipFile(&zip, &length, sizeof(U32));

        LPSTR description = (LPSTR)malloc(length + 1);
        description[length] = NULL;

        ReadZipFile(&zip, description, length);

        map->Description = AcquireCleanUnicodeString(description, TRUE);
    }

    if (map->Description[0] == '$')
    {
        U32 x = 0;
        LPSTR description = map->Description;

        while (description[x] != '\n' && description[x] != '\r') { x++; }

        description[x] = NULL;

        do { do { x++; } while (description[x] == '\n'); } while (description[x] == '\r');

        map->Description = (LPSTR)malloc(strlen(&description[x]) + 1);

        strcpy(map->Description, &description[x]);

        map->MissionDescription = (LPSTR)malloc(strlen(description) + 1);

        strcpy(map->MissionDescription, description);

        free(description);
    }
    else { map->MissionDescription = NULL; }

    for (U32 x = 0; map->Description[x] != NULL; x++)
    {
        if (map->Description[x] == '#') { map->Description[x] = NULL; break; }
    }

    {
        CONST U32 size = map->Descriptor.Details.Width <= MAX_MAP_SIZE && map->Descriptor.Details.Height <= MAX_MAP_SIZE
            ? map->Descriptor.Details.Height * map->Descriptor.Details.Width * sizeof(PIXEL)
            : map->Descriptor.Details.Height * map->Descriptor.Details.Width / 2;

        map->Pixels = (PIXEL*)malloc(size);
        ReadZipFile(&zip, map->Pixels, size);
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10018240
BOOL InitializeMultiMap(LPCSTR name, MAPPTR map)
{
    DisposeMap(map);

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, ZIPFILE_OPEN_READ)) { return FALSE; }

    {
        U32 magic = 0;
        ReadZipFile(&zip, &magic, sizeof(U32));

        if (magic != MAP_FILE_MULTI_MAGIC)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    {
        U32 version = 0;
        ReadZipFile(&zip, &version, sizeof(U32));
        if (version != MODULE_MULTI_MAP_VERSION_VALUE)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    ReadZipFile(&zip, &map->Header, sizeof(MAPHEADER));
    ReadZipFile(&zip, &map->Descriptor.Actors, sizeof(MAPMINMAX));

    ReadZipFile(&zip, &map->Objects.Unk00, MAX_MAP_STRUCT3_COUNT * sizeof(MAPSTRUCT3));

    for (U32 x = 0; x < MAX_MAP_STRUCT3_COUNT; x++)
    {
        if (map->Objects.Unk00[x].Unk00 < 0 || map->Objects.Unk00[x].Unk01 < 0
            || map->Descriptor.Details.Width <= map->Objects.Unk00[x].Unk00
            || map->Descriptor.Details.Height <= map->Objects.Unk00[x].Unk01
            || map->Objects.Unk00[x].Unk02 < 0 || (MAX_MAP_SIZE - 1) < map->Objects.Unk00[x].Unk02)
        {
            map->Objects.Unk00[x].Unk00 = 0;
            map->Objects.Unk00[x].Unk02 = 0;
            map->Objects.Unk00[x].Unk01 = 0;
        }
    }

    ReadZipFile(&zip, &map->Descriptor.Details, sizeof(MAPDETAILS));

    for (U32 x = 0; x < MAX_MAP_STRUCT4_COUNT; x++)
    {
        ReadZipFile(&zip, &map->Objects.Unk01[x], sizeof(U32));
        map->Objects.Unk01[x].Unk01 = 0xffff; // TODO
    }

    {
        CHAR buffer[272]; // TODO
        ReadZipFile(&zip, buffer, 272); // TODO
    }

    ParseMapScripts(map, &zip);

    {
        U32 length = 0;
        ReadZipFile(&zip, &length, sizeof(U32));

        LPSTR description = (LPSTR)malloc(length + 1);
        description[length] = NULL;

        ReadZipFile(&zip, description, length);

        map->Description = AcquireCleanUnicodeString(description, TRUE);
    }

    if (map->Description[0] == '$')
    {
        U32 x = 0;
        LPSTR description = map->Description;

        while (description[x] != '\n' && description[x] != '\r') { x++; }

        description[x] = NULL;

        do { do { x++; } while (description[x] == '\n'); } while (description[x] == '\r');

        map->Description = (LPSTR)malloc(strlen(&description[x]) + 1);

        strcpy(map->Description, &description[x]);

        map->MissionDescription = (LPSTR)malloc(strlen(description) + 1);

        strcpy(map->MissionDescription, description);

        free(description);
    }
    else { map->MissionDescription = NULL; }

    for (U32 x = 0; map->Description[x] != NULL; x++)
    {
        if (map->Description[x] == '#') { map->Description[x] = NULL; break; }
    }

    {
        CONST U32 size = map->Descriptor.Details.Width <= MAX_MAP_SIZE && map->Descriptor.Details.Height <= MAX_MAP_SIZE
            ? map->Descriptor.Details.Height * map->Descriptor.Details.Width * sizeof(PIXEL)
            : map->Descriptor.Details.Height * map->Descriptor.Details.Width / 2; // TODO

        map->Pixels = (PIXEL*)malloc(size);
        ReadZipFile(&zip, map->Pixels, size);
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10017f60
BOOL ParseMapScripts(MAPPTR map, ZIPFILEPTR file)
{
    U32 count = 0;
    ReadZipFile(file, &count, sizeof(U32));

    for (U32 x = 0; x < count; x++)
    {
        U32 length = 0;
        ReadZipFile(file, &length, sizeof(U32));

        if (length != 0)
        {
            S32* scripts = (S32*)malloc(length);
            ReadZipFile(file, scripts, length);
            ParseMapScriptCommands(map, scripts);
            free(scripts);
        }
    }

    return TRUE;
}

// 0x1001a9f0
BOOL ValidateSingleMapFile(LPCSTR name)
{
    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, 0)) { return FALSE; }

    U32 magic = 0;
    ReadZipFile(&zip, &magic, sizeof(U32));

    if (magic != MAP_FILE_SINGLE_MAGIC)
    {
        CloseZipFile(&zip);

        return FALSE;
    }

    U32 version = 0;
    ReadZipFile(&zip, &version, sizeof(U32));

    if (version != MODULE_VERSION_VALUE)
    {
        CloseZipFile(&zip);

        return FALSE;
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10017EB0
BOOL ValidateMultiMapFile(LPCSTR name)
{
    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, 0)) { return FALSE; }

    U32 magic = 0;
    ReadZipFile(&zip, &magic, sizeof(U32));

    if (magic != MAP_FILE_MULTI_MAGIC)
    {
        CloseZipFile(&zip);

        return FALSE;
    }

    U32 version = 0;
    ReadZipFile(&zip, &version, sizeof(U32));

    if (version != MODULE_MULTI_MAP_VERSION_VALUE)
    {
        CloseZipFile(&zip);

        return FALSE;
    }

    CloseZipFile(&zip);

    return TRUE;
}

#define GETCOMMAND(X) (*X)
#define INCREMENT(X, V) (X = X + V)

// 0x10017fe0
VOID ParseMapScriptCommands(MAPPTR map, S32* commands)
{
    S32 stack[MAX_SCRIPTS_COUNT];
    
    S32* ptr = stack;
    stack[MAX_SCRIPTS_COUNT - 1] = 0x10017fea; // TODO

    S32 value = 0;

    if (GETCOMMAND(commands) != SCRIPTSCOMMAND_SCRIPT_END)
    {
        do
        {
            if (GETCOMMAND(commands) >= SCRIPTSCOMMAND_NONE)
            {
                switch (GETCOMMAND(commands))
                {
                case SCRIPTSCOMMAND_AND:
                case SCRIPTSCOMMAND_OR:
                case SCRIPTSCOMMAND_IF_COUNT_DOWN:
                case SCRIPTSCOMMAND_IF_TIME_FROM_MISSION_START:
                case SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR:
                case SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_ZONE1:
                case SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_ZONE2:
                case SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_GROUP1:
                case SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_GROUP2:
                {
                    value = ptr[-2];
                    INCREMENT(ptr, -1); break;
                }
                case SCRIPTSCOMMAND_IF_GROUP_UNIT_COUNT_OF_TYPE:
                case SCRIPTSCOMMAND_IF_PLAYER_UNIT_COUNT_OF_TYPE:
                case SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_FOR_GROUP:
                case SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_FOR_PLAYER:
                case SCRIPTSCOMMAND_IF_PLANES_FOR_PLAYER_COUNT:
                case SCRIPTSCOMMAND_IF_PLANES_FOR_PLAYER_MISSIONS_COUNT:
                case SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_PERCENT_FOR_GROUP:
                case SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_PERCENT_FOR_PLAYER:
                {
                    value = ptr[-4];
                    INCREMENT(ptr, -3); break;
                }
                case SCRIPTSCOMMAND_RETURN_IF_FALSE:
                case SCRIPTSCOMMAND_TIMER_STOP:
                case SCRIPTSCOMMAND_ZONE_POINT_TO:
                case SCRIPTSCOMMAND_PHRASE_SHOW:
                case SCRIPTSCOMMAND_COUNT_DOWN_START:
                case SCRIPTSCOMMAND_CAMPAIGN_SET_NEXT_MISSION:
                case SCRIPTSCOMMAND_MISSION_COMPLETE:
                case SCRIPTSCOMMAND_PLANES_ROUTE_ADD_OBJECT:
                case SCRIPTSCOMMAND_PLANES_ROUTE_ADD_OBJECT_AS_DROP:
                {
                    value = ptr[-1];
                    INCREMENT(ptr, -1); break;
                }
                case SCRIPTSCOMMAND_PLANES_FOR_PLAYER_SEND_TO_ZONE_AND_LAND:
                case SCRIPTSCOMMAND_PLANES_FOR_PLAYER_SEND_TO_OBJECT_AND_LAND:
                case SCRIPTSCOMMAND_MULTI_REINFORCEMENT_START:
                {
                    value = ptr[-4];
                    INCREMENT(ptr, -4); break;
                }
                case SCRIPTSCOMMAND_TIMER_START:
                case SCRIPTSCOMMAND_GROUP_BEHAVIOUR_SET:
                case SCRIPTSCOMMAND_GROUP_BEHAVIOUR_ZONE_1SET:
                case SCRIPTSCOMMAND_GROUP_BEHAVIOUR_ZONE_2SET:
                case SCRIPTSCOMMAND_GROUP_BEHAVIOUR_GROUP_1SET:
                case SCRIPTSCOMMAND_GROUP_BEHAVIOUR_GROUP_2SET:
                case SCRIPTSCOMMAND_VARIABLE_SET:
                case SCRIPTSCOMMAND_PHRASE_SHOW_AND_POINT_TO_MARKER:
                case SCRIPTSCOMMAND_GROUP_GIVE_TO_PLAYER:
                case SCRIPTSCOMMAND_GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_ZONE:
                case SCRIPTSCOMMAND_GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_OBJECT:
                {
                    value = ptr[-2];
                    INCREMENT(ptr, -2); break;
                }
                case  SCRIPTSCOMMAND_PLANES_ADD_TO_PLAYER:
                case  SCRIPTSCOMMAND_PLANES_MISSIONS_ADD_TO_PLAYER:
                case  SCRIPTSCOMMAND_PLANES_ROUTE_SEND_AND_LAND_FOR_PLAYER:
                case  SCRIPTSCOMMAND_VARIABLE_MODIFY:
                {
                    value = ptr[-3];
                    INCREMENT(ptr, -2); break;
                }
                case SCRIPTSCOMMAND_NEGATE:
                case SCRIPTSCOMMAND_IF_TIMER_COMPLETE:
                case SCRIPTSCOMMAND_IF_OBJECT_IS_DESTROYED:
                {
                    value = ptr[-1]; break;
                }
                case SCRIPTSCOMMAND_IF_GROUP_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_GROUP:
                {
                    value = ptr[-6];
                    INCREMENT(ptr, -5); break;
                }
                case SCRIPTSCOMMAND_IF_PLAYER_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_PLAYER:
                {
                    value = ptr[-5];
                    INCREMENT(ptr, -4); break;
                }
                case SCRIPTSCOMMAND_GROUP_RESURRECT_THROUGH_FLAG_WITH_PARAMS:
                {
                    value = ptr[-7];
                    INCREMENT(ptr, -7); break;
                }
                case SCRIPTSCOMMAND_REINFORCEMENT_SEND:
                {
                    value = ptr[-5];
                    INCREMENT(ptr, -5); break;
                }
                case SCRIPTSCOMMAND_IF_GROUP_WAS_ATTACKED_TIME_AGO:
                case SCRIPTSCOMMAND_IF_VARIABLE_VALUE:
                {
                    value = ptr[-3];
                    INCREMENT(ptr, -2); break;
                }
                case SCRIPTSCOMMAND_IF_MISSION_START:
                {
                    *ptr = value;
                    INCREMENT(ptr, 1); break;
                }
                case SCRIPTSCOMMAND_MULTI_REINFORCEMENT_FOR_FLAGS:
                {
                    value = ptr[-5];
                    INCREMENT(ptr, -5);
                    FUN_100181f0(map, value); break;
                }
                case SCRIPTSCOMMAND_MULTI_PLANES_FOR_FLAGS:
                case SCRIPTSCOMMAND_MULTI_PLANES_MISSIONS_FOR_FLAGS:
                {
                    value = ptr[-3];
                    INCREMENT(ptr, -3);
                    FUN_100181f0(map, value); break;
                }
                case SCRIPTSCOMMAND_MULTI_PHRASE_FOR_FLAGS:
                {
                    value = ptr[-2];
                    INCREMENT(ptr, -2);
                    FUN_100181f0(map, value); break;
                }
                }
            }
            else
            {
                *ptr = GETCOMMAND(commands) & SCRIPTSCOMMAND_SCRIPT_END;
                INCREMENT(ptr, 1);
            }

            INCREMENT(commands, 1);

        } while (GETCOMMAND(commands) != SCRIPTSCOMMAND_SCRIPT_END);
    }
}

// 0x100181f0
VOID FUN_100181f0(MAPPTR map, S32 value)
{
    // TODO
}
