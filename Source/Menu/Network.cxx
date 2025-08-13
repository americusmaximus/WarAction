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

#include "Assets.hxx"
#include "Logger.hxx"
#include "MenuSave.hxx"
#include "Messages.hxx"
#include "Network.hxx"
#include "State.hxx"
#include "Unknown.hxx"

#include <stdlib.h>

// 0x1003a754
NETWORKMODULESTATECONTAINERSELF NetworkStateSelfState =
{
    ReleaseNetworkModuleState
};

NETWORKCONTAINER NetworkState;

MULTIPLAYERSETTINGS MultiplayerSettingsState; // TODO Move to networkstate

GUID GUID_10048b94; // TODO

U32 NetworkType = 3; // TODO

STATIC CONST GUID NetworkApplication = { 0xA3CE7402, 0xEB37, 0x11D1, { 0x93, 0x3B, 0x00, 0x40, 0xC7, 0x95, 0x1C, 0xE6 } };

// TODO restructure this whole file, so it is pretty and makes sense

// 0x1001e0f0
LPDIRECTPLAY3A AcquireNetworkInstance(VOID)
{
    return State.App->Module->Network;
}

// 0x1001e060
NETWORKSTATEMODULECONTAINERPTR CLASSCALL ActivateNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self)
{
    self->Self = &NetworkStateSelfState;

    self->Connections.Count = 0;
    self->Connections.Items = NULL;

    self->Sessions.Count = 0;
    self->Sessions.Items = NULL;

    self->Allocatedx20 = NULL;
    self->Instance = NULL;
    self->Lobby = NULL;
    self->Connection = NULL;
    self->Unk1111 = 0xffffffff; // TODO
    self->IsActive = FALSE;
    self->Descriptor = NULL;

    strcpy(State.Name, "<<>>");

    return self;
}

// 0x1001e0c0
NETWORKSTATEMODULECONTAINERPTR CLASSCALL ReleaseNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeNetworkModuleState(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x1001e0e0
VOID CLASSCALL DisposeNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self)
{
    self->Self = &NetworkStateSelfState;
}

// 0x1001e4a0
// a.k.a. CAConnect::isLobbied
S32 CLASSCALL InitializeNetworkModuleState(NETWORKSTATEMODULECONTAINERPTR self) // TODO return type
{
    LogMessage("CAConnect::isLobbied(%d)\n", self->Unk1111);

    if (self->Unk1111 != -1) { return self->Unk1111; } // TODO

    self->Unk1111 = 0; // TODO
    self->Connection = NULL;

    LogMessage("CAConnect::isLobbied() : Creating Lobby3A\n");

    self->Status = CoCreateInstance(CLSID_DirectPlayLobby, NULL,
        CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3, (LPVOID*)&self->Lobby);

    if (!IsDirectPlayError(self->Status))
    {
        DWORD size = 0;
        self->Status = self->Lobby->GetConnectionSettings(0, NULL, &size);

        LogMessage("CAConnect::isLobbied() : GetConnectionSettings(%s)\n",
            AcquireDirectPlayErrorDescription(self->Status));

        if (self->Status != DPERR_NOTLOBBIED
            && (self->Status == DPERR_BUFFERTOOSMALL || !IsDirectPlayError(self->Status)))
        {
            self->Connection = (LPDPLCONNECTION)malloc(size);
            self->Status = self->Lobby->GetConnectionSettings(0, &self->Connection, &size);

            if (!IsDirectPlayError(self->Status))
            {
                strcpy(State.Name, self->Connection->lpPlayerName->lpszShortNameA);

                LogMessage("CAConnect::isLobbied() : Ok, PLAYER=%s\n", State.Name);

                self->Unk1111 = 1; // TODO

                return 1; // TODO
            }
        }
    }

    LogMessage("CAConnect::isLobbied() : FAILED\n");

    DIRECTPLAYRELEASE(self->Lobby);

    if (self->Connection != NULL)
    {
        free(self->Connection);
        self->Connection = NULL;
    }

    return 0; // TODO
}

// 0x1001da80
// a.k.a. DPLAY_Done
VOID ReleaseNetworkStateContainer(VOID)
{
    CloseNetworkModuleStateConnection();
    CloseNetworkModuleStateConnection(&NetworkState.State);

    if (State.App->Module->Network != NULL)
    {
        LogMessage("Connect.Done()\n");

        State.App->Module->Network = NULL;

        LogTime();
        LogMessage("DP released.\n");
    }
}

// 0x1001da10
VOID CloseNetworkModuleStateConnection(VOID)
{
    if (State.App->Module->Network == NULL) { return; }

    if (NetworkState.Connection.IsHost)
    {
        LogMessage("HOST_Disconnect()\n");

        DisconnectHostNetwork();

        NetworkState.Connection.IsHost = FALSE;
    }

    if (NetworkState.Connection.IsClient)
    {
        LogMessage("CLIENT_Disconnect()\n");

        DisconnectClientNetwork();

        NetworkState.Connection.IsClient = FALSE;
        NetworkState.Client.State = NETWORKCLIENTSTATE_NONE;
    }
}

// 0x1001e2b0
VOID CLASSCALL CloseNetworkModuleStateConnection(NETWORKSTATEMODULECONTAINERPTR state)
{
    {
        LPDIRECTPLAY3A instance = AcquireNetworkInstance();

        if (instance != NULL)
        {
            LogMessage("get_DP()->Close()\n");

            instance->Close();
            
            LogMessage("get_DP()->Release()\n");

            LogTime();

            LogMessage("DPLAY: DP released, ref=%d\n", instance->Release());
        }
    }

    if (state->Instance != NULL)
    {
        LogMessage("lpDPL->Release()\n");

        LogTime();
        LogMessage("DPLAY: DPL released, ref=%d\n", state->Instance->Release());

        state->Instance = NULL;
    }

    if (state->Lobby != NULL)
    {
        LogMessage("lpLobby3->Release()\n");

        LogTime();
        LogMessage("DPLAY: lpLobby3 released, ref=%d\n", state->Lobby->Release());

        state->Lobby = NULL;
    }

    if (state->Connection != NULL)
    {
        free(state->Connection);

        state->Connection = NULL;

        LogTime();
        LogMessage("DPLAY: lpLobbyConnection released\n");
    }

    state->IsActive = FALSE;
}

// 0x1001dc60
VOID ReleaseNetworkStateInstance(VOID)
{
    if (NetworkState.State.Instance == NULL) { return; }

    DIRECTPLAYRELEASE(NetworkState.State.Instance);

    LogTime();
    LogMessage("DPLAY: DPL released\n");
}

// 0x1001c220
VOID DisconnectHostNetwork(VOID)
{
    NetworkState.Server.Message.Length = 1;
    NetworkState.Server.Message.Content[0 /* NETWORK_MESSAGE_ID_INDEX */ ] = NETWORK_MESSAGE_DELETE_PLAYER; // TODO

    for (U32 x = 0; x < MAX_NETWORK_STATE_PLAYER_COUNT; x++)
    {
        if (NetworkState.Server.Players[x].ID != DPID_SYSMSG)
        {
            LogMessage("HOST_SendSBuf(%i)\n", x);
            SendNetworkMessage(x);

            LogMessage("HOST_DeletePlayer(%i)\n", x);
            DisconnectNetworkPlayer(x);
        }
    }
}

