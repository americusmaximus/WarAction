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

#include "Objects.hxx"

#include <DirectPlay.hxx>
#include <Player.hxx>

// TODO restructure this whole file, so it is pretty and makes sense

#define INVALID_NETWORK_PLAYER_INDEX                (-1)

#define MAX_NETWORK_STATE_PLAYER_COUNT              16 /* TODO */
#define MAX_NETWORK_STATE_MESSAGE_CONTENT_LENGTH    32768

#define MAX_NETWORK_STATE_PIRECT_PLAY_PLAYER_COUT   22 /* TODO */

//#define NETWORK_MESSAGE_ID_INDEX                  0
//#define NETWORK_MESSAGE_PLAYER_ID_INDEX           1
//#define NETWORK_MESSAGE_PLAYER_ID_INDEX_RELATIVE  0
//#define NETWORK_MESSAGE_UNK_2_INDEX               2 /* TODO */
//#define NETWORK_MESSAGE_UNK_3_INDEX               3 /* TODO */
//#define NETWORK_MESSAGE_UNK_4_INDEX               4 /* TODO */

//#define NETWORK_MESSAGE_PLAYER_NAME_INDEX         8

#define NETWORKMESSAGE_CONNECT_PLAYER               0
#define NETWORKMESSAGE_UPDATE_PLAYER                1
#define NETWORKMESSAGE_UPDATE_PLAYER_STATE          2
#define NETWORKMESSAGE_UPDATE_PLAYER_DATA           3
#define NETWORKMESSAGE_DISCONECT_PLAYER             4 /* TODO */
#define NETWORKMESSAGE_PLAYER_STARTING              5
#define NETWORK_MESSAGE_ACKNOWLEDGE_GAME_STATE      6
#define NETWORK_MESSAGE_ACKNOWLEDGE_MAP             7

#define NETWORK_MESSAGE_ACKNOWLEDGE_STARTUP         19

#define NETWORK_MESSAGE_DELETE_PLAYER               36

#define NETWORK_MESSAGE_0x31                        0x31 /* TODO */

#define NETWORK_MESSAGE_VERSION                     0x10015

#define MAX_SESSION_NAME_LENGTH                     256

#define MAX_IP_ADDRESS_STRING_LENGTH                256

typedef struct UnkNetworkID // TODO
{
    GUID    ID;
    U32     Unk1; // TODO
} UNKNETWORKID, * UNKNETWORKIDPTR; // TODO

// 0x1004860d
EXTERN UNKNETWORKID UnkNetworkID_1004860d; // TODO move

typedef struct NetworkMessageContent
{
    // TODO
} NETWORKMESSAGECONTENT, * NETWORKMESSAGECONTENTPTR; // TODO DO we need this ?

typedef struct NetworkMessage // TODO DO we need this ?
{
    U32                     Type;
    NETWORKMESSAGECONTENT   Content;
} NETWORKMESSAGE, * NETWORKMESSAGEPTR;

typedef struct NetworkMessageConnect
{
    U8      Index;
    U32     Unk01; // TODO
    U8      Unk02; // TODO
    U8      Unk03; // TODO
    U8      Unk04; // TODO
    U8      Unk05; // TODO
    U8      Length;
    CHAR    Name[1];
    U32     Version;
} NETWORKMESSAGECONNECT, * NETWORKMESSAGECONNECTPTR;

LPDIRECTPLAY3A AcquireNetworkInstance(VOID);

LPCSTR AcquireDirectPlayErrorDescription(CONST HRESULT code);

struct NetworkStateModuleContainer;

typedef NetworkStateModuleContainer* (CLASSCALL* NETWORKSTATEMODULECONTAINERRELEASEACTION)(NetworkStateModuleContainer* self, CONST OBJECTRELEASETYPE mode);

