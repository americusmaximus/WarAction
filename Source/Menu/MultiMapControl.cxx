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

#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "FileFinder.hxx"
#include "Graphics.hxx"
#include "MenuSave.hxx"
#include "MultiMapControl.hxx"
#include "Saves.hxx"
#include "SceneControl.hxx"
#include "State.hxx"

#include <../Text/Resources.hxx>

#include <stdlib.h>

// 0x1003a59c
MULTIMAPCONTROLSELF MultiMapControlSelfState =
{
    (MULTIMAPCONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeMultiMapControl,
    (MULTIMAPCONTROLDISABLEACTION)DisablePanelControl,
    (MULTIMAPCONTROLTICKACTION)TickPanelControl,
    ActionMultiMapControl,
    (MULTIMAPCONTROLRELEASEACTION)ReleasePanelControl,
};

// 0x1000f050
MULTIMAPCONTROLPTR CLASSCALL ActivateMultiMapControl(MULTIMAPCONTROLPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self,
        &AssetsState.Assets.Multi3BK, &AssetsState.Assets.Multi3BT, CONTROLACTION_MULTI3_OK);

    self->Self = &MultiMapControlSelfState;

    self->Scroll = ActivateScrollControl(ALLOCATE(SCROLLCONTROL), &AssetsState.Assets.Multi3SC, CONTROLACTION_SCROLL);
    self->Items = ActivateStringList(ALLOCATE(STRINGLIST));

    self->List = ActivateListControl(ALLOCATE(LISTCONTROL),
        CONTROLACTION_LIST_SELECT, 49, 87, 270, 303, self->Scroll, &AssetsState.Fonts.Main, self->Items);

    self->Options = ActivateObjectType4x12(ALLOCATE(CONTROLTYPE4X12),
        CONTROLACTION_MULTI3_OPTIONS, &AssetsState.Assets.Multi3OP, TRUE);

    self->Size = ActivateLabelControl(ALLOCATE(LABELCONTROL), 532, 344, 78, 15, NULL);
    self->Size->IsCentered = TRUE;
    self->Size->Font = &AssetsState.Fonts.Main;
    self->Size->Color = ADJUSTCOLOR(0xC800);

    self->Actors = ActivateLabelControl(ALLOCATE(LABELCONTROL), 539, 359, 57, 15, NULL);
    self->Actors->IsCentered = TRUE;
    self->Actors->Font = &AssetsState.Fonts.Main;
    self->Actors->Color = ADJUSTCOLOR(0xC800);

    self->Map = ActivateMapControl(ALLOCATE(MAPCONTROL), 537, 277, 479, 122, 113, 142, self->Size, self->Actors);

    AppendControlNode(self->Nodes, (CONTROLPTR)self->List);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Options);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Size);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Actors);
    AppendControlNode(self->Nodes, (CONTROLPTR)self->Map);

    return self;
}

// 0x1000f7b0
VOID CLASSCALL InitializeMultiMapControl(MULTIMAPCONTROLPTR self)
{
    self->Unk12 = 0; // TODO

    InitializeScrollControlArea(self->Scroll, 7, -13, 3);
    InitializePanelControl((PANELCONTROLPTR)self);
    ClickObjectType4x12(self->Options, MULTI_MAP_GAME_MAPS_BUTTON_INDEX);
    FUN_1000f340(self);
    SelectListControlItem(self->List, 0);
}

// 0x1000f7f0
U32 CLASSCALL ActionMultiMapControl(MULTIMAPCONTROLPTR self)
{
    if (self->Unk12 != 0) { return CONTROLACTION_MULTI3_OK; } // TODO

    U32 action = ActionPanelControl((PANELCONTROLPTR)self);
    
    CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command != NULL && command->Command == CONTROLCOMMAND_TEXT_CONTROL)
    {
        if (command->Action == CONTROLACTION_LIST_SELECT && command->Parameter1 == 2) // TODO
        {
            CHAR path[MAX_FILE_NAME_LENGTH];
            ZeroMemory(path, MAX_FILE_NAME_LENGTH);

            if (FUN_10002440(self->Options) == 0) // TODO
            {
                {
                    STRINGVALUE name, value;
                    AcquireSettingsValue(&name, IDS_MULTI_MAP_DIR);
                    AcquireStringValue(&value, StringsState.Scratch);

                    STRINGVALUE setting;
                    STRINGVALUEPTR actual = AcquireSettingsValue(&setting, name, value);

                    strcpy(path, actual->Value);

                    ReleaseStringValue(actual);
                }

                self->Items->Self->AcquireValue(self->Items,
                    self->List->Index, (LPSTR)((ADDR)path + (ADDR)(strlen(path) + 1)));

                strcat(path, ".smm");
            }

            InitializeMapMapControl(self->Map, path);
            DequeueControlCommand(TRUE);
        }

        if (command->Action == CONTROLACTION_MULTI3_OPTIONS && command->Parameter1 == 4) // TODO
        {
            DequeueControlCommand(TRUE);
            FUN_1000f340(self);
        }

        if (command->Action == CONTROLACTION_LIST_SELECT && command->Parameter1 == 7) // TODO
        {
            action = CONTROLACTION_MULTI3_OK;
            goto LAB_1000f98e; // TODO refactor out
        }
    }

    if (action != CONTROLACTION_MULTI3_OK) { return action; }

