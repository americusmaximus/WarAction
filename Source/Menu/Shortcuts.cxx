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

#include "Lock.hxx"
#include "Shortcuts.hxx"

#include <stdlib.h>

SHORTCUTSTATEMODULECONTAINER ShortcutsState;

// 0x10007110
SHORTCUTSTATEPTR CLASSCALL ActivateShortcuts(SHORTCUTSTATEPTR self)
{
    self->Unk00 = 0; // TODO
    self->Unk01 = 0; // TODO
    self->Unk05 = 0; // TODO

    LOCK lock;
    ActivateLock(&lock);

    if (ShortcutsState.Items == NULL)
    {
        ShortcutsState.Items = ALLOCATE(SHORTCUT);

        ShortcutsState.Items[0].Unk00 = NULL;
        ShortcutsState.Items[0].Unk01 = NULL;
        ShortcutsState.Items[0].Unk02 = NULL;
        ShortcutsState.Items[0].Unk05 = 1; // TODO
    }

    ShortcutsState.Count = ShortcutsState.Count + 1;

    ReleaseLock(&lock);

    self->Unk09 = 0; // TODO

    SHORTCUTPTR shortcut = ALLOCATE(SHORTCUT);

    shortcut->Unk01 = ShortcutsState.Items;
    shortcut->Unk05 = 0; // TODO
    shortcut->Unk00 = shortcut;

    self->Items = shortcut;
    self->Items->Unk02 = self->Items;

    return self;
}

// 0x100071c0
VOID CLASSCALL InitializeShortcuts(SHORTCUTSTATEPTR self)
{
    // TODO NOT IMPLEMENTED
}

// 0x10009d40
S32 CLASSCALL AcquireShortcut(SHORTCUTSTATEPTR self, CONST U32 action)
{
    // TODO NOT IMPLEMENTED
    /*Shortcut** ppSVar1;
    Shortcut* local_8;
    Shortcut* local_4;

    local_8 = FUN_1000b700(self, &action);
    local_4 = self->Items;

    if ((local_8 == local_4) || (action < (int)local_8->Unk03)) {
        ppSVar1 = &local_4;
    }
    else {
        ppSVar1 = &local_8;
    }
    if (*ppSVar1 != local_4) {
        return (*ppSVar1)->Unk04;
    }*/

    return 0; // TODO
}

// 0x1000b700
SHORTCUTPTR CLASSCALL FUN_1000b700(SHORTCUTSTATEPTR self, int* param_2)
{
    Shortcut* pSVar1;
    Shortcut* pSVar2;
    Shortcut* pSVar3;

    pSVar2 = self->Items;
    if (pSVar2->Unk01 != ShortcutsState.Items) {
        pSVar1 = pSVar2;
        pSVar3 = pSVar2->Unk01;
        do {
            pSVar2 = pSVar3;
            if ((int)pSVar2->Unk03 < *param_2) {
                pSVar3 = pSVar2->Unk02;
                pSVar2 = pSVar1;
            }
            else {
                pSVar3 = pSVar2->Unk00;
            }
            pSVar1 = pSVar2;
        } while (pSVar3 != ShortcutsState.Items);
    }

    return pSVar2;
}

// 0x1000bce0
VOID CLASSCALL FUN_1000bce0(SHORTCUTPTR self) // TODO
{
    SHORTCUTPTR pSVar1; // TODO

    SHORTCUTPTR pSVar2 = self->Unk00->Unk02; // TODO
    if (pSVar2 != ShortcutsState.Items)
    {
        for (pSVar1 = pSVar2->Unk00; pSVar1 != ShortcutsState.Items; pSVar1 = pSVar1->Unk00)
        {
            pSVar2 = pSVar1;
        }

        self->Unk00 = pSVar2;
        return;
    }

    pSVar2 = self->Unk00->Unk01;

    if (self->Unk00 == pSVar2->Unk02)
    {
        do {
            self->Unk00 = pSVar2;
            pSVar2 = pSVar2->Unk01;
        } while (self->Unk00 == pSVar2->Unk02);
    }

    if (self->Unk00->Unk02 != pSVar2) { self->Unk00 = pSVar2; }
}