// 0x1001ae10
// a.k.a. HOST_SendSBuf
VOID SendNetworkMessage(CONST U32 indx)
{
    if (NetworkState.Server.Message.Length != 0)
    {
        CONST HRESULT result = State.App->Module->Network->Send(DPID_SERVERPLAYER,
            NetworkState.Server.Players[indx].ID, DPSEND_NONE,
            NetworkState.Server.Message.Content, NetworkState.Server.Message.Length);

        if (result != DPERR_INVALIDPARAM && result != DPERR_INVALIDPLAYER)
        {
            NetworkState.Server.Players[indx].Ticks = GetTickCount();
            
            return;
        }

        DisconnectNetworkPlayer(indx);

        LogTime();
        LogMessage("SERVER: Player %i is droped out.\n", indx);
    }
}

// 0x1001b010
// a.k.a. HOST_DeletePlayer
VOID DisconnectNetworkPlayer(CONST U32 indx)
{
    if (NetworkState.Server.Players[indx].ID != DPID_SYSMSG)
    {
        LogTime();
        LogMessage("SERVER: Delete player %i.\n", indx);

        State.App->Module->Network->DestroyPlayer(NetworkState.Server.Players[indx].ID);

        NetworkState.Server.Players[indx].ID = DPID_SYSMSG;
        NetworkState.Server.Players[indx].Unk01 = 0; // TODO

        NetworkState.Server.PlayerStates[indx].Unk00 = NetworkState.Server.PlayerStates[indx].Unk00 + 1; // TODO
    }
}

// 0x1001d4c0
// a.k.a. CLIENT_Disconnect
VOID DisconnectClientNetwork(VOID)
{
    NetworkState.Client.Message.Length = 0;

    AppendNetworkClientMessage(NETWORKMESSAGE_DISCONECT_PLAYER);

    LogTime();
    LogMessage("CLIENT: Sending disconect message.\n");
    SendNetworkClientMessage();

    LogMessage("Connect.get_DP()->DestroyPlayer(%X)\n", NetworkState.Client.ID);

    CONST HRESULT result = AcquireNetworkInstance()->DestroyPlayer(NetworkState.Client.ID);

    LogTime();
    LogMessage("DPLAY: DestroyPlayer -> %x\n", result);
}

// 0x1001c670
VOID SendNetworkClientMessage(VOID)
{
    if (NetworkState.Client.Message.Length != 0)
    {
        CONST HRESULT result = State.App->Module->Network->Send(NetworkState.Client.ID,
            NetworkState.Client.Host, DPSEND_NONE,
            NetworkState.Client.Message.Content, NetworkState.Client.Message.Length);

        NetworkState.Client.Ticks = GetTickCount();

        if (result == DPERR_INVALIDPARAM || result == DPERR_INVALIDPLAYER)
        {
            NetworkState.Client.State = NETWORKCLIENTSTATE_DISCONNECTED;
        }
    }
}

// 0x1001c630
VOID AppendNetworkClientMessage(CONST U8 message)
{
    NetworkState.Client.Message.Content[NetworkState.Client.Message.Length] = message;
    NetworkState.Client.Message.Length = NetworkState.Client.Message.Length + sizeof(U8);
}

// 0x1001d940
BOOL InitializeNetworkClient(VOID)
{
    if (!InitializeNetworkModuleStateClient(&NetworkState.State, State.Name, NULL, 0)) { return FALSE; }

    NetworkState.Client.ID = NetworkState.State.ClientID;

    FUN_1001c710();

    NetworkState.Connection.IsClient = TRUE;
    
    LogTime();
    LogMessage("Client started, ID=%X\n", NetworkState.Client.ID);

    SendNetworkClientJoinRequest();

    return TRUE;
}

// 0x1001f1e0
BOOL CLASSCALL InitializeNetworkModuleStateClient(NETWORKSTATEMODULECONTAINERPTR self, LPCSTR name, LPVOID content, CONST U32 length)
{
    LPDIRECTPLAY3A instance = AcquireNetworkInstance();

    DPNAME value;
    ZeroMemory(&value, sizeof(DPNAME));

    value.dwSize = sizeof(DPNAME);
    value.lpszShortNameA = (LPSTR)name;

    self->Status = instance->CreatePlayer(&self->ClientID, &value, NULL, content, length, DPPLAYER_CLIENT);

    LogTime();
    LogMessage("DPLAY: CreatePlayer(%x) -> %x (%s)\n",
        self->ClientID, self->Status, AcquireDirectPlayErrorDescription(self->Status));

    if (self->Status != DP_OK) { return FALSE; }

    if (self->Descriptor != NULL) { free(self->Descriptor); }

    DWORD size = 0;
    self->Status = instance->GetSessionDesc(self->Descriptor, &size);

    if (self->Status != DPERR_BUFFERTOOSMALL) { return FALSE; }

    self->Descriptor = (LPDPSESSIONDESC2)malloc(size);

    self->Status = instance->GetSessionDesc(self->Descriptor, &size);

    return self->Status == DP_OK;
}

