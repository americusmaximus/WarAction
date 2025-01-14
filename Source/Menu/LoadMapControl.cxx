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
#include "ControlCommand.hxx"
#include "FileFinder.hxx"
#include "Graphics.hxx"
#include "LoadMapControl.hxx"
#include "MenuSave.hxx"
#include "Saves.hxx"
#include "SceneControl.hxx"
#include "Settings.hxx"
#include "State.hxx"

#include <..\Text\Resources.hxx>

#include <stdio.h>

// 0x1003a6a4
LOADMAPCONTROLSELF LoadMapControlSelfState =
{
    (LOADMAPCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeLoadMapControl,
    DisableLoadMapControl,
    (LOADMAPCONTROLTICKACTION)TickPanelControl ,
    ActionLoadMapControl,
    (LOADMAPCONTROLRELEASEACTION)ReleasePanelControl
};

STATIC S32 SortMapFileItem(void const* a, void const* b)
{
    MAPFILEITEMPTR first = (MAPFILEITEMPTR)a;
    MAPFILEITEMPTR second = (MAPFILEITEMPTR)b;

    return CompareFileTime(&second->Time, &first->Time);
}

// 0x10015410
LOADMAPCONTROLPTR CLASSCALL ActivateLoadMapControl(LOADMAPCONTROLPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self,
        &AssetsState.Assets.Single4BK, &AssetsState.Assets.Single4BT, CONTROLACTION_SINGLE4_LOAD);

    self->Self = &LoadMapControlSelfState;

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL),
        &AssetsState.Assets.Single4SC, CONTROLACTION_SCROLL);

    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->List = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 49, 87, 270, 303, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Size = ActivateLabelControl(ALLOCATE(LABELCONTROL), 532, 344, 78, 15, NULL);
    self->Size->IsCentered = TRUE;
    self->Size->Font = &AssetsState.Fonts.Main;
    self->Size->Color = ADJUSTCOLOR(0xC800);

    self->Map = ActivateMapControl(ALLOCATE(MAPCONTROL), 537, 277, 480, 93, 111, 176, self->Size, NULL);

    AppendControlNode(self->Nodes, (CONTROLPTR)self->List);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Size);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Map);

    return self;
}

// 0x10015a10
VOID CLASSCALL InitializeLoadMapControl(LOADMAPCONTROLPTR self)
{
    self->Unk10 = 0; // TODO

    InitializeScrollControlArea(self->Scroll, 7, -13, 3);
    InitializePanelControl((PANELCONTROLPTR)self);
    InitializeMapsLoadMapControl(self);
    SelectListControlItem(self->List, 0);
}

// 0x10015d60
VOID CLASSCALL DisableLoadMapControl(LOADMAPCONTROLPTR self)
{
    ReleaseStringList(self->Items);
    DisablePanelControl((PANELCONTROLPTR)self);
}

// 0x10015a50
U32 CLASSCALL ActionLoadMapControl(LOADMAPCONTROLPTR self)
{
    if (self->Unk10 != 0) { return CONTROLACTION_SINGLE4_LOAD; } // TODO

    U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_UI && command->Action == CONTROLACTION_LIST_SELECT)
    {
        if (command->Parameter1 == CONTROLACTION_UI_CHANGE)
        {
            CHAR path[MAX_FILE_NAME_LENGTH];

            {
                STRINGVALUE name, value;
                AcquireSettingsValue(&name, IDS_SINGLE_MAP_DIR);
                AcquireStringValue(&value, StringsState.Scratch);

                STRINGVALUE setting;
                STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

                strcpy(path, actual->Value);

                ReleaseStringValue(actual);
            }

            self->Items->Self->AcquireValue(self->Items, self->List->Index,
                (LPSTR)((ADDR)path + (ADDR)strlen(path)));

            LPSTR colon = strrchr(path, ':');
            if (colon != NULL) { colon[0] = NULL; }

            strcat(path, ".ssm");

            InitializeMapMapControl(self->Map, path);
        }

        if (command->Action == CONTROLACTION_LIST_SELECT && command->Parameter1 == CONTROLACTION_UI_SELECT)
        {
            action = CONTROLACTION_SINGLE4_LOAD;
            goto LAB_10015ba5; // TODO
        }
    }

    if (action != CONTROLACTION_SINGLE4_LOAD) { return action; }

LAB_10015ba5:
    if (self->List->Index == INVALID_LIST_CONTROL_INDEX) { return CONTROLACTION_NONE; }

    MenuSaveState.Save.Unk1A4 = 0; // TODO
    self->Items->Self->AcquireValue(self->Items, self->List->Index, MenuSaveItem_10048880.Name);

    return action;
}

// 0x10015620
VOID CLASSCALL InitializeMapsLoadMapControl(LOADMAPCONTROLPTR self)
{
    ReleaseStringList(self->Items);

    self->List->Index = INVALID_LIST_CONTROL_INDEX;

    CHAR pattern[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SINGLE_MAP_DIR);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        strcpy(pattern, actual->Value);

        ReleaseStringValue(actual);
    }

    strcat(pattern, "*.ssm");

    U32 count = 0;
    MAPFILEITEMPTR maps = NULL;

    FINDFILECONTEXT context;
    HANDLE handle = FindFile(pattern, &context);

    if (handle != INVALID_HANDLE_VALUE)
    {
        CHAR file[MAX_FILE_NAME_LENGTH];

        do
        {
            strcpy(file, context.Path);

            LPSTR dot = strrchr(file, '.');
            if (dot != NULL) { dot[0] = NULL; }

            strupr(file);

            CHAR path[MAX_FILE_NAME_LENGTH];

            {
                STRINGVALUE name, value;
                AcquireSettingsValue(&name, IDS_SINGLE_MAP_DIR);
                AcquireStringValue(&value, StringsState.Scratch);

                STRINGVALUE setting;
                STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

                strcpy(path, actual->Value);

                ReleaseStringValue(actual);
            }

            strcat(path, file);
            strcat(path, ".ssm");

            if (ValidateMapFile(path))
            {
                MAPPTR map = ActivateMap(ALLOCATE(MAP));
                InitializeSingleMap(path, map);

                maps = (MAPFILEITEMPTR)realloc(maps, (count + 1) * sizeof(MAPFILEITEM));

                ZeroMemory(&maps[count].Time, sizeof(FILETIME));

                if (map->Unk0x154 == NULL) { strcpy(maps[count].Name, file); }
                else { sprintf(maps[count].Name, "%s: %s", file, map->Unk0x154); }

                count = count + 1;

                DisposeMap(map);
                free(map);
            }
        } while (FindFileNext(handle, &context));
    }

    FindClose(handle);

    if (count != 0)
    {
        // NOTE: Originally in-place bubble sorting.
        qsort(maps, count, sizeof(MAPFILEITEM), SortMapFileItem);

        for (U32 x = 0; x < count; x++) { AppendStringList(self->Items, maps[x].Name); }
    }

    free(maps);

    ListControlCommandUnknown1(self->List);
    InitializeMapMapControl(self->Map, NULL);
}