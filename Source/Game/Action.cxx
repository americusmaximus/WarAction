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

#include "Action.hxx"

#include <stdlib.h>

ACTIONSTATEMODULECONTAINER ActionState;

// 0x1008fbd0
VOID CLASSCALL InitializeActionHandler(ACTIONHANDLERPTR self, ACTIONHANDLERPTR* destination, CONST U32 priority, ACTIONHANDLERLAMBDA action)
{
    self->Action = action;
    self->Priority = priority;

    ACTIONHANDLERPTR first = *destination;
    ACTIONHANDLERPTR current = NULL;
    ACTIONHANDLERPTR next = first;

    if (first != NULL)
    {
        ACTIONHANDLERPTR following = NULL;

        do
        {
            if (first == ActionState.Release)
            {
                if (next->Priority < priority) { break; }
            }
            else if (priority < next->Priority) { break; }

            following = next->Next;
            current = next;
            next = following;
        } while (following != NULL);

        if (current != NULL)
        {
            current->Next = self;

            self->Next = next;

            return;
        }
    }

    *destination = self;

    self->Next = next;
}

// 0x1008fc30
VOID CLASSCALL ReleaseActionHandler(ACTIONHANDLERPTR self)
{
    if (ActionState.Active == self) { ActionState.Active = self->Next; }

    ACTIONHANDLERPTR* destination = NULL;

    if (ContainsActionHandler(self, ActionState.Activate)) { destination = &ActionState.Activate; }
    else if (ContainsActionHandler(self, ActionState.Initialize)) { destination = &ActionState.Initialize; }
    else if (ContainsActionHandler(self, ActionState.Execute)) { destination = &ActionState.Execute; }
    else if (ContainsActionHandler(self, ActionState.Release)) { destination = &ActionState.Release; }
    else if (ContainsActionHandler(self, ActionState.Message)) { destination = &ActionState.Message; }
    else { return; }

    ACTIONHANDLERPTR current = *destination;

    if (current != self)
    {
        if (current != NULL)
        {
            ACTIONHANDLERPTR next = NULL;

            while (next = current->Next, next != self)
            {
                current = next;

                if (next == NULL) { return; }
            }

            current->Next = self->Next;
        }

        return;
    }

    *destination = self->Next;
}

// 0x1008fcf0
BOOL CLASSCALL ContainsActionHandler(ACTIONHANDLERPTR self, ACTIONHANDLERPTR handler)
{
    while (handler != NULL)
    {
        if (self == handler) { return TRUE; }

        handler = handler->Next;
    }

    return FALSE;
}

// 0x1008fd40
VOID ReleaseActionHandler(ACTIONHANDLERPTR self, ACTIONHANDLERLAMBDA action)
{
    if (self != NULL)
    {
        while (self->Action != action)
        {
            self = self->Next;

            if (self == NULL) { return; }
        }

        ReleaseActionHandler(self);

        free(self);
    }
}

// 0x1008fd10
VOID InitializeActionHandler(ACTIONHANDLERPTR* destination, CONST U32 priority, ACTIONHANDLERLAMBDA action)
{
    if (action != NULL)
    {
        ACTIONHANDLERPTR handler = ALLOCATE(ACTIONHANDLER);

        if (handler != NULL) { InitializeActionHandler(handler, destination, priority, action); }
    }
}