// 0x1001dcf0
LPCSTR AcquireDirectPlayErrorDescription(CONST HRESULT code)
{
    switch (code)
    {
    case DP_OK: { return "DP_OK"; }
    case DPERR_ALREADYINITIALIZED: { return "DPERR_ALREADYINITIALIZED"; }
    case DPERR_ACCESSDENIED: { return "DPERR_ACCESSDENIED"; }
    case DPERR_ACTIVEPLAYERS: { return "DPERR_ACTIVEPLAYERS"; }
    case DPERR_BUFFERTOOSMALL: { return "DPERR_BUFFERTOOSMALL"; }
    case DPERR_CANTADDPLAYER: { return "DPERR_CANTADDPLAYER"; }
    case DPERR_CANTCREATEGROUP: { return "DPERR_CANTCREATEGROUP"; }
    case DPERR_CANTCREATEPLAYER: { return "DPERR_CANTCREATEPLAYER"; }
    case DPERR_CANTCREATESESSION: { return "DPERR_CANTCREATESESSION"; }
    case DPERR_CAPSNOTAVAILABLEYET: { return "DPERR_CAPSNOTAVAILABLEYET"; }
    case DPERR_EXCEPTION: { return "DPERR_EXCEPTION"; }
    case DPERR_GENERIC: { return "DPERR_GENERIC"; }
    case DPERR_INVALIDFLAGS: { return "DPERR_INVALIDFLAGS"; }
    case DPERR_INVALIDOBJECT: { return "DPERR_INVALIDOBJECT"; }
    case DPERR_INVALIDPARAM: { return "DPERR_INVALIDPARAM"; }
    case DPERR_INVALIDPLAYER: { return "DPERR_INVALIDPLAYER"; }
    case DPERR_INVALIDGROUP: { return "DPERR_INVALIDGROUP"; }
    case DPERR_NOCAPS: { return "DPERR_NOCAPS"; }
    case DPERR_NOCONNECTION: { return "DPERR_NOCONNECTION"; }
    case DPERR_NOMEMORY: { return "DPERR_NOMEMORY"; }
    case DPERR_NOMESSAGES: { return "DPERR_NOMESSAGES"; }
    case DPERR_NONAMESERVERFOUND: { return "DPERR_NONAMESERVERFOUND"; }
    case DPERR_NOPLAYERS: { return "DPERR_NOPLAYERS"; }
    case DPERR_NOSESSIONS: { return "DPERR_NOSESSIONS"; }
    case DPERR_PENDING: { return "DPERR_PENDING"; }
    case DPERR_SENDTOOBIG: { return "DPERR_SENDTOOBIG"; }
    case DPERR_TIMEOUT: { return "DPERR_TIMEOUT"; }
    case DPERR_UNAVAILABLE: { return "DPERR_UNAVAILABLE"; }
    case DPERR_UNSUPPORTED: { return "DPERR_UNSUPPORTED"; }
    case DPERR_BUSY: { return "DPERR_BUSY"; }
    case DPERR_USERCANCEL: { return "DPERR_USERCANCEL"; }
    case DPERR_NOINTERFACE: { return "DPERR_NOINTERFACE"; }
    case DPERR_CANNOTCREATESERVER: { return "DPERR_CANNOTCREATESERVER"; }
    case DPERR_PLAYERLOST: { return "DPERR_PLAYERLOST"; }
    case DPERR_SESSIONLOST: { return "DPERR_SESSIONLOST"; }
    case DPERR_UNINITIALIZED: { return "DPERR_UNINITIALIZED"; }
    case DPERR_NONEWPLAYERS: { return "DPERR_NONEWPLAYERS"; }
    case DPERR_INVALIDPASSWORD: { return "DPERR_INVALIDPASSWORD"; }
    case DPERR_CONNECTING: { return "DPERR_CONNECTING"; }
    case DPERR_BUFFERTOOLARGE: { return "DPERR_BUFFERTOOLARGE"; }
    case DPERR_CANTCREATEPROCESS: { return "DPERR_CANTCREATEPROCESS"; }
    case DPERR_APPNOTSTARTED: { return "DPERR_APPNOTSTARTED"; }
    case DPERR_INVALIDINTERFACE: { return "DPERR_INVALIDINTERFACE"; }
    case DPERR_NOSERVICEPROVIDER: { return "DPERR_NOSERVICEPROVIDER"; }
    case DPERR_UNKNOWNAPPLICATION: { return "DPERR_UNKNOWNAPPLICATION"; }
    case DPERR_NOTLOBBIED: { return "DPERR_NOTLOBBIED"; }
    case DPERR_SERVICEPROVIDERLOADED: { return "DPERR_SERVICEPROVIDERLOADED"; }
    case DPERR_ALREADYREGISTERED: { return "DPERR_ALREADYREGISTERED"; }
    case DPERR_NOTREGISTERED: { return "DPERR_NOTREGISTERED"; }
    case DPERR_AUTHENTICATIONFAILED: { return "DPERR_AUTHENTICATIONFAILED"; }
    case DPERR_CANTLOADSSPI: { return "DPERR_CANTLOADSSPI"; }
    case DPERR_ENCRYPTIONFAILED: { return "DPERR_ENCRYPTIONFAILED"; }
    case DPERR_SIGNFAILED: { return "DPERR_SIGNFAILED"; }
    case DPERR_CANTLOADSECURITYPACKAGE: { return "DPERR_CANTLOADSECURITYPACKAGE"; }
    case DPERR_ENCRYPTIONNOTSUPPORTED: { return "DPERR_ENCRYPTIONNOTSUPPORTED"; }
    case DPERR_CANTLOADCAPI: { return "DPERR_CANTLOADCAPI"; }
    case DPERR_NOTLOGGEDIN: { return "DPERR_NOTLOGGEDIN"; }
    case DPERR_LOGONDENIED: { return "DPERR_LOGONDENIED"; }
    }

    return "unknown error";
}

// 0x1001d320
VOID SendNetworkClientJoinRequest(VOID)
{
    NetworkState.Client.Message.Length = 0;

    AppendNetworkClientMessage(NETWORKMESSAGE_CONNECT_PLAYER);

    AppendNetworkClientMessage(MenuSaveState.Save.Unk08);

    AppendNetworkClientMessageValue(NetworkState.Client.ID);

    //AppendNetworkClientMessage(ARRAY_1004834c[0]); // TODO
    //AppendNetworkClientMessage(ARRAY_1004834c[1]); // TODO
    //AppendNetworkClientMessage(ARRAY_1004834c[2]); // TODO
    //AppendNetworkClientMessage(ARRAY_1004834c[3]); // TODO

    CONST U32 length = strlen(NetworkState.Client.Name);

    AppendNetworkClientMessage(length);
    
    for (U32 x = 0; x < length; x++)
    {
        AppendNetworkClientMessage(NetworkState.Client.Name[x]);
    }

    AppendNetworkClientMessageValue(NETWORK_MESSAGE_VERSION);

    SendNetworkClientMessage();

    LogTime();
    LogMessage("CLIENT: Sending join request.\n");
}

// 0x1001c650
VOID AppendNetworkClientMessageValue(CONST U32 message)
{
    ((U32*)(&NetworkState.Client.Message.Content[NetworkState.Client.Message.Length]))[0] = message;
    NetworkState.Client.Message.Length = NetworkState.Client.Message.Length + sizeof(U32);
}

// 0x1001bbf0
VOID HandleNetworkHostMessage(CONST DPID id, CONST NETWORKMESSAGEPTR message, CONST U32 size)
{
    if (id == DPID_SYSMSG) { return; }

    // TODO NOT IMPLEMENTED

    /*
    switch (message->Type)
    {
    case NETWORKMESSAGE_CONNECT_PLAYER: { ConnectNetworkClient(id, (NETWORKMESSAGECONNECTPTR)&message->Content, size - 1); break; }
    case NETWORKMESSAGE_UPDATE_PLAYER:
    {
        UpdateNetworkClient(id, message[NETWORK_MESSAGE_PLAYER_ID_INDEX], message[NETWORK_MESSAGE_UNK_2_INDEX],
            message[NETWORK_MESSAGE_UNK_3_INDEX], message[NETWORK_MESSAGE_UNK_4_INDEX]); break;
    }
    case NETWORKMESSAGE_UPDATE_PLAYER_STATE: { FUN_1001b7d0(id, (U8*)((ADDR)message + (ADDR)sizeof(U8)), size - 1); break; }
    case NETWORKMESSAGE_UPDATE_PLAYER_DATA: { FUN_1001b8d0(id, (U8*)((ADDR)message + (ADDR)sizeof(U8)), size - 1); break; }
    case NETWORKMESSAGE_DISCONECT_PLAYER: { FUN_1001b090(id, FALSE); break; }
    case NETWORKMESSAGE_PLAYER_STARTING: { FUN_1001bae0(id); break; }
    case 6:
        FUN_1001bba0(message, size);
        return;
    case 7:
        FUN_1001b970(id, (uint*)(message + 1));
        return;
    case 8:
        FUN_1001b9e0(id, (int*)(message + 1));
        return;
    case 0x13:
        FUN_1001b870(id, (undefined4*)(message + 1));
        return;
    case 0x40:
        FUN_1001bb30(id);
        return;
    case 0x41:
        FUN_1001bb70(id);
        return;
    }
    */
}

