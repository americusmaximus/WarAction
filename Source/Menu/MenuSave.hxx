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

// TODO should be part of network?

typedef struct MenuSaveItem // TODO name
{
    DWORD       Size;
    FILETIME    Time;
    CHAR        Name[MAX_FILE_NAME_LENGTH];
} MENUSAVEITEM, * MENUSAVEITEMPTR; // TODO name

typedef struct MenuSaveFileContent /* TOOD better name*/
// TODO reads/rites are 0x1a4, which is smalled that whole stuct. Make a substruct?
{
    U32 Unk00; // TODO
    U32 Unk01; // TODO
    U32 Unk02; // TODO
    U32 Unk03; // TODO
    U32 Unk04; // TODO
    U32 Unk05; // TODO
    U32 Ticks;
    U32 MaxDelay;
    U8 Unk08; // TODO
    U8 Unk09[3]; // TODO
    // TODO Players
    U32 Unk196; // TODO
    U32 Unk19a; // TODO
    U32 Unk19e; // TODO
    U16 Unk1a2; //TODO
    U8 Unk1A4; // TODO
    CHAR UnkXXXXX[59]; // TODO

} MENUSAVEFILECONTENT, * MENUSAVEFILECONTENTPTR;

typedef struct MenuSaveStateModuleContainer
{
    MENUSAVEFILECONTENT Save; // 0x10048621
} MENUSAVESTATEMODULECONTAINER, * MENUSAVESTATEMODULECONTAINERPTR;

EXTERN MENUSAVESTATEMODULECONTAINER MenuSaveState;

// 0x10048880
EXTERN MenuSaveItem MenuSaveItem_10048880; // TODO make part of the state