typedef struct NetworkModuleStateContainerSelf
{
    NETWORKSTATEMODULECONTAINERRELEASEACTION Release;
} NETWORKMODULESTATECONTAINERSELF, * NETWORKMODULESTATECONTAINERSELFPTR;

EXTERN NETWORKMODULESTATECONTAINERSELF NetworkStateSelfState;

typedef struct NetworkSession
{
    GUID            ID;
    U32             MaxPlayerCount;
    U32             CurrentPlayerCount;
    CHAR            Name[200]; // TODO
    GUID            User; // TODO better name
} NETWORKSESSION, * NETWORKSESSIONPTR;

typedef struct NetworkConnection
{
    GUID    ID;
    LPSTR   Name;
    LPVOID  Connection; // TODO type
} NETWORKCONNECTION, * NETWORKCONNECTIONPTR;

// a.k.a. CAConnect
typedef struct NetworkStateModuleContainer
{
    NETWORKMODULESTATECONTAINERSELFPTR  Self;
    LPDIRECTPLAY3A                      Instance;
    LPDIRECTPLAYLOBBY3A                 Lobby;
    struct
    {
        NETWORKCONNECTIONPTR            Items;
        U32                             Count;
    } Connections;
    struct
    {
        U32                             Count;
        NETWORKSESSIONPTR               Items;
    } Sessions;
    HRESULT                             Status;
    LPVOID Allocatedx20; // TODO
    LPDPLCONNECTION                     Connection;

    U32 Unk[94]; // TODO

    U32 Unk1111; // TODO // TODO enum
    LPDPSESSIONDESC2                    Descriptor;
    DPID ClientID;
    BOOL IsActive;
} NETWORKSTATEMODULECONTAINER, * NETWORKSTATEMODULECONTAINERPTR;

typedef struct NetworkPlayer
{
    DWORD   ID;
    U32 Unk01; // TODO
    U32 Unk02; // TODO
    U32     Ticks;
    U32     JoinTicks;
    U32 Unk05; // TODO
    U32 Unk06; // TODO
    U32     Ping;
} NETWORKPLAYER, * NETWORKPLAYERPTR;

typedef struct NetworkPlayerState
{
    U32 Unk00; // TODO

    // TODO

    S32 Unk88; // TODO name, type
    S32 Unk8C; // TODO name, type
    U32 Unk90; //  TODO
    S32 Unk94; // TODO name, type
} NETWORKPLAYERSTATE, * NETWORKPLAYERSTATEPTR;

typedef struct NetworkPlayerDescriptor
{
    CHAR    Name[MAX_PLAYER_NAME_LENGTH];
    U8      Unk111; // TODO
    U8      Unk112; // TODO
    U8      Unk113; // TODO
    U8      Unk114; // TODO
    U8      Unk115; // TODO
} NETWORKPLAYERDESCRIPTOR, * NETWORKPLAYERDESCRIPTORPTR;

typedef struct MultiPlayerSettings // TODO name
{
    U32 Unknown; // TODO
    U32 MultiSpeed; // TODO
    U32 TimerOptions; // TODO
    U32 FlagActivationTime; // TODO
    U32 DisablementOnStart; // TODO
    U32 NoFlagCountDown; // TODO
    U32 NoUnitsCountDown; // TODO
} MULTIPLAYERSETTINGS, * MULTIPLAYERSETTINGSPTR; // TODO name

typedef enum NetworkClientState
{
    NETWORKCLIENTSTATE_NONE         = 0,
    NETWORKCLIENTSTATE_UNKNOWN1     = 1,
    // TODO
    NETWORKCLIENTSTATE_DISCONNECTED = 3,
    NETWORKCLIENTSTATE_FORCE_DWORD  = 0x7FFFFFF
} NETWORKCLIENTSTATE, * NETWORKCLIENTSTATEPTR;

