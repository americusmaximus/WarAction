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

#include "ActionArea.hxx"
#include "ControlCommand.hxx"

#include <stdlib.h>

#define MIN_ACTION_VALUE 0x4FFFFFFF
#define MAX_ACTION_VALUE 0x7FFFFFFF

ACTIONAREASTATEMODULECONTAINER ActionAreaState;

// 0x10022390
ACTIONAREAPTR AcquireActionArea(CONST U32 action)
{
    ACTIONAREAPTR item = ActionAreaState.Items;

    while (item != NULL)
    {
        if (item->Action == action) { return item; }

        item = item->Next;
    }

    return item;
}

// 0x10022430
VOID CLASSCALL SelectActionAreaDimensions(ACTIONAREAPTR self, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height)
{
    self->X = x;
    self->Y = y;
    self->Width = width;
    self->Height = height;
}

// 0x10022470
VOID ActivateActionArea(CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, CONST U32 options, CONST U32 action, CONST U32 priority)
{
    U32 id = action;

    if (id == 0)
    {
        id = MIN_ACTION_VALUE;

        while (id < MAX_ACTION_VALUE) { if (AcquireActionArea(action) != NULL) { id = id + 1; } }
    }
    else if (AcquireActionArea(action) != NULL) { DequeueControlCommand(action); }

    ACTIONAREAPTR area = ALLOCATE(ACTIONAREA);

    area->Action = id;

    area->X = x;
    area->Y = y;
    area->Width = width;
    area->Height = height;

    area->Options = options | 0x8000; // TODO
    area->Priority = priority;

    area->Next = NULL;

    EnqueueActionArea(area);
}

// 0x100223b0
VOID EnqueueActionArea(ACTIONAREAPTR area)
{
    ACTIONAREAPTR current = ActionAreaState.Items;
    ACTIONAREAPTR next = NULL;

    while (current != NULL)
    {
        ACTIONAREAPTR self = current;
        current = self;

        if (self->Priority < area->Priority) { break; }

        current = self->Next;
        next = self;
    };

    area->Next = current;

    if (next == NULL) { ActionAreaState.Items = area; return; }

    next->Next = area;
}

// 0x100223f0
VOID DequeueActionArea(ACTIONAREAPTR self)
{
    ACTIONAREAPTR current = ActionAreaState.Items;
    ACTIONAREAPTR next = NULL;

    if (current != NULL)
    {
        ACTIONAREAPTR value = current;

        while (value != self)
        {
            current = value->Next;
            next = value;

            if (value->Next == NULL) { return; }

            value = current;
        }

        if (next == NULL) { ActionAreaState.Items = value->Next; return; }

        next->Next = value->Next;
    }
}

// 0x100222e0
VOID CLASSCALL ReleaseActionArea(ACTIONAREAPTR self)
{
    ACTIONAREAPTR current = ActionAreaState.Items;
    ACTIONAREAPTR next = NULL;

    if (current != NULL)
    {
        ACTIONAREAPTR value = NULL;

        do
        {
            value = current;

            if (value == self)
            {
                if (next == NULL) { ActionAreaState.Items = self->Next; }
                else { next->Next = self->Next; }
            }

            current = value->Next;
            next = value;

        } while (value->Next != NULL);
    }
}

// 0x10022580
BOOL FUN_10022580(CONST U32 param_1, CONST U32 param_2) // TODO
{
    ACTIONAREAPTR area = AcquireActionArea(param_1);

    if (area == NULL) { return FALSE; }

    FUN_10022450(area, param_2);

    return TRUE;
}

// 0x10022450
VOID CLASSCALL FUN_10022450(ACTIONAREAPTR self, CONST U32 param_2) // TODO name
{
    self->Options = self->Options & 0xfffffe00 | param_2; // TODO
}