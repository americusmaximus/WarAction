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
    self->Map_Header1.Unk00 = 0; // TODO
    self->Map_Header1.Unk01 = 0; // TODO
    self->Map_Header1.Unk02 = 0; // TODO
    self->Map_Header1.Unk03 = 0; // TODO

    self->Map_Header2.Actors.Min_Players = 0;
    self->Map_Header2.Actors.Max_Players = 0;
    self->Map_Header2.Actors.Unk02 = 0; // TODO
    self->Map_Header2.Actors.Unk03 = 0; // TODO

    self->Map_Header2.TypeAndSize.Type = 0; // TODO
    self->Map_Header2.TypeAndSize.Width = 0;
    self->Map_Header2.TypeAndSize.Height = 0;

    self->Mis_Desc = NULL;
    self->Pixels = NULL;
    self->MissionDescription = NULL;

    return self;
}

// 0x10017e50
VOID CLASSCALL DisposeMap(MAPPTR self)
{
    if (self->Mis_Desc != NULL)
    {
        free(self->Mis_Desc);

        self->Mis_Desc = NULL;
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

    ReadZipFile(&zip, &map->Map_Header1, sizeof(MAPHEADER));
    ReadZipFile(&zip, &map->Map_Header2, sizeof(MAPSTRUCT2));

    map->Map_Header2.Actors.Min_Players = 1;
    map->Map_Header2.Actors.Max_Players = 1;
    map->Map_Header2.Actors.Unk02 = 1; // TODO
    map->Map_Header2.Actors.Unk03 = 1; // TODO

    {
        U32 length = 0;
        ReadZipFile(&zip, &length, sizeof(U32));

        LPSTR description = (LPSTR)malloc(length + 1);
        description[length] = NULL;

        ReadZipFile(&zip, description, length);

        map->Mis_Desc = AcquireCleanUnicodeString(description, TRUE);
    }

    if (map->Mis_Desc[0] == '$')
    {
        U32 x = 0;
        LPSTR description = map->Mis_Desc;

        while (description[x] != '\n' && description[x] != '\r') { x++; }

        description[x] = NULL;

        do { do { x++; } while (description[x] == '\n'); } while (description[x] == '\r');

        map->Mis_Desc = (LPSTR)malloc(strlen(&description[x]) + 1);

        strcpy(map->Mis_Desc, &description[x]);

        map->MissionDescription = (LPSTR)malloc(strlen(description));

        strcpy(map->MissionDescription, description);

        free(description);
    }
    else { map->MissionDescription = NULL; }

    for (U32 x = 0; map->Mis_Desc[x] != NULL; x++)
    {
        if (map->Mis_Desc[x] == '#') { map->Mis_Desc[x] = NULL; break; }
    }

    {
        CONST U32 size = map->Map_Header2.TypeAndSize.Width <= MAX_MAP_SIZE && map->Map_Header2.TypeAndSize.Height <= MAX_MAP_SIZE
            ? map->Map_Header2.TypeAndSize.Height * map->Map_Header2.TypeAndSize.Width * sizeof(PIXEL)
            : map->Map_Header2.TypeAndSize.Height * map->Map_Header2.TypeAndSize.Width / 2;

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

    ReadZipFile(&zip, &map->Map_Header1, sizeof(MAPHEADER));
    ReadZipFile(&zip, &map->Map_Header2.Actors, sizeof(MAPMINMAX));

    ReadZipFile(&zip, &map->Mis_objects.Unk00, MAX_MAP_STRUCT3_COUNT * sizeof(MAPSTRUCT3));

    for (U32 x = 0; x < MAX_MAP_STRUCT3_COUNT; x++)
    {
        if (map->Mis_objects.Unk00[x].Unk00 < 0 || map->Mis_objects.Unk00[x].Unk01 < 0
            || map->Map_Header2.TypeAndSize.Width <= map->Mis_objects.Unk00[x].Unk00 || map->Map_Header2.TypeAndSize.Height <= map->Mis_objects.Unk00[x].Unk01
            || map->Mis_objects.Unk00[x].Unk02 < 0 || (MAX_MAP_SIZE - 1) < map->Mis_objects.Unk00[x].Unk02)
        {
            map->Mis_objects.Unk00[x].Unk00 = 0;
            map->Mis_objects.Unk00[x].Unk02 = 0;
            map->Mis_objects.Unk00[x].Unk01 = 0;
        }
    }

    ReadZipFile(&zip, &map->Map_Header2.TypeAndSize, sizeof(MAPTYPESIZE));

    for (U32 x = 0; x < MAX_MAP_STRUCT4_COUNT; x++)
    {
        ReadZipFile(&zip, &map->Mis_objects.Unk01[x], sizeof(U32));
        map->Mis_objects.Unk01[x].Unk01 = 0xffff; // TODO
    }

    {
        CHAR buffer[272]; // TODO
        ReadZipFile(&zip, buffer, 272); // TODO
    }

    ScriptParser(map, &zip);

    {
        U32 length = 0;
        ReadZipFile(&zip, &length, sizeof(U32));

        LPSTR description = (LPSTR)malloc(length + 1);
        description[length] = NULL;

        ReadZipFile(&zip, description, length);

        map->Mis_Desc = AcquireCleanUnicodeString(description, TRUE);
    }

    if (map->Mis_Desc[0] == '$')
    {
        U32 x = 0;
        LPSTR description = map->Mis_Desc;

        while (description[x] != '\n' && description[x] != '\r') { x++; }

        description[x] = NULL;

        do { do { x++; } while (description[x] == '\n'); } while (description[x] == '\r');

        map->Mis_Desc = (LPSTR)malloc(strlen(&description[x]) + 1);

        strcpy(map->Mis_Desc, &description[x]);

        map->MissionDescription = (LPSTR)malloc(strlen(description));

        strcpy(map->MissionDescription, description);

        free(description);
    }
    else { map->MissionDescription = NULL; }

    for (U32 x = 0; map->Mis_Desc[x] != NULL; x++)
    {
        if (map->Mis_Desc[x] == '#') { map->Mis_Desc[x] = NULL; break; }
    }

    {
        CONST U32 size = map->Map_Header2.TypeAndSize.Width <= MAX_MAP_SIZE && map->Map_Header2.TypeAndSize.Height <= MAX_MAP_SIZE
            ? map->Map_Header2.TypeAndSize.Height * map->Map_Header2.TypeAndSize.Width * sizeof(PIXEL)
            : map->Map_Header2.TypeAndSize.Height * map->Map_Header2.TypeAndSize.Width / 2; // TODO

        map->Pixels = (PIXEL*)malloc(size);
        ReadZipFile(&zip, map->Pixels, size);
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10017f60
BOOL ScriptParser(MAPPTR map, ZIPFILEPTR file) // TODO
{
    U32 script_num = 0;
    ReadZipFile(file, &script_num, sizeof(U32));

    for (U32 x = 0; x < script_num; x++)
    {
        U32 script_length = 0;
        ReadZipFile(file, &script_length, sizeof(U32));

        if (script_length != 0)
        {
            LPVOID scripts = malloc(script_length); // TODO
            ReadZipFile(file, scripts, script_length);
            ScriptCommandPerser(map, scripts); // TODO Decoding script commands
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

//Decoding script commands
void ScriptCommandPerser(MAPPTR map, LPVOID value)
{
    S32 v7; // [esp+4h] [ebp-1004h]
    S8 v8; // [esp+8h] [ebp-1000h] BYREF

    S32* result = (S32*)value;
    S32* position = (S32*)value;
    if (*(S32*)value != SCRIPT_END)
    {
        S32 v4 = v7;
        S32* Stack_for_num_scripts = (S32*)&v8;
        do
        {
            S32 scripts = *position;
            if (*position >= 0) // Filter conditions and parameters for these conditions
            {
                switch (scripts)
                {
                case AND:
                case OR:
                case IF_COUNT_DOWN:
                case IF_TIME_FROM_MISSION_START:
                case IF_GROUP_BEHAVIOUR:
                case IF_GROUP_BEHAVIOUR_ZONE1:
                case IF_GROUP_BEHAVIOUR_ZONE2:
                case IF_GROUP_BEHAVIOUR_GROUP1:
                case IF_GROUP_BEHAVIOUR_GROUP2:
                    v4 = *(Stack_for_num_scripts - 2);
                    --Stack_for_num_scripts;
                    break;
                case IF_GROUP_UNIT_COUNT_OF_TYPE:
                case IF_PLAYER_UNIT_COUNT_OF_TYPE:
                case IF_ZONE_UNIT_COUNT_FOR_GROUP:
                case IF_ZONE_UNIT_COUNT_FOR_PLAYER:
                case IF_PLANES_FOR_PLAYER_COUNT:
                case IF_PLANES_FOR_PLAYER_MISSIONS_COUNT:
                case IF_ZONE_UNIT_COUNT_PERCENT_FOR_GROUP:
                case IF_ZONE_UNIT_COUNT_PERCENT_FOR_PLAYER:
                    v4 = *(Stack_for_num_scripts - 4);
                    Stack_for_num_scripts -= 3;
                    break;
                case RETURN_IF_FALSE:
                case TIMER_STOP:
                case ZONE_POINT_TO:
                case PHRASE_SHOW:
                case COUNT_DOWN_START:
                case CAMPAIGN_SET_NEXT_MISSION:
                case MISSION_COMPLETE:
                case PLANES_ROUTE_ADD_OBJECT:
                case PLANES_ROUTE_ADD_OBJECT_AS_DROP:
                    --Stack_for_num_scripts;
                    v4 = *Stack_for_num_scripts;
                    break;
                case PLANES_FOR_PLAYER_SEND_TO_ZONE_AND_LAND:
                case PLANES_FOR_PLAYER_SEND_TO_OBJECT_AND_LAND:
                case MULTI_REINFORCEMENT_START:
                    Stack_for_num_scripts -= 4;
                    v4 = *Stack_for_num_scripts;
                    break;
                case TIMER_START:
                case GROUP_BEHAVIOUR_SET:
                case GROUP_BEHAVIOUR_ZONE_1SET:
                case GROUP_BEHAVIOUR_ZONE_2SET:
                case GROUP_BEHAVIOUR_GROUP_1SET:
                case GROUP_BEHAVIOUR_GROUP_2SET:
                case VARIABLE_SET:
                case PHRASE_SHOW_AND_POINT_TO_MARKER:
                case GROUP_GIVE_TO_PLAYER:
                case GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_ZONE:
                case GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_OBJECT:
                    Stack_for_num_scripts -= 2;
                    v4 = *Stack_for_num_scripts;
                    break;
                case PLANES_ADD_TO_PLAYER:
                case PLANES_MISSIONS_ADD_TO_PLAYER:
                case PLANES_ROUTE_SEND_AND_LAND_FOR_PLAYER:
                case VARIABLE_MODIFY:
                    Stack_for_num_scripts -= 3;
                    v4 = *Stack_for_num_scripts;
                    break;
                case NEGATE:
                case IF_TIMER_COMPLETE:
                case IF_OBJECT_IS_DESTROYED:
                    v4 = *(Stack_for_num_scripts - 1);
                    break;
                case IF_GROUP_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_GROUP:
                    v4 = *(Stack_for_num_scripts - 6);
                    Stack_for_num_scripts -= 5;
                    break;
                case IF_PLAYER_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_PLAYER:
                    v4 = *(Stack_for_num_scripts - 5);
                    Stack_for_num_scripts -= 4;
                    break;
                case GROUP_RESURRECT_THROUGH_FLAG_WITH_PARAMS:
                    Stack_for_num_scripts -= 7;
                    v4 = *Stack_for_num_scripts;
                    break;
                case REINFORCEMENT_SEND:
                    Stack_for_num_scripts -= 5;
                    break; // 50/50
                case IF_GROUP_WAS_ATTACKED_TIME_AGO:
                case IF_VARIABLE_VALUE:
                    v4 = *(Stack_for_num_scripts - 3);
                    Stack_for_num_scripts -= 2;
                    break;
                case IF_MiISSION_START:
                    *Stack_for_num_scripts = v4;
                    Stack_for_num_scripts++;
                    break;
                case MULTI_REINFORCEMENT_FOR_FLAGS:
                    v4 = *(Stack_for_num_scripts - 5);
                    Stack_for_num_scripts -= 5;
                    //CommandParser(map, v4);
                    break;
                case MULTI_PLANES_FOR_FLAGS:
                case MULTI_PLANES_MISSIONS_FOR_FLAGS:
                    v4 = *(Stack_for_num_scripts - 3);
                    Stack_for_num_scripts -= 3;
                    //CommandParser(map, v4);
                    break;
                case MULTI_PHRASE_FOR_FLAGS:
                    v4 = *(Stack_for_num_scripts - 2);
                    Stack_for_num_scripts -= 2;
                    //CommandParser(map, v4);
                    break;
                default:
                    break;
                }
            }
            else
            {
                *Stack_for_num_scripts = scripts & 0x7FFFFFFF; //remove the negative value from the parameter
                ++Stack_for_num_scripts;
            }
            result = (S32*)position[1];
            ++position;
        } while (result != (S32*)SCRIPT_END);
    }
    //return (S32)result;
    //return 0;
}

//S16* CommandParser(MAPPTR map, int a2)
//{
//    S32 v2 = -1;
//    S16* v3 = (S16*)(map + 144);
//    map->Mis_Scripts.Unk00;
//    S32 v4 = 32;
//    do
//    {
//        if (*v3 > v2)
//            v2 = *v3;
//        v3 += 3;
//        --v4;
//    } while (v4);
//    S16 v5 = v2 + 1;
//    S16* result = (S16*)(map + 144);
//    for (S32 i = 0; i < 32; ++i)
//    {
//        if (((1 << i) & a2) != 0)
//            *result = v5;
//        result += 3;
//    }
//    return result;
//}