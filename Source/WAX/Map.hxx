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

#define MAX_SCRIPTS_COUNT       1024

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
    MAPTYPE     Type;
    S32		    Width;
    S32		    Height;
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

typedef enum ScriptsCommand
{
    SCRIPTSCOMMAND_NONE                                                     = 0,
    SCRIPTSCOMMAND_AND                                                      = 1,
    SCRIPTSCOMMAND_OR                                                       = 2,

    // Unit type, group, comparison type, quantity
    SCRIPTSCOMMAND_IF_GROUP_UNIT_COUNT_OF_TYPE                              = 3,

    // Unit type, player, comparison type, quantity
    SCRIPTSCOMMAND_IF_PLAYER_UNIT_COUNT_OF_TYPE                             = 4,

    // Comparison type, quantity, group, zone
    SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_FOR_GROUP                             = 5,

    // Comparison type, quantity, player, zone
    SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_FOR_PLAYER                            = 6,
    SCRIPTSCOMMAND_RETURN_IF_FALSE                                          = 7,

    // Number, type of aircraft, player, zone, airfield
    SCRIPTSCOMMAND_PLANES_FOR_PLAYER_SEND_TO_ZONE_AND_LAND                  = 8,

    // Player, comparison type, quantity, aircraft type
    SCRIPTSCOMMAND_IF_PLANES_FOR_PLAYER_COUNT                               = 9,

    // Player, comparison type, quantity, aircraft type
    SCRIPTSCOMMAND_IF_PLANES_FOR_PLAYER_MISSIONS_COUNT                      = 10,
    SCRIPTSCOMMAND_SCRIPT_TURN_OFF                                          = 11,

    // Timer number, time in seconds
    SCRIPTSCOMMAND_TIMER_START                                              = 12,

    // Timer number
    SCRIPTSCOMMAND_TIMER_STOP                                               = 13,

    // Zone number
    SCRIPTSCOMMAND_ZONE_POINT_TO                                            = 14,

    // Phrase number
    SCRIPTSCOMMAND_PHRASE_SHOW                                              = 15,

    // Time in seconds
    SCRIPTSCOMMAND_COUNT_DOWN_START                                         = 16,

    // Parameters of the next mission
    SCRIPTSCOMMAND_CAMPAIGN_SET_NEXT_MISSION                                = 17,

    // Result: 0 - draw, 1 - win, 2 - loss
    SCRIPTSCOMMAND_MISSION_COMPLETE                                         = 18,

    // Group, behavior
    SCRIPTSCOMMAND_GROUP_BEHAVIOUR_SET                                      = 19,

    // Group, zone
    SCRIPTSCOMMAND_GROUP_BEHAVIOUR_ZONE_1SET                                = 20,

    // Group, zone
    SCRIPTSCOMMAND_GROUP_BEHAVIOUR_ZONE_2SET                                = 21,

    // Group, install group
    SCRIPTSCOMMAND_GROUP_BEHAVIOUR_GROUP_1SET                               = 22,

    // Group, install group
    SCRIPTSCOMMAND_GROUP_BEHAVIOUR_GROUP_2SET                               = 23,

    // Player, number, type of aircraft
    SCRIPTSCOMMAND_PLANES_ADD_TO_PLAYER                                     = 24,

    // Player, number, type of aircraft
    SCRIPTSCOMMAND_PLANES_MISSIONS_ADD_TO_PLAYER                            = 25,

    SCRIPTSCOMMAND_NEGATE                                                   = 26,

    // Timer number
    SCRIPTSCOMMAND_IF_TIMER_COMPLETE                                        = 27,

    // Comparison type (0 - strictly, 1 - more than, 2 - less than), time in seconds
    SCRIPTSCOMMAND_IF_COUNT_DOWN                                            = 28,
    
    // Comparison type (1 - more than, 2 - less than), time in seconds
    SCRIPTSCOMMAND_IF_TIME_FROM_MISSION_START                               = 29,

    // Comparison type, percentage, group, zone
    SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_PERCENT_FOR_GROUP                     = 30,

    // Comparison type, percentage, group, zone
    SCRIPTSCOMMAND_IF_ZONE_UNIT_COUNT_PERCENT_FOR_PLAYER                    = 31,

    // Unit type, group, comparison type, percentage, type in another group, another group
    SCRIPTSCOMMAND_IF_GROUP_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_GROUP    = 32,

    // Unit type, player, comparison type, percentage, other player's type, other player
    SCRIPTSCOMMAND_IF_PLAYER_UNIT_COUNT_PERCENT_OF_TYPE_RELATIVE_TO_PLAYER  = 33,

    // Group, behavior
    SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR                                       = 34,

    // Group, zone
    SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_ZONE1                                 = 35,

    // Group, zone
    SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_ZONE2                                 = 36,

    // Group, verification group
    SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_GROUP1                                = 37,

    // Group, verification group
    SCRIPTSCOMMAND_IF_GROUP_BEHAVIOUR_GROUP2                                = 38,

    // Group, flag, Zone, Delay, Health%, Ammo %, Experience%, Morale % {SSRW + fuel}
    SCRIPTSCOMMAND_GROUP_RESURRECT_THROUGH_FLAG_WITH_PARAMS                 = 39,

    // Player, reinforcements, Flag, Zone, delay in seconds
    SCRIPTSCOMMAND_REINFORCEMENT_SEND                                       = 40,

    // Object
    SCRIPTSCOMMAND_IF_OBJECT_IS_DESTROYED                                   = 41,

    // Number, type of aircraft, player, object, airfield
    SCRIPTSCOMMAND_PLANES_FOR_PLAYER_SEND_TO_OBJECT_AND_LAND                = 42,

    // No parameters
    SCRIPTSCOMMAND_PLANES_ROUTE_START                                       = 43,

    // Object
    SCRIPTSCOMMAND_PLANES_ROUTE_ADD_OBJECT                                  = 44,

    // Object
    SCRIPTSCOMMAND_PLANES_ROUTE_ADD_OBJECT_AS_DROP                          = 45,
    
    // Number, type of aircraft, player, airfield
    SCRIPTSCOMMAND_PLANES_ROUTE_SEND_AND_LAND_FOR_PLAYER                    = 46,

    // Cell reference, value, or value reference
    SCRIPTSCOMMAND_VARIABLE_SET                                             = 47,

    // Cell reference, operation (0 - increase, 1 - decrease), value, or value reference
    SCRIPTSCOMMAND_VARIABLE_MODIFY                                          = 48,

    // Group, type of comparison, time seconds
    SCRIPTSCOMMAND_IF_GROUP_WAS_ATTACKED_TIME_AGO                           = 49,

    // Cell, comparison type, value, or cell reference
    SCRIPTSCOMMAND_IF_VARIABLE_VALUE                                        = 50,
    SCRIPTSCOMMAND_IF_MISSION_START                                         = 51,

    // Reinforcement RU, reinforcement DE, reinforcement AN, reinforcement US, reinforcement J, delay in seconds
    SCRIPTSCOMMAND_MULTI_REINFORCEMENT_START                                = 52,
    
    // Set of flags, RU reinforcement DE, reinforcement AN, reinforcement US, reinforcement J, reinforcement, delay in seconds
    SCRIPTSCOMMAND_MULTI_REINFORCEMENT_FOR_FLAGS                            = 53,

    // Set of flags, number, type of aircraft (0 - bombers, 1 - reconnaissance, 2 - transports, 3 - cargo, 4 - interceptors)
    SCRIPTSCOMMAND_MULTI_PLANES_FOR_FLAGS                                   = 54,

    // A set of flags, a message
    SCRIPTSCOMMAND_MULTI_PHRASE_FOR_FLAGS                                   = 55,

    // Set of flags, number, type of aircraft
    SCRIPTSCOMMAND_MULTI_PLANES_MISSIONS_FOR_FLAGS                          = 56,

    // Phrase number, object number
    SCRIPTSCOMMAND_PHRASE_SHOW_AND_POINT_TO_MARKER                          = 57,

    // Group, player
    SCRIPTSCOMMAND_GROUP_GIVE_TO_PLAYER                                     = 58,

    // Group number, zone number
    SCRIPTSCOMMAND_GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_ZONE                  = 59,

    // Group number, object number
    SCRIPTSCOMMAND_GROUP_SHOT_ONE_ROCKET_ARTILLERY_TO_OBJECT                = 60,

    SCRIPTSCOMMAND_SCRIPT_START                                             = 0x464F4143,
    SCRIPTSCOMMAND_SCRIPT_END                                               = 0x7FFFFFFF,
} SCRIPTSCOMMAND, * SCRIPTSCOMMANDPTR;