LAB_1000f98e: // TODO refactor out

    if (self->List->Index == INVALID_LIST_CONTROL_INDEX) { return CONTROLACTION_NONE; }

    // TODO NOT IMPLEMENTED
    /*
    {
        MenuSaveState.Save.Unk1A4 = FUN_10002440(self->Options) != 0; // TODO

        self->Items->Self->AcquireValue(self->Items, self->List->Index, MenuSaveItem_10048880.Name);

        cVar1 = s_.smm_1003fa7c[4];
        cVar2 = s_.sms_1003fa74[4];
        SVar8.Value = (char*)0xffffffff;
        pcVar12 = MenuSaveItem_10048880.Name;

        if (MenuSaveState.Save.Unk1A4 == 0) // TODO
        {
            do {
                pcVar9 = pcVar12;
                if (SVar8.Value == (char*)0x0) break;
                SVar8.Value = SVar8.Value + -1;
                pcVar9 = pcVar12 + 1;
                cVar2 = *pcVar12;
                pcVar12 = pcVar9;
            } while (cVar2 != '\0');
            SVar8.Value = (char*)CONCAT31((int3)((uint)SVar8.Value >> 8), s_.smm_1003fa7c[4]);
            cVar2 = s_.smm_1003fa7c[1];
            cVar3 = s_.smm_1003fa7c[2];
            cVar4 = s_.smm_1003fa7c[3];
            pcVar9[-0xffffffff00000001] = s_.smm_1003fa7c[0];
            pcVar9[0] = cVar2;
            pcVar9[1] = cVar3;
            pcVar9[2] = cVar4;
            pcVar9[3] = cVar1;
            local_644 = &stack0xfffff99c;
            pcVar12 = StringsState.Scratch;
            AcquireStringValue((StringValue*)&stack0xfffff99c, StringsState.Scratch);
            local_648 = &stack0xfffff998;
            local_4 = 1;
            AcquireSettingsValue((StringValue*)&stack0xfffff998, IDS_MULTI_MAP_DIR);
            local_4 = 0xffffffff;
            pSVar6 = AcquireSettingsValue(&local_654, (StringValue)pcVar12, SVar8);
            pcVar9 = pSVar6->Value;
            pcVar12 = acStack_640;
            do {
                cVar2 = *pcVar9;
                pcVar9 = pcVar9 + 1;
                *pcVar12 = cVar2;
                pcVar12 = pcVar12 + 1;
            } while (cVar2 != '\0');
        }
        else {
            do {
                pcVar9 = pcVar12;
                if (SVar8.Value == (char*)0x0) break;
                SVar8.Value = SVar8.Value + -1;
                pcVar9 = pcVar12 + 1;
                cVar1 = *pcVar12;
                pcVar12 = pcVar9;
            } while (cVar1 != '\0');
            local_644 = &stack0xfffff99c;
            cVar1 = s_.sms_1003fa74[1];
            cVar3 = s_.sms_1003fa74[2];
            cVar4 = s_.sms_1003fa74[3];
            pcVar9[-0xffffffff00000001] = s_.sms_1003fa74[0];
            pcVar9[0] = cVar1;
            pcVar9[1] = cVar3;
            pcVar9[2] = cVar4;
            pcVar12 = StringsState.Scratch;
            pcVar9[3] = cVar2;
            AcquireStringValue((StringValue*)&stack0xfffff99c, StringsState.Scratch);
            local_648 = &stack0xfffff998;
            local_4 = 2;
            AcquireSettingsValue((StringValue*)&stack0xfffff998, IDS_SAVES);
            local_4 = 0xffffffff;
            pSVar6 = AcquireSettingsValue(&local_654, (StringValue)pcVar12, SVar8);
            pcVar9 = pSVar6->Value;
            pcVar12 = acStack_640;
            do {
                cVar2 = *pcVar9;
                pcVar9 = pcVar9 + 1;
                *pcVar12 = cVar2;
                pcVar12 = pcVar12 + 1;
            } while (cVar2 != '\0');
        }
        ReleaseStringValue(&local_654);


        uVar7 = 0xffffffff;
        pcVar12 = MenuSaveItem_10048880.Name;
        do {
            pcVar9 = pcVar12;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar9 = pcVar12 + 1;
            cVar2 = *pcVar12;
            pcVar12 = pcVar9;
        } while (cVar2 != '\0');
        uVar7 = ~uVar7;
        iVar5 = -1;
        pcVar12 = acStack_640;
        do {
            pcVar11 = pcVar12;
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            pcVar11 = pcVar12 + 1;
            cVar2 = *pcVar12;
            pcVar12 = pcVar11;
        } while (cVar2 != '\0');
        pcVar12 = pcVar9 + -uVar7;
        pcVar9 = pcVar11 + -1;
        for (uVar10 = uVar7 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
            *(undefined4*)pcVar9 = *(undefined4*)pcVar12;
            pcVar12 = pcVar12 + 4;
            pcVar9 = pcVar9 + 4;
        }
        for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *pcVar9 = *pcVar12;
            pcVar12 = pcVar12 + 1;
            pcVar9 = pcVar9 + 1;
        }
        
        {
            FINDFILECONTEXT context;
            HANDLE handle = FindFile(acStack_640, &context);
            MenuSaveItem_10048880.Size = context.FileSizeLow;
            MenuSaveItem_10048880.Time = context.LastWriteTime;
            FindClose(handle);
        }

        if (MenuSaveState.Save.Unk1A4 != 0) // TODO
        {
            FUN_10018570(acStack_640, &UnkNetworkID_1004860d);
            FUN_100185f0(acStack_640);
            
            BINFILE file = { (BFH)INVALID_BINARY_FILE_VALUE };
            OpenBinFile(&file, "XCHNG\\menu.sav", BINFILEOPENTYPE_READ);
            ReadBinFile(&file, &MenuSaveState.Save, 0x1a4); // TODO doesn't match struct size. substruct?
            CloseBinFile(&file);
        }
    }
    */

    return action;
}

// 0x1000f340
VOID CLASSCALL FUN_1000f340(MULTIMAPCONTROLPTR self)
{
    // TODO NOT IMPLEMENTED
}