// 0x1001b270
VOID ConnectNetworkClient(CONST DPID id, CONST NETWORKMESSAGECONNECTPTR message, CONST U32 size)
{
    if (DAT_100470c8 != 0) // TODO
    {
        StopSendingPlayers(id, AcquireAssetMessage(ASSET_MESSAGE_REASON_NO_PLAYERS));

        return;
    }

    if (MenuSaveState.Save.Unk1A4 != 0) // TODO
    {
        LogTime();
        LogMessage("SERVER: Attempt to join with ID=%d\n", message->Index);
    }

    S32 indx = AcquireNetworkPlayerIndex(id);

    if (indx != INVALID_NETWORK_PLAYER_INDEX)
    {
        NetworkState.Server.Players[indx].JoinTicks = GetTickCount();

        FUN_1001af30(indx);

        return;
    }

    if (MenuSaveState.Save.Unk1A4 == NULL) // TODO
    {
        indx = AcquireNetworkPlayerIndex(DPID_SYSMSG);

        if (indx == INVALID_NETWORK_PLAYER_INDEX)
        {
            StopSendingPlayers(id, AcquireAssetMessage(ASSET_MESSAGE_REASON_NO_PLAYERS));

            return;
        }
    }
    else
    {
        indx = message->Index;

        if (NetworkState.Server.Players[indx].ID != DPID_SYSMSG)
        {
            StopSendingPlayers(id, AcquireAssetMessage(ASSET_MESSAGE_REASON_NO_PLAYERS));

            return;
        }
    }

    LogTime();
    LogMessage("SERVER: Add new player %i from %X\n", indx, id);

    NetworkState.Server.Players[indx].ID = id;
    NetworkState.Server.Players[indx].JoinTicks = GetTickCount();
    NetworkState.Server.Players[indx].Unk01 = message->Unk01;

    NetworkState.Players[indx].Unk111 = message->Unk02;
    NetworkState.Players[indx].Unk112 = message->Unk03;
    NetworkState.Players[indx].Unk113 = message->Unk04;
    NetworkState.Players[indx].Unk114 = message->Unk05;


    // TODO NOT IMPLEMENTED

    /*
    uVar10 = message->Length;
    size = size + -10;
    piVar13 = &message->Name;
    iVar5 = 0;
    if (uVar10 != 0) {
        size = size - uVar10;
        do {
            cVar1 = *(char*)piVar13;
            piVar13 = (int*)((int)piVar13 + 1);
            NetworkState.Players[iVar3].Name[iVar5] = cVar1;
            iVar5 = iVar5 + 1;
        } while (iVar5 < (int)uVar10);
    }
    NetworkState.Players[indx].Name[iVar5] = '\0';

    TODO
    */
}

// 0x1001dae0
VOID ExecuteNetwork(VOID) // TODO name
{
    if (State.App->Module->Network == NULL) { return; }

    while (TRUE)
    {
        DPID from = DPID_SYSMSG, to = DPID_SYSMSG;
        DWORD size = 0;

        HRESULT result = State.App->Module->Network->Receive(&from, &to, DPRECEIVE_ALL, NULL, &size);

        if (result != DPERR_BUFFERTOOSMALL) { break; }

        LPVOID content = malloc(size);

        State.App->Module->Network->Receive(&from, &to, DPRECEIVE_ALL, content, &size);

        if (from == DPID_SYSMSG)
        {
            LPDPMSG_GENERIC message = (LPDPMSG_GENERIC)content;
            if (message->dwType == DPSYS_DESTROYPLAYERORGROUP)
            {
                if (NetworkState.Connection.IsHost)
                {
                    LogTime();

                    CONST DPID id = ((LPDPMSG_DESTROYPLAYERORGROUP)content)->dpId;

                    LogMessage("Received DESTROYPLAYER for player %x\n", id);

                    FUN_1001b090(id, FALSE);
                }
            }
            else if (message->dwType == DPSYS_SESSIONLOST) { NetworkState.Client.State = NETWORKCLIENTSTATE_DISCONNECTED; }
        }
        else
        {
            if (NetworkState.Connection.IsHost && to == DPID_SERVERPLAYER) { HandleNetworkHostMessage(from, (NETWORKMESSAGEPTR)content, size); }
            if (NetworkState.Connection.IsClient && to != DPID_SERVERPLAYER) { HandleNetworkClientMessage(from, (NETWORKMESSAGEPTR)content, size); }
        }

        free(content);
    }

    if (NetworkState.Connection.IsHost) { FUN_1001c370(); }

    if (NetworkState.Connection.IsClient)
    {
        CONST U32 ticks = GetTickCount();

        if (NetworkState.Client.State == NETWORKCLIENTSTATE_NONE)
        {
            if (ticks - NetworkState.Client.Ticks <= 500) { return; }

            SendNetworkClientJoinRequest();
        }

        if (NetworkState.Client.State == NETWORKCLIENTSTATE_UNKNOWN1)
        {
            if ((DAT_10058e18 != 0) && 500 < GetTickCount() - NetworkState.Client.JoinTicks)
            {
                FUN_1001d050(1, 0xffffffff); // TODO
                NetworkState.Client.JoinTicks = GetTickCount();
            }

            // TODO NOT IMPLEMENTED
            /*
            for (U32 x = 0; x < MAX_NETWORK_STATE_PLAYER_COUNT; x++)
            {
                if (NetworkPlayer_ARRAY_100483d1[x].ID != DPID_SYSMSG)
                {
                    if (NetworkPlayer_ARRAY_100483d1[x].ID == DAT_1004884e)
                    {
                        if (500 < ticks - NetworkState.Client.Ticks && (DAT_10058e18 == 0)) { FUN_1001d3f0(x); }
                    }
                    else if (3000 < ticks - NetworkPlayer_ARRAY_100483d1[x].Ticks)
                    {
                        NetworkState.Client.Message.Length = 0;

                        AppendNetworkClientMessage(0x40); // TODO
                        AppendNetworkClientMessageValue(DAT_1004884e);

                        if (NetworkPlayer_ARRAY_100483d1[x].Unk02 == DPID_SYSMSG)
                        {
                            State.App->Module->Network->Send(NetworkState.Client.ID,
                                TODO, DPSEND_NONE, NetworkState.Client.Message.Content, NetworkState.Client.Message.Length);
                            State.App->Module->Network->Send(NetworkState.Client.ID,
                                NetworkPlayer_ARRAY_100483d1[x].Unk01, DPSEND_NONE, NetworkState.Client.Message.Content, NetworkState.Client.Message.Length);
                        }
                        else
                        {
                            State.App->Module->Network->Send(NetworkState.Client.ID,
                                NetworkPlayer_ARRAY_100483d1[x].Unk02, DPSEND_NONE, NetworkState.Client.Message.Content, NetworkState.Client.Message.Length);
                        }
                    }
                }
            }
            */
        }
    }
}