typedef struct NetworkContainer
{
    NETWORKSTATEMODULECONTAINER State;                                          // 0x10058e28

    struct
    {
        CHAR                    Name[MAX_PLAYER_NAME_LENGTH];                   // 0x1004832c

        DPID                    ID;         // 0x1004884a
        DPID                    Host;       // 0x100485d1
        U32                     Ticks;      // 0x100485dd // TODO NAME

        U32                     JoinTicks;  // 0x10058e1c // TODO NAME

        struct
        {
            U8                  Content[MAX_NETWORK_STATE_MESSAGE_CONTENT_LENGTH];  // 0x10048bf0
            U32                 Length;                                             // 0x10058bf8
        } Message;

        NETWORKCLIENTSTATE State;                                                   // 0x100480d8
    } Client;

    struct
    {
        NETWORKPLAYER       Players[MAX_NETWORK_STATE_PLAYER_COUNT];                // 0x10047428
        NETWORKPLAYERSTATE  PlayerStates[MAX_NETWORK_STATE_PLAYER_COUNT];           // 0x10047628 // Better name

        struct
        {
            U8 Content[MAX_NETWORK_STATE_MESSAGE_CONTENT_LENGTH];                   // 0x10050bf8
            U32 Length;                                                             // 0x10058c00
        } Message;
    } Server;

    NETWORKPLAYERDESCRIPTOR Players[MAX_NETWORK_STATE_PLAYER_COUNT];                // 0x100471d8

    struct
    {
        BOOL IsHost;                                                                // 0x10048b8c
        BOOL IsClient;                                                              // 0x10048b90
    } Connection;
} NETWORKCONTAINER, * NETWORKCONTAINERPTR;

EXTERN NETWORKCONTAINER NetworkState;

// 0x10047fa8
EXTERN MULTIPLAYERSETTINGS MultiplayerSettingsState; // TODO Move to networkstate

// 0x10048b94
EXTERN GUID GUID_10048b94; // TODO

// TODO better names

// 0x1003fba4
EXTERN U32 NetworkType; // TODO Better Name

NETWORKSTATEMODULECONTAINERPTR CLASSCALL ActivateNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self);
NETWORKSTATEMODULECONTAINERPTR CLASSCALL ReleaseNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self, CONST OBJECTRELEASETYPE mode);
BOOL CLASSCALL MustCreateNetworkModuleStateSession(NETWORKSTATEMODULECONTAINERPTR self);
S32 CLASSCALL InitializeNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self);
VOID CLASSCALL DisposeNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self);

VOID CloseNetworkModuleStateConnection(VOID);
VOID CLASSCALL CloseNetworkModuleStateConnection(NETWORKSTATEMODULECONTAINERPTR state);

VOID ReleaseNetworkStateInstance(VOID);
VOID ReleaseNetworkStateContainer(VOID);

VOID DisconnectClientNetwork(VOID);
VOID DisconnectHostNetwork(VOID);

VOID SendNetworkMessage(CONST U32 indx);
VOID DisconnectNetworkPlayer(CONST U32 indx);
VOID SendNetworkClientMessage(VOID);
VOID AppendNetworkClientMessage(CONST U8 message);
VOID AppendNetworkClientMessageValue(CONST U32 message);
VOID SendNetworkClientJoinRequest(VOID);

BOOL InitializeNetworkClient(VOID);

BOOL CLASSCALL InitializeNetworkModuleStateClient(NETWORKSTATEMODULECONTAINERPTR self, LPCSTR name, LPVOID content, CONST U32 length);
VOID HandleNetworkHostMessage(CONST DPID id, CONST NETWORKMESSAGEPTR message, CONST U32 size);
VOID ConnectNetworkClient(CONST DPID id, CONST NETWORKMESSAGECONNECTPTR message, CONST U32 size);

VOID ExecuteNetwork(VOID);

BOOL IsDirectPlayError(CONST HRESULT value);

BOOL InitializeNetworkServer(VOID);

