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

#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"
#include "Renderer.Basic.hxx"

#define MAX_MAP_SIZE            128

#define MAX_MAP_STRUCT3_COUNT   16 /* TODO */
#define MAX_MAP_STRUCT4_COUNT   32 /* TODO */

#define MAP_FILE_SINGLE_MAGIC   0x4D535353 /* MSSS */
#define MAP_FILE_MULTI_MAGIC    0x4D4D5353 /* MMSS */

typedef struct MapHeader
{
    U32 Unk00; // TODO
    U32 Unk01; // TODO
    U32 Unk02; // TODO
    U32 Unk03; // TODO
} MAPHEADER, * MAPHEADERPTR;

typedef struct MapMinMax
{
    U32 Min_Players; // TODO
    U32 Max_Players; // TODO
    U32 Unk02; // TODO
    U32 Unk03; // TODO
} MAPMINMAX, * MAPMINMAXPTR;

typedef enum MapType
{
    MAPTYPE_SUMMER	= 0,
    MAPTYPE_WINTER	= 1,
    MAPTYPE_BEACH	= 2,
    MAPTYPE_DESERT	= 4
} MAPTYPE, * MAPTYPEPTR;

typedef struct MapDetails
{
    MAPTYPE Type;
    S32		Width;
    S32		Height;
} MAPDETAILS, * MAPDETAILSPTR;

typedef struct MapDescriptor
{
    MAPMINMAX   Actors;
    MAPDETAILS  Details;
} MAPDESCRIPTOR, * MAPDESCRIPTORPTR;

typedef struct MapStruct3 // TODO
{
    S16 Unk00; // TODO
    S16 Unk01; // TODO
    U16 Unk02; // TODO
} MAPSTRUCT3, * MAPSTRUCT3PTR;

typedef struct MapStruct4 // TODO
{
    U16 Unk00; // TODO
    U16 Unk01; // TODO
} MAPSTRUCT4, * MAPSTRUCT4PTR;

typedef struct MissionObjects
{
    MAPSTRUCT3 Unk00[MAX_MAP_STRUCT3_COUNT]; // TODO
    MAPSTRUCT4 Unk01[MAX_MAP_STRUCT4_COUNT]; // TODO
} MISSIONOBJECTS, * MISSIONOBJECTSPTR;

typedef struct MissionScripts
{
    S16 Unk00; // TODO;
} MISSIONSCRIPTS, * MISSIONSCRIPTSPTR;

typedef struct Map
{
    MAPHEADER       Header;
    MAPDESCRIPTOR   Descriptor;
    MISSIONOBJECTS  Objects;
    LPSTR           Description;
    PIXEL*          Pixels;
    LPSTR           MissionDescription;
    MISSIONSCRIPTS	Scripts;
} MAP, * MAPPTR;