// 0x1001dff0
BOOL IsDirectPlayError(CONST HRESULT value)
{
    if (value == DP_OK) { return FALSE; }

    LogMessage("Error: %s\n", AcquireDirectPlayErrorDescription(value));

    return  TRUE;
}

// 0x1001e600
BOOL CLASSCALL MustCreateNetworkModuleStateSession(NETWORKSTATEMODULECONTAINERPTR self)
{
    return self->Connection->dwFlags & DPLCONNECTION_CREATESESSION;
}

// 0x1001d7d0
BOOL InitializeNetworkServer(VOID)
{
    // TODO NOT IMPLEMENTED

    return FALSE;
}

// 0x1001ef10
// a.k.a. CAConnect::Create
BOOL CLASSCALL OpenNetworkStateModuleContainer(NETWORKSTATEMODULECONTAINERPTR self, LPSTR name, CONST U32 players, CONST GUID guid)
{
    LogMessage("CAConnect::Create(%d)\n", self->Unk1111);

    if (self->Descriptor != NULL) { free(self->Descriptor); }


    self->Descriptor = (LPDPSESSIONDESC2)malloc(sizeof(DPSESSIONDESC2));
    ZeroMemory(self->Descriptor, sizeof(DPSESSIONDESC2));

    self->Descriptor->dwSize = sizeof(DPSESSIONDESC2);
    self->Descriptor->dwFlags = DPSESSION_MIGRATEHOST;

    self->Descriptor->guidApplication = NetworkApplication;

    self->Descriptor->dwCurrentPlayers = 0;
    self->Descriptor->dwMaxPlayers = players + 1;

    self->Descriptor->lpszSessionNameA = name;

    CopyMemory(&self->Descriptor->dwUser1, &guid, sizeof(GUID));

    do
    {
        self->Status = AcquireNetworkInstance()->Open(self->Descriptor, DPOPEN_RETURNSTATUS | DPOPEN_CREATE);

        LogTime();

        LogMessage("DPLAY: Open(CREATE) -> %x (%s)\n", self->Status, AcquireDirectPlayErrorDescription(self->Status));
    } while (self->Status == DPERR_CONNECTING);

    return self->Status == DP_OK;
}

// 0x1001e610
// a.k.a. CAConnect::connectThruLobby
BOOL CLASSCALL ConnectNetworkStateModuleContainer(NETWORKSTATEMODULECONTAINERPTR self, CONST GUID guid)
{
    LogMessage("CAConnect::connectThruLobby()\n");
    LogMessage("CAConnect::connectThruLobby() : dwFlags=%x\n", self->Connection->dwFlags);
    LogMessage("CAConnect::connectThruLobby() : dwSize=%x\n", self->Connection->dwSize);

    self->Connection->lpSessionDesc->dwFlags |= DPSESSION_MIGRATEHOST;

    if (MustCreateNetworkModuleStateSession(self))
    {
        self->Connection->lpSessionDesc->dwCurrentPlayers = 0;
        self->Connection->lpSessionDesc->dwMaxPlayers = (MAX_NETWORK_STATE_PIRECT_PLAY_PLAYER_COUT + 1);

        self->Connection->lpSessionDesc->guidApplication = NetworkApplication;

        CHAR name[MAX_SESSION_NAME_LENGTH];
        wsprintfA(name, "%s' lobby", State.Name);

        self->Connection->lpSessionDesc->lpszSessionNameA = name;

        CopyMemory(&self->Connection->lpSessionDesc->dwUser1, &guid, sizeof(GUID));
    }

    self->Status = self->Lobby->SetConnectionSettings(0, 0, self->Connection);

    LogMessage("CAConnect::connectThruLobby() : SetConnectionSettings(%s)\n",
        AcquireDirectPlayErrorDescription(self->Status));

    BOOL result = FALSE;

    if (!IsDirectPlayError(self->Status))
    {
        LogMessage("CAConnect::connectThruLobby() --> Connecting_Ex\n");

        LPDIRECTPLAY3A instance = NULL;
        self->Status = self->Lobby->ConnectEx(0, IID_IDirectPlay3A, (LPVOID*)&instance, NULL);

        LogMessage("CAConnect::connectThruLobby() : ConnectEx(%s)\n",
            AcquireDirectPlayErrorDescription(self->Status));

        if (!IsDirectPlayError(self->Status))
        {
            LogMessage("CAConnect::connectThruLobby() : _lpDP=%x\n", instance);

            SelectNetworkInstance(instance);

            DPSESSIONDESC2 desc;
            CopyMemory(&desc, self->Connection->lpSessionDesc, sizeof(DPSESSIONDESC2));

            SelectNetworkStateModuleContainerDesc(self, &desc);
            LogMessage("CAConnect::connectThruLobby() : DP=%x\n", State.App->Module->Network);

            result = TRUE;
        }
    }

    DIRECTPLAYRELEASE(self->Lobby);

    if (self->ClientID != NULL)
    {
        free(self->Connection);
        self->Connection = NULL;
    }

    return result;
}

// 0x1001e110
VOID SelectNetworkInstance(LPDIRECTPLAY3A value)
{
    State.App->Module->Network = value;
}

// 0x1001e210
BOOL CLASSCALL SelectNetworkStateModuleContainerDesc(NETWORKSTATEMODULECONTAINERPTR self, LPDPSESSIONDESC2 desc)
{
    if (self->Descriptor != NULL) { free(self->Descriptor); }

    self->Descriptor = (LPDPSESSIONDESC2)malloc(desc->dwSize);

    CopyMemory(self->Descriptor, desc, desc->dwSize);

    return TRUE;
}

// 0x1001f040
BOOL CLASSCALL JoinNetworkStateModuleContainer(NETWORKSTATEMODULECONTAINERPTR self, CONST U32 indx)
{
    if (InitializeNetworkModuleState(self) == 0) // TODO
    {
        if (self->Descriptor != NULL) { free(self->Descriptor); }

        self->Descriptor = (LPDPSESSIONDESC2)malloc(sizeof(DPSESSIONDESC2));
        ZeroMemory(self->Descriptor, sizeof(DPSESSIONDESC2));

        self->Descriptor->dwSize = sizeof(DPSESSIONDESC2);

        CopyMemory(&self->Descriptor->guidInstance, &self->Sessions.Items[indx].ID, sizeof(GUID));

        do
        {
            LogTime();
            LogMessage("DPLAY: Open(JOIN) -> ... waiting ...\n");

            self->Status = AcquireNetworkInstance()->Open(self->Descriptor, DPOPEN_RETURNSTATUS | DPOPEN_JOIN);

            LogTime();
            LogMessage("DPLAY: Open(JOIN) -> %x (%s)\n",
                self->Status, AcquireDirectPlayErrorDescription(self->Status));
        } while (self->Status == DPERR_CONNECTING);

        if (self->Status != DP_OK) { return FALSE; }
    }
    else
    {
        if (!ConnectNetworkStateModuleContainer(self, NetworkApplication)) { return FALSE; }
    }

    if (self->Descriptor != NULL) { free(self->Descriptor); }

    DWORD size = 0;
    self->Status = AcquireNetworkInstance()->GetSessionDesc(self->Descriptor, &size);

    if (self->Status != DPERR_BUFFERTOOSMALL) { return FALSE; }

    self->Descriptor = (LPDPSESSIONDESC2)malloc(size);

    self->Status = AcquireNetworkInstance()->GetSessionDesc(self->Descriptor, &size);

    return self->Status == DP_OK;
}