VOID SelectNetworkInstance(LPDIRECTPLAY3A value);
BOOL CLASSCALL OpenNetworkStateModuleContainer(NETWORKSTATEMODULECONTAINERPTR self, LPSTR name, CONST U32 players, CONST GUID guid);
BOOL CLASSCALL ConnectNetworkStateModuleContainer(NETWORKSTATEMODULECONTAINERPTR self, CONST GUID guid);
BOOL CLASSCALL JoinNetworkStateModuleContainer(NETWORKSTATEMODULECONTAINERPTR self, CONST U32 indx);
BOOL CLASSCALL EnumNetworkStateModuleContainerSessions(NETWORKSTATEMODULECONTAINERPTR self);

BOOL PASCAL EnumNetworkStateModuleContainerSessionsCallback(LPCDPSESSIONDESC2 lpThisSD, LPDWORD lpdwTimeOut, DWORD dwFlags, LPVOID lpContext);
VOID CLASSCALL AppendNetworkStateModuleContainerSession(NETWORKSTATEMODULECONTAINERPTR self, LPCDPSESSIONDESC2 desc);
VOID StopSendingPlayers(CONST DPID id, LPCSTR message); // TODO name
VOID FUN_1001af30(CONST U32 indx);
VOID AppendIntegerNetwordServerMessage(CONST S32 value); // TODO name, type, U32?
VOID FUN_1001d710(S32 param_1, S32 param_2, S32 param_3, LPCSTR param_4); // TODO

VOID CLASSCALL DisposeNetworkState(NETWORKSTATEMODULECONTAINERPTR self);
VOID CLASSCALL ReleaseNetworkState(NETWORKSTATEMODULECONTAINERPTR self);

U32 CLASSCALL AcquireGameSessions(NETWORKSTATEMODULECONTAINERPTR self, NETWORKSESSIONPTR* sessions, U32* count); // TODO name, type
BOOL PASCAL EnumNetwordConnectionsCallback(LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext); // TODO name
VOID CLASSCALL FUN_1001f390(NETWORKSTATEMODULECONTAINERPTR self, LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize, LPCDPNAME lpName); // TODO name

VOID SendPlayerMap(CONST U32 indx); // TODO name
VOID SendPlayerMultiplayerSettings(CONST U32 indx); // TODO name
VOID FUN_1001ade0(LPCVOID content, CONST U32 size); // TODO name

VOID FUN_1001be90(CONST U32 indx); // TODO name
VOID FUN_1001c710(VOID); // TODO name

BOOL FUN_10018570(LPCSTR path, UNKNETWORKIDPTR value); // TODO name
BOOL InitializeNetwork(VOID); // TODO name
BOOL CLASSCALL InitializeNetworkState(NETWORKSTATEMODULECONTAINERPTR self);
BOOL CLASSCALL NetworkModuleStateContainerUnknown1(NETWORKSTATEMODULECONTAINERPTR self, LPCSTR name);
BOOL CLASSCALL NetworkModuleStateContainerUnknown2(NETWORKSTATEMODULECONTAINERPTR self, LPCSTR name);
BOOL CLASSCALL FUN_1001e950(NETWORKSTATEMODULECONTAINERPTR self); // TODO name
BOOL CLASSCALL FUN_1001e260(NETWORKSTATEMODULECONTAINERPTR self); // TODO name

S32 AcquireNetworkPlayerIndex(CONST DPID id); // TODO
BOOL FUN_1001b090(CONST DPID id, CONST BOOL ban); // TODO
VOID HandleNetworkClientMessage(CONST DPID id, CONST NETWORKMESSAGEPTR message, CONST U32 size);
VOID FUN_1001c370(VOID); // TODO
VOID FUN_1001d050(S32 param_1, S32 param_2); // TODO
BOOL CLASSCALL SelectNetworkStateModuleContainerDesc(NETWORKSTATEMODULECONTAINERPTR self, LPDPSESSIONDESC2 desc);
VOID AppendByteNetwordServerMessage(CONST U8 value); // TODO name, type