typedef enum ScriptsCommands
{
    NONE = 0x00,
    AND,
    OR,
    IF_GROUP_UNIT_COUNT_OF_TYPE, //unit type, group, comparison type, quantity
    IF_PLAYER_UNIT_COUNT_OF_TYPE, //unit type, player, comparison type, quantity
    IF_ZONE_UNIT_COUNT_FOR_GROUP, //comparison type, quantity, group, zone
    IF_ZONE_UNIT_COUNT_FOR_PLAYER, //comparison type, quantity, player, zone
    RETURN_IF_FALSE,
    PLANES_FOR_PLAYER_SEND_TO_ZONE_AND_LAND, //Number, type of aircraft, player, zone, airfield
    IF_PLANES_FOR_PLAYER_COUNT, //Player, comparison type, quantity, aircraft type
    IF_PLANES_FOR_PLAYER_MISSIONS_COUNT, //Player, comparison type, quantity, aircraft type
    SCRIPT_TURN_OFF,
    TIMER_START, //timer number, time in seconds
    TIMER_STOP, //timer number
    ZONE_POINT_TO, //zone number
    PHRASE_SHOW, //phrase number
    COUNT_DOWN_START, //time in seconds
    CAMPAIGN_SET_NEXT_MISSION, //parameters of the next mission
    MISSION_COMPLETE, //result (0 - draw, 1 - win, 2- loss)
    GROUP_BEHAVIOUR_SET, //Group, behavior
    GROUP_BEHAVIOUR_ZONE_1SET, //Group, zone
    GROUP_BEHAVIOUR_ZONE_2SET, //Group, zone
    GROUP_BEHAVIOUR_GROUP_1SET, //Group, install group
    GROUP_BEHAVIOUR_GROUP_2SET, //Group, install group
    PLANES_ADD_TO_PLAYER, //Player, number, type of aircraft
    PLANES_MISSIONS_ADD_TO_PLAYER, //Player, number, type of aircraft
    NEGATE,
    IF_TIMER_COMPLETE, //timer number
    IF_COUNT_DOWN, //comparison type (0 - strictly, 1 - more than, 2 - less than), time in seconds
    IF_TIME_FROM_MISSION_START, //comparison type (1 - more than, 2 - less than), time in seconds
    IF_ZONE_UNIT_COUNT_PERCENT_FOR_GROUP, //comparison type, percentage, group, zone
    IF_ZONE_UNIT_COUNT_PERCENT_FOR_PLAYER, //comparison type, percentage, group, zone
    IF_GROUP_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_GROUP, //Unit type, group, comparison type, percentage, type in another group, another group
    IF_PLAYER_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_PLAYER, //Unit type, player, comparison type, percentage, other player's type, other player
    IF_GROUP_BEHAVIOUR, //Group, behavior
    IF_GROUP_BEHAVIOUR_ZONE1, //Group, zone
    IF_GROUP_BEHAVIOUR_ZONE2, //Group, zone
    IF_GROUP_BEHAVIOUR_GROUP1, //Group, verification group
    IF_GROUP_BEHAVIOUR_GROUP2, //Group, verification group
    GROUP_RESURRECT_THROUGH_FLAG_WITH_PARAMS, //Group, flag, Zone, Delay, Health%, Ammo %, Experience%, Morale % {SSRW + fuel}
    REINFORCEMENT_SEND, //Player, reinforcements, Flag, Zone, delay in seconds
    IF_OBJECT_IS_DESTROYED, //Object
    PLANES_FOR_PLAYER_SEND_TO_OBJECT_AND_LAND, //Number, type of aircraft, player, object, airfield
    PLANES_ROUTE_START, //No parameters
    PLANES_ROUTE_ADD_OBJECT, //Object
    PLANES_ROUTE_ADD_OBJECT_AS_DROP, //Object
    PLANES_ROUTE_SEND_AND_LAND_FOR_PLAYER, //Number, type of aircraft, player, airfield
    VARIABLE_SET, //Cell reference, value, or value reference
    VARIABLE_MODIFY, //Cell reference, operation (0 - increase, 1 - decrease), value, or value reference
    IF_GROUP_WAS_ATTACKED_TIME_AGO, //Group, type of comparison, time seconds
    IF_VARIABLE_VALUE, //Cell, comparison type, value, or cell reference
    IF_MiISSION_START,
    MULTI_REINFORCEMENT_START, //Reinforcement ru, reinforcement de, reinforcement an, reinforcement us, reinforcement J, delay in seconds
    MULTI_REINFORCEMENT_FOR_FLAGS, //Set of flags, ru reinforcement, de reinforcement, AN reinforcement, US reinforcement, J reinforcement, delay in seconds
    MULTI_PLANES_FOR_FLAGS, //Set of flags, number, type of aircraft (0 - bombers, 1 - reconnaissance, 2 - transports, 3 - cargo, 4 - interceptors)
    MULTI_PHRASE_FOR_FLAGS, //A set of flags, a message
    MULTI_PLANES_MISSIONS_FOR_FLAGS, //Set of flags, number, type of aircraft    
    PHRASE_SHOW_AND_POINT_TO_MARKER, //phrase number, object number
    GROUP_GIVE_TO_PLAYER, //Group, player     
    GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_ZONE, //group number, zone number
    GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_OBJECT = 0x3C, //group number, object number
    SCRIPT_START = 0x464F4143,
    SCRIPT_END = 0x7fffffff,
} SCRIPTSCOMMANDS, * SCRIPTSCOMMANDSPTR;