// 0x1001f5e0
VOID CLASSCALL DisposeNetworkState(NETWORKSTATEMODULECONTAINERPTR self)
{
    free(self->Sessions.Items);

    self->Sessions.Count = 0;
    self->Sessions.Items = NULL;
}

// 0x1001ee60
// a.k.a. EnumSessions
BOOL CLASSCALL EnumNetworkStateModuleContainerSessions(NETWORKSTATEMODULECONTAINERPTR self)
{
    if (InitializeNetworkModuleState(self) != 0) { return TRUE; } // TODO

    DPSESSIONDESC2 desc;
    ZeroMemory(&desc, sizeof(DPSESSIONDESC2));

    desc.dwSize = sizeof(DPSESSIONDESC2);
    desc.guidApplication = NetworkApplication;

    self->Sessions.Count = 0;

    self->Status = AcquireNetworkInstance()->EnumSessions(&desc, 1000,
        EnumNetworkStateModuleContainerSessionsCallback, self, DPENUMSESSIONS_STOPASYNC);

    LogTime();
    LogMessage("DPLAY: EnumSessions(STOPASYNC) -> %x (%s)\n",
        self->Status, AcquireDirectPlayErrorDescription(self->Status));

    return self->Status == DP_OK;
}

// 0x1001f630
// a.k.a. EnumSessionsCallback
BOOL PASCAL EnumNetworkStateModuleContainerSessionsCallback(LPCDPSESSIONDESC2 lpThisSD, LPDWORD lpdwTimeOut, DWORD dwFlags, LPVOID lpContext)
{
    LogTime();
    LogMessage("DPLAY: EnumSessionsCallback (dwFlag=%d)\n", dwFlags);

    if (dwFlags & DPESC_TIMEDOUT) { return FALSE; }

    AppendNetworkStateModuleContainerSession((NETWORKSTATEMODULECONTAINERPTR)lpContext, lpThisSD);

    return TRUE;
}

// 0x1001f460
VOID CLASSCALL AppendNetworkStateModuleContainerSession(NETWORKSTATEMODULECONTAINERPTR self, LPCDPSESSIONDESC2 desc)
{
    self->Sessions.Items = (NETWORKSESSIONPTR)realloc(self->Sessions.Items,
        (self->Sessions.Count + 1) * sizeof(NETWORKSESSION));

    lstrcpyA(self->Sessions.Items[self->Sessions.Count].Name, desc->lpszSessionNameA);

    self->Sessions.Items[self->Sessions.Count].CurrentPlayerCount = desc->dwCurrentPlayers;
    self->Sessions.Items[self->Sessions.Count].MaxPlayerCount = desc->dwMaxPlayers;

    CopyMemory(&self->Sessions.Items[self->Sessions.Count].ID, &desc->guidInstance, sizeof(GUID));
    CopyMemory(&self->Sessions.Items[self->Sessions.Count].User, &desc->dwUser1, sizeof(GUID));

    LogTime();
    LogMessage("DPLAY: RecordGame, game_num=%d\n", self->Sessions.Count);

    self->Sessions.Count = self->Sessions.Count + 1;
}

// 0x1001ae90
VOID StopSendingPlayers(CONST DPID id, LPCSTR message) // TODO name
{
    CHAR value[256]; // TODO

    strcat(value, message == NULL ? " " : message);

    State.App->Module->Network->Send(DPID_SERVERPLAYER, id, DPSEND_NONE, value, strlen(value) + 1);
    LogTime();
    LogMessage("SERVER: No more players sended to %X, reason=`%s`\n", id, message == NULL ? "NULL" : message);
}

// 0x1001d710
VOID FUN_1001d710(S32 param_1, S32 param_2, S32 param_3, LPCSTR param_4) // TODO
{
    // TODO NOT IMPLEMENTED

    //if (TODO) // TODO
    {
        // TODO


        //State.App->Module->Network->Send(NetworkState.Client.ID,
        //    DPID_SERVERPLAYER, DPSEND_NONE, TODO, TODO);

    }
}

// 0x1001af30
VOID FUN_1001af30(CONST U32 indx) // TODO
{
    NetworkState.Server.Message.Length = 0;

    AppendByteNetwordServerMessage(0x21); // TODO length?
    AppendIntegerNetwordServerMessage(NetworkState.Server.Players[indx].ID);
    AppendIntegerNetwordServerMessage(MenuSaveState.Save.MaxDelay);
    AppendIntegerNetwordServerMessage(MenuSaveState.Save.Ticks);

    if (MenuSaveState.Save.Unk1A4 == 0) // TODO
    {
        // TODO NOT IMPLEMENTED
        /*
        AppendIntegerNetwordServerMessage(DAT_100470cc);
        AppendIntegerNetwordServerMessage(DAT_100470d4);
        AppendIntegerNetwordServerMessage(DAT_100470d0);

        CONST U32 length = strlen(CHAR_ARRAY_100470d8) + 1;

        for (U32 x = 0; x < length; x++)
        {
            AppendByteNetwordServerMessage(CHAR_ARRAY_100470d8[x]);
        }
        */
    }

    AppendIntegerNetwordServerMessage(NETWORK_MESSAGE_VERSION);
    SendNetworkMessage(indx);

    NetworkState.Server.Players[indx].Unk05 = GetTickCount();

    LogTime();
    LogMessage("SERVER: Game StartupInfo sended to %i\n", indx);
}

// 0x1001adc0
VOID AppendIntegerNetwordServerMessage(CONST S32 value) // TODO name, type
{
    // TODO types
    ((S32*)NetworkState.Server.Message.Content)[NetworkState.Server.Message.Length] = value;
    NetworkState.Server.Message.Length = NetworkState.Server.Message.Length + sizeof(S32);
}

// 0x1001ada0
VOID AppendByteNetwordServerMessage(CONST U8 value) // TODO name, type
{
    // TODO types
    NetworkState.Server.Message.Content[NetworkState.Server.Message.Length] = value;
    NetworkState.Server.Message.Length = NetworkState.Server.Message.Length + sizeof(U8);
}

