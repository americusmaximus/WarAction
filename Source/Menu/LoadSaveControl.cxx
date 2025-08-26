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
#include "MessageControl.hxx"
#include "Messages.hxx"
#include "LoadSaveControl.hxx"
#include "Saves.hxx"
#include "Settings.hxx"

#include <..\Text\Resources.hxx>

#include <stdlib.h>

// 0x1003a68c
LOADSAVECONTROLSELF LoadSaveControlSelfState =
{
    (LOADSAVECONTROLTYPEACTION)AcquireControlTypeLoadSave,
    InitializeLoadSaveControl,
    (LOADSAVECONTROLDISABLEACTION)DisablePanelControl,
    (LOADSAVECONTROLTICKACTION)TickPanelControl,
    (LOADSAVECONTROLACTIONACTION)ActionLoadSaveControl,
    (LOADSAVECONTROLRELEASEACTION)ReleasePanelControl
};

STATIC S32 SortSaveFileItem(void const* a, void const* b)
{
    SAVEFILEITEMPTR first = (SAVEFILEITEMPTR)a;
    SAVEFILEITEMPTR second = (SAVEFILEITEMPTR)b;

    return CompareFileTime(&second->Time, &first->Time);
}

// 0x10014c00
LOADSAVECONTROLPTR CLASSCALL ActivateLoadSaveControl(LOADSAVECONTROLPTR self)
{
    self->Self = &LoadSaveControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Scene = ActivateSceneControl(ALLOCATE(SCENECONTROL),
        &AssetsState.Assets.Single3BK, &AssetsState.Assets.Single3BT, CONTROLACTION_SINGLE3_LOAD);

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.Single3SC, CONTROLACTION_SCROLL);

    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->List = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 48, 84, 264, 306, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Nodes = ALLOCATE(CONTROLNODE);

    InitControlNode(self->Nodes, (CONTROLPTR)self->Scene);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->List);

    return self;
}

// 0x10014d60
VOID CLASSCALL InitializeLoadSaveControl(LOADSAVECONTROLPTR self)
{
    CHAR pattern[MAX_FILE_NAME_LENGTH];

    {
        STRINGVALUE name, value;
        AcquireSettingsValue(&name, IDS_SAVES);
        AcquireStringValue(&value, StringsState.Scratch);

        STRINGVALUE setting;
        STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

        strcpy(pattern, actual->Value);

        ReleaseStringValue(actual);
    }

    strcat(pattern, "*.sss");

    U32 count = 0;
    SAVEFILEITEMPTR saves = NULL;

    FINDFILECONTEXT context;
    HANDLE handle = FindFile(pattern, &context);

    if (handle != INVALID_HANDLE_VALUE)
    {
        CHAR path[MAX_FILE_NAME_LENGTH];

        do
        {
            strcpy(path, context.Path);

            LPSTR dot = strrchr(path, '.');
            if (dot != NULL) { dot[0] = NULL; }

            strupr(path);

            SAVEFILEITEMPTR items =
                (SAVEFILEITEMPTR)realloc(saves, (count + 1) * sizeof(SAVEFILEITEM));

            if (items != NULL) { saves = items; } else { break; }

            saves[count].Time = context.LastWriteTime;
            strcpy(saves[count].Name, path);

            count = count + 1;
        } while (FindFileNext(handle, &context));
    }

    FindClose(handle);
    ReleaseStringList(self->Items);

    if (count != 0)
    {
        // NOTE: Originally in-place bubble sorting.
        qsort(saves, count, sizeof(SAVEFILEITEM), SortSaveFileItem);

        for (U32 x = 0; x < count; x++) { AppendStringList(self->Items, saves[x].Name); }
    }

    free(saves);
    InitializeScrollControlArea(self->Scroll, 9, -11, 1);
    InitializePanelControl((PANELCONTROLPTR)self);
    SelectListControlItem(self->List, 0);
}

// 0x10015350
U32 CLASSCALL ActionLoadSaveControl(LOADSAVECONTROLPTR self)
{
    CONST U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command == NULL || command->Action != CONTROLACTION_LIST_SELECT || command->Parameter1 != CONTROLACTION_UI_SELECT)
    {
        if (action != CONTROLACTION_SINGLE3_LOAD)
        {
            if (action != CONTROLACTION_SINGLE3_CANCEL) { return CONTROLACTION_NONE; }

            return CONTROLACTION_SINGLE3_CANCEL;
        }
    }
    else { DequeueControlCommand(TRUE); }

    if (self->List->Index >= INVALID_LIST_CONTROL_INDEX)
    {
        CHAR name[MAX_FILE_NAME_LENGTH];
        strcpy(name, self->Items->Items[self->List->Index]);

        if (LoadSaveState(name)) { return CONTROLACTION_SINGLE3_LOAD; }

        ShowMessageControl(&MessageControlState,
            AcquireAssetMessage(ASSET_MESSAGE_UNABLE_TO_LOAD_SAVE), MESSAGE_BUTTON_OK);
    }

    return CONTROLACTION_NONE;
}
