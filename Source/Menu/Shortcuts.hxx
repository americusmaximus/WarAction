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

#pragma once

#include "Basic.hxx"

#define SHORTCUT_NONE   0

#pragma pack(push, 1)
typedef struct Shortcut
{
    Shortcut*   Unk00; // TODO
    Shortcut*   Unk01; // TODO
    Shortcut*   Unk02; // TODO
    U32         Unk03; // TODO
    U32         Unk04; // TODO
    U32         Unk05; // TODO
} SHORTCUT, * SHORTCUTPTR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ShortcutState
{
    U8          Unk00; // TODO
    U8          Unk01; // TODO
    U8          Unk02; // TODO
    U8          Unk03; // TODO
    SHORTCUTPTR Items; // TODO
    U8          Unk05; // TODO
    U8          Unk06; // TODO
    U8          Unk07; // TODO
    U8          Unk08; // TODO
    U32         Unk09; // TODO
} SHORTCUTSTATE, * SHORTCUTSTATEPTR;
#pragma pack(pop)

typedef struct ShortcutStateModuleContainer
{
    SHORTCUTSTATE   State;  // 0x100462a0
    U32             Count;  // 0x100462b0
    SHORTCUTPTR     Items;  // 0x100462b4
} SHORTCUTSTATEMODULECONTAINER, * SHORTCUTSTATEMODULECONTAINERPTR;

EXTERN SHORTCUTSTATEMODULECONTAINER ShortcutsState;

SHORTCUTSTATEPTR CLASSCALL ActivateShortcuts(SHORTCUTSTATEPTR self); // TODO name
VOID CLASSCALL InitializeShortcuts(SHORTCUTSTATEPTR self);

VOID CLASSCALL FUN_1000bce0(SHORTCUTPTR self); // TODO Name
S32 CLASSCALL AcquireShortcut(SHORTCUTSTATEPTR self, CONST U32 action); // TODO return type