// 0x1001f570
VOID CLASSCALL ReleaseNetworkState(NETWORKSTATEMODULECONTAINERPTR self)
{
    for (U32 x = 0; x < self->Connections.Count; x++)
    {
        if (self->Connections.Items[x].Name != NULL) { free(self->Connections.Items[x].Name); }
        if (self->Connections.Items[x].Connection != NULL) { free(self->Connections.Items[x].Connection); }
    }

    free(self->Connections.Items);

    self->Connections.Count = 0;
    self->Connections.Items = NULL;
}

// 0x1001ebd0
// a.k.a. CAConnect::GetGameList
U32 CLASSCALL AcquireGameSessions(NETWORKSTATEMODULECONTAINERPTR self, NETWORKSESSIONPTR* sessions, U32* count) // TODO name, type
{
    LogMessage("CAConnect::GetGameList\n");

    if (InitializeNetworkModuleState(self) == 0) // TODO
    {
        LogMessage("CAConnect::GetGameList, not lobbied\n");

        DPSESSIONDESC2 desc;
        ZeroMemory(&desc, sizeof(DPSESSIONDESC2));

        desc.dwSize = sizeof(DPSESSIONDESC2);
        desc.guidApplication = NetworkApplication;

        self->Sessions.Count = 0;
        self->Status = AcquireNetworkInstance()->EnumSessions(&desc, 1000,
            EnumNetworkStateModuleContainerSessionsCallback, self,
            DPENUMSESSIONS_RETURNSTATUS | DPENUMSESSIONS_ASYNC | DPENUMSESSIONS_AVAILABLE);

        LogTime();
        LogMessage("DPLAY: EnumSessions(ASYNC) -> %x (%s); game_num = %d\n",
            self->Status, AcquireDirectPlayErrorDescription(self->Status), self->Sessions.Count);

        if (self->Status != DPERR_NOCONNECTION)
        {
            if (self->Status != DPERR_CONNECTING) { return self->Status == DP_OK; }

            return 2; // TODO
        }

        return 3; // TODO
    }

    if (self->Sessions.Items != NULL) { free(self->Sessions.Items); }

    self->Sessions.Count = 0;
    self->Sessions.Items = (NETWORKSESSIONPTR)malloc(sizeof(NETWORKSESSION));

    *count = self->Sessions.Count;
    *sessions = self->Sessions.Items;

    LogMessage("CAConnect::GetGameList, sessionName=%d\n",
        strlen(self->Connection->lpSessionDesc->lpszSessionNameA)); // TODO check for -239...

    CHAR name[256]; // TODO
    wsprintfA(name, "[%s]", self->Connection->lpSessionDesc->lpszSessionNameA == NULL
        ? "NULL" : self->Connection->lpSessionDesc->lpszSessionNameA);
    lstrcpyA(self->Sessions.Items[self->Sessions.Count].Name, name);

    self->Sessions.Items[self->Sessions.Count].CurrentPlayerCount = self->Connection->lpSessionDesc->dwCurrentPlayers;
    self->Sessions.Items[self->Sessions.Count].MaxPlayerCount = self->Connection->lpSessionDesc->dwMaxPlayers;

    CopyMemory(&self->Sessions.Items[self->Sessions.Count].ID,
        &self->Connection->lpSessionDesc->guidInstance, sizeof(GUID));

    CopyMemory(&self->Sessions.Items[self->Sessions.Count].User,
        &self->Connection->lpSessionDesc->dwUser1, sizeof(GUID));

    self->Sessions.Count = self->Sessions.Count + 1;
    *count = self->Sessions.Count + 1;

    return 1; // TODO
}

// 0x1001bf20
VOID SendPlayerMap(CONST U32 indx) // TODO name
{
    if (NetworkState.Server.PlayerStates[indx].Unk8C != DAT_100470cc)
    {
        if (NetworkState.Server.PlayerStates[indx].Unk8C == -1) // TODO
        {
            AppendIntegerNetwordServerMessage(GetTickCount());
            AppendIntegerNetwordServerMessage(0);
            AppendIntegerNetwordServerMessage(0);
            AppendIntegerNetwordServerMessage(0);
            NetworkState.Server.PlayerStates[indx].Unk94 = 1000; // TODO
        }
        else
        {
            // TODO NOT IMPLEMENTED
            /*
            if (DAT_100470cc < size + uVar5)
            {
                size = DAT_100470cc - uVar5;
            }

            AppendIntegerNetwordServerMessage(GetTickCount());
            AppendIntegerNetwordServerMessage(uVar5);
            AppendIntegerNetwordServerMessage(size);

            {
                puStack_11c = &stack0xfffffecc;
                pcVar10 = StringsState.Scratch;
                AcquireStringValue((StringValue*)&stack0xfffffecc, StringsState.Scratch);
                uStack_4 = 0;
                puStack_110 = &stack0xfffffec8;
                AcquireSettingsValue((StringValue*)&stack0xfffffec8, IDS_MULTI_MAP_DIR);
                uStack_4 = 0xffffffff;
                pSVar3 = AcquireSettingsValue(&SStack_114, (StringValue)pcVar10, value);
                pcVar6 = pSVar3->Value;
                pcVar10 = acStack_10c;
                do {
                    cVar1 = *pcVar6;
                    pcVar6 = pcVar6 + 1;
                    *pcVar10 = cVar1;
                    pcVar10 = pcVar10 + 1;
                } while (cVar1 != '\0');
                ReleaseStringValue(&SStack_114);
            }

            uVar7 = 0xffffffff;
            pcVar10 = MenuSaveItem_10048880.Name;
            do {
                pcVar6 = pcVar10;
                if (uVar7 == 0) break;
                uVar7 = uVar7 - 1;
                pcVar6 = pcVar10 + 1;
                cVar1 = *pcVar10;
                pcVar10 = pcVar6;
            } while (cVar1 != '\0');
            puStack_11c = (undefined*)~uVar7;
            iVar8 = -1;
            pcVar10 = acStack_10c;
            do {
                pcVar9 = pcVar10;
                if (iVar8 == 0) break;
                iVar8 = iVar8 + -1;
                pcVar9 = pcVar10 + 1;
                cVar1 = *pcVar10;
                pcVar10 = pcVar9;
            } while (cVar1 != '\0');
            pcVar10 = pcVar6 + -(int)puStack_11c;
            pcVar6 = pcVar9 + -1;
            for (uVar7 = (uint)puStack_11c >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
                *(undefined4*)pcVar6 = *(undefined4*)pcVar10;
                pcVar10 = pcVar10 + 4;
                pcVar6 = pcVar6 + 4;
            }
            for (uVar7 = (uint)puStack_11c & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
                *pcVar6 = *pcVar10;
                pcVar10 = pcVar10 + 1;
                pcVar6 = pcVar6 + 1;
            }

            if (!OpenBinFile(&local_120, acStack_10c, 0)) { return; }

            PointBinFile(&local_120, uVar5, 0);
            ReadBinFile(&local_120, NetworkState.Server.Message.Content + NetworkState.Server.Message.Length, size);
            CloseBinFile(&local_120);

            uVar5 = 0;
            uVar7 = 0;
            if (size != 0) {
                do {
                    uVar5 = uVar5 * 0x10 +
                        (uint)NetworkState.Server.Message.Content
                        [uVar7 + NetworkState.Server.Message.Length] + (uVar5 >> 0x1c);
                    uVar7 = uVar7 + 1;
                } while (uVar7 < size);
            }
            NetworkState.Server.Message.Length = NetworkState.Server.Message.Length + size;
            AppendIntegerNetwordServerMessage(uVar5);
            */
        }
        
        NetworkState.Server.PlayerStates[indx].Unk90 = GetTickCount();

        LogTime();
        //LogMessage("SERVER: Sending MAP (%d,%d) to player %d\n", todo, todo, todo); // TODO
    }
}

