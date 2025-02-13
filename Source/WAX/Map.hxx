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
    U32 Min_Players;
    U32 Max_Players;
    U32 Unk02; // TODO
    U32 Unk03; // TODO
} MAPMINMAX, * MAPMINMAXPTR;

typedef struct MapTypeSize
{
    U32 Type; // map type, summer = 0 winter = 1 beach = 2 desert = 4
    S32 Width;
    S32 Height;
} MAPTYPESIZE, * MAPTYPESIZEPTR;

typedef struct MapStruct2 // TODO
{
    MAPMINMAX   Actors;
    MAPTYPESIZE TypeAndSize;
} MAPSTRUCT2, * MAPSTRUCT2PTR;

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

typedef struct MisObjects // TODO
{
    MAPSTRUCT3 Unk00[MAX_MAP_STRUCT3_COUNT]; // TODO;
    MAPSTRUCT4 Unk01[MAX_MAP_STRUCT4_COUNT]; // TODO;
} MIS_OBJECTS, * MIS_OBJECTS_PTR;

typedef struct MisScripts// TODO
{
	S16 Unk00; // TODO;
} MIS_SCRIPTS, * MIS_SCRIPTS_PTR;

typedef struct Map
{
    MAPHEADER       Map_Header1;
    MAPSTRUCT2      Map_Header2;
    MIS_OBJECTS     Mis_objects; // TODO
    LPSTR           Mis_Desc;
    PIXEL*          Pixels;
    LPSTR           MissionDescription;
	MIS_SCRIPTS Mis_Scripts; // TODO
} MAP, * MAPPTR;

enum SCRIPTS_NUM
{
	None = 0x00,
	And,
	Or,
	IfGroupUnitCountOfType, //unit type, group, comparison type, quantity
	IfPlayerUnitCountOfType, //unit type, player, comparison type, quantity
	IfZoneUnitCountForGroup, //comparison type, quantity, group, zone
	IfZoneUnitCountForPlayer, //comparison type, quantity, player, zone
	ReturnIfFalse,
	PlanesForPlayerSendToZoneAndLand, //Number, type of aircraft, player, zone, airfield
	IfPlanesForPlayerCount, //Player, comparison type, quantity, aircraft type
	IfPlanesForPlayerMissionsCount, //Player, comparison type, quantity, aircraft type
	ScriptTurnOff,
	TimerStart, //timer number, time in seconds
	TimerStop, //timer number
	ZonePointTo, //zone number
	PhraseShow, //phrase number
	CountDownStart, //time in seconds
	CampaignSetNextMission, //parameters of the next mission
	MissionComplete, //result (0 - draw, 1 - win, 2- loss)
	GroupBehaviourSet, //Group, behavior
	GroupBehaviourZone1Set, //Group, zone
	GroupBehaviourZone2Set, //Group, zone
	GroupBehaviourGroup1Set, //Group, install group
	GroupBehaviourGroup2Set, //Group, install group
	PlanesAddToPlayer, //Player, number, type of aircraft
	PlanesMissionsAddToPlayer, //Player, number, type of aircraft
	Negate,
	IfTimerComplete, //timer number
	IfCountDown, //comparison type (0 - strictly, 1 - more than, 2 - less than), time in seconds
	IfTimeFromMissionStart, //comparison type (1 - more than, 2 - less than), time in seconds
	IfZoneUnitCountPercentForGroup, //comparison type, percentage, group, zone
	IfZoneUnitCountPercentForPlayer, //comparison type, percentage, group, zone
	IfGroupUnitCountPercentOfTypeRelativeToGroup, //Unit type, group, comparison type, percentage, type in another group, another group
	IfPlayerUnitCountPercentOfTypeRelativeToPlayer, //Unit type, player, comparison type, percentage, other player's type, other player
	IfGroupBehaviour, //Group, behavior
	IfGroupBehaviourZone1, //Group, zone
	IfGroupBehaviourZone2, //Group, zone
	IfGroupBehaviourGroup1, //Group, verification group
	IfGroupBehaviourGroup2, //Group, verification group
	GroupResurrectThroughFlagWithParams, //Group, flag, Zone, Delay, Health%, Ammo %, Experience%, Morale % {SSRW + fuel}
	ReinforcementSend, //Player, reinforcements, Flag, Zone, delay in seconds
	IfObjectIsDestroyed, //Object
	PlanesForPlayerSendToObjectAndLand, //Number, type of aircraft, player, object, airfield
	PlanesRouteStart, //No parameters
	PlanesRouteAddObject, //Object
	PlanesRouteAddObjectAsDrop, //Object
	PlanesRouteSendAndLandForPlayer, //Number, type of aircraft, player, airfield
	VariableSet, //Cell reference, value, or value reference
	VariableModify, //Cell reference, operation (0 - increase, 1 - decrease), value, or value reference
	IfGroupWasAttackedTimeAgo, //Group, type of comparison, time seconds
	IfVariableValue, //Cell, comparison type, value, or cell reference
	IfMissionStart,
	MultiReinforcementStart, //Reinforcement ru, reinforcement de, reinforcement an, reinforcement us, reinforcement J, delay in seconds
	MultiReinforcementForFlags, //Set of flags, ru reinforcement, de reinforcement, AN reinforcement, US reinforcement, J reinforcement, delay in seconds
	MultiPlanesForFlags, //Set of flags, number, type of aircraft (0 - bombers, 1 - reconnaissance, 2 - transports, 3 - cargo, 4 - interceptors)
	MultiPhraseForFlags, //A set of flags, a message
	MultiPlanesMissionsForFlags, //Set of flags, number, type of aircraft    
	PhraseShowAndPointToMarker, //phrase number, object number
	GroupGiveToPlayer, //Group, player     
	GroupShotOneRocketArtilleryToZone, //group number, zone number
	GroupShotOneRocketArtilleryToObject = 0x3C, //group number, object number
	ScriptStart = 0x464F4143,
	ScriptEND = 0x7fffffff,
};