// 0x1001be60
VOID SendPlayerMultiplayerSettings(CONST U32 indx) // TODO name

{
    if (NetworkState.Server.PlayerStates[indx].Unk88 != DAT_10047fc4)
    {
        AppendIntegerNetwordServerMessage(DAT_10047fc4);
        FUN_1001ade0(&MultiplayerSettingsState, sizeof(MULTIPLAYERSETTINGS));
    }
}

// 0x1001ade0
VOID FUN_1001ade0(LPCVOID content, CONST U32 size) // TODO name
{
    for (U32 x = 0; x < size; x++) { AppendByteNetwordServerMessage(((LPBYTE)content)[x]); }
}

// 0x1001be90
VOID FUN_1001be90(CONST U32 indx) // TODO name
{
    // TODO not implemented
}

// 0x1001c710
VOID FUN_1001c710(VOID) // TODO name
{
    // TODO not implemented
}

// 0x10018570
BOOL FUN_10018570(LPCSTR path, UNKNETWORKIDPTR value) // TODO name
{
    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (OpenZipFile(&zip, path, ZIPFILE_OPEN_READ)) { return FALSE; }

    ReadZipFile(&zip, &value->ID, sizeof(GUID));
    ReadZipFile(&zip, &value->Unk1, sizeof(U32)); // TODO
    CloseZipFile(&zip);

    return TRUE;
}

// 0x1001d9a0
// a.k.a. DPLAY_InitHostorClient
BOOL InitializeNetwork(VOID) // TODO name
{
    State.App->Module->Network = NULL;

    if (!InitializeNetworkState(&NetworkState.State)) { return FALSE; }

    ZeroMemory(&GUID_10048b94, sizeof(GUID));

    LogTime();
    LogMessage("DPLAY_InitHostorClient().\n");

    return TRUE;
}

// 0x1001e130
BOOL CLASSCALL InitializeNetworkState(NETWORKSTATEMODULECONTAINERPTR self)
{
    if (InitializeNetworkModuleState(self) == 0) // TODO
    {
        LPDIRECTPLAY3A instance = NULL;
        self->Status = CoCreateInstance(CLSID_DirectPlay, NULL,
            CLSCTX_INPROC_SERVER, IID_IDirectPlay3A, (LPVOID*)&instance);

        LogTime();
        LogMessage("DPLAY: CoCreateInstance(DirectPlay) -> %x (%s)\n",
            self->Status, AcquireDirectPlayErrorDescription(self->Status));

        if (self->Status != DP_OK)
        {
            self->IsActive = FALSE;

            return FALSE;
        }

        SelectNetworkInstance(instance);
        ReleaseNetworkState(self);

        self->Status = AcquireNetworkInstance()->EnumConnections(&NetworkApplication,
            EnumNetwordConnectionsCallback, self, DPCONNECTION_DIRECTPLAY);

        LogMessage("DPLAY: EnumConnections -> %x (%s)\n",
            self->Status, AcquireDirectPlayErrorDescription(self->Status));

        if (self->Status != DP_OK)
        {
            self->IsActive = FALSE;

            return FALSE;
        }
        self->IsActive = TRUE;

    }

    return TRUE;
}

// 0x1001f600
BOOL PASCAL EnumNetwordConnectionsCallback(LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
    FUN_1001f390((NETWORKSTATEMODULECONTAINERPTR)lpContext, lpguidSP, lpConnection, dwConnectionSize, lpName);

    return TRUE;
}

// 0x1001f390
VOID CLASSCALL FUN_1001f390(NETWORKSTATEMODULECONTAINERPTR self, LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize, LPCDPNAME lpName)
{
    self->Connections.Items = (NETWORKCONNECTIONPTR)realloc(self->Connections.Items,
        (self->Connections.Count + 1) * sizeof(NETWORKCONNECTION));

    self->Connections.Items[self->Connections.Count].Name = (LPSTR)malloc(strlen(lpName->lpszShortNameA) + 1);
    strcpy(self->Connections.Items[self->Connections.Count].Name, lpName->lpszShortNameA);

    self->Connections.Items[self->Connections.Count].Connection = malloc(dwConnectionSize);
    CopyMemory(self->Connections.Items[self->Connections.Count].Connection, lpConnection, dwConnectionSize);

    CopyMemory(&self->Connections.Items[self->Connections.Count].ID, lpguidSP, sizeof(GUID));

    self->Connections.Count = self->Connections.Count + 1;
}

// 0x1001e9f0
BOOL CLASSCALL NetworkModuleStateContainerUnknown1(NETWORKSTATEMODULECONTAINERPTR self, LPCSTR name)
{
    // TODO NOT IMPLEMENTED
    return TRUE;
}

// 0x1001eae0
BOOL CLASSCALL NetworkModuleStateContainerUnknown2(NETWORKSTATEMODULECONTAINERPTR self, LPCSTR name)
{
    // TODO NOT IMPLEMENTED
    return TRUE;
}

// 0x1001e950
BOOL CLASSCALL FUN_1001e950(NETWORKSTATEMODULECONTAINERPTR self) // TODO
{
    // TODO NOT IMPLEMENTED
    return TRUE;
}

// 0x1001e260
BOOL CLASSCALL FUN_1001e260(NETWORKSTATEMODULECONTAINERPTR self) // TODO
{
    self->Status = AcquireNetworkInstance()->SetSessionDesc(self->Descriptor, 0); // TODO FLAG VALUE
    
    LogTime();
    LogMessage("DPLAY: SetSessionDesc -> %x (%s)\n",
        self->Status, AcquireDirectPlayErrorDescription(self->Status));

    return self->Status == DP_OK;
}

// 0x1001ad60
S32 AcquireNetworkPlayerIndex(CONST DPID id)
{
    // TODO NOT IMPLEMENTED
    return -1;
}

// 0x1001b090
BOOL FUN_1001b090(CONST DPID id, CONST BOOL ban) // TODO
{
    // TODO NOT IMPLEMENTED

    return FALSE;
}

// 0x1001d1c0
VOID HandleNetworkClientMessage(CONST DPID id, CONST NETWORKMESSAGEPTR message, CONST U32 size)
{
    // TODO NOT IMPLEMENTED
}

// 0x1001c370
VOID FUN_1001c370(VOID) // TODO
{
    // TODO NOT IMPLEMENTED
}

// 0x1001d050
VOID FUN_1001d050(S32 param_1, S32 param_2) // TODO
{
    // TODO NOT IMPLEMENTED
}