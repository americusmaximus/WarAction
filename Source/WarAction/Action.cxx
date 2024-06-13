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
#include "State.hxx"

// 0x00402270
VOID CLASSCALL InitializeActionHandler(ACTIONHANDLERPTR self, ACTIONHANDLERPTR * destination, CONST U32 priority, ACTIONHANDLERLAMBDA action)
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
            if (first == State.Actions.Release)
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

// 0x00402770
VOID InitializeActionHandler(ACTIONHANDLERPTR* destination, CONST U32 priority, ACTIONHANDLERLAMBDA action)
{
    if (action != NULL)
    {
        ACTIONHANDLERPTR handler = (ACTIONHANDLERPTR)malloc(sizeof(ACTIONHANDLER));

        if (handler != NULL) { InitializeActionHandler(handler, destination, priority, action); }
    }
}

// 0x004027d0
VOID InitializeActionHandler(CONST U32 priority, ACTIONHANDLERLAMBDA action)
{
    InitializeActionHandler(&State.Actions.Action, priority, action);
}

// 0x00402770
VOID InitializeWindowActionHandler(CONST U32 priority, WINDOWACTIONHANDLERLAMBDA action)
{
    InitializeActionHandler(&State.Actions.Message, priority, (ACTIONHANDLERLAMBDA)action);
}

// 0x00402390
BOOL ContainsWindowStateHandler(ACTIONHANDLERPTR self, ACTIONHANDLERPTR handler)
{
    while (handler != NULL)
    {
        if (self == handler) { return TRUE; }

        handler = handler->Next;
    }

    return FALSE;
}

// 0x004022d0
VOID CLASSCALL ReleaseActionHandler(ACTIONHANDLERPTR self)
{
    if (State.Actions.Active == self) { State.Actions.Active = self->Next; }

    ACTIONHANDLERPTR* destination = NULL;

    if (ContainsWindowStateHandler(self, State.Actions.Activate)) { destination = &State.Actions.Activate; }
    else if (ContainsWindowStateHandler(self, State.Actions.Initialize)) { destination = &State.Actions.Initialize; }
    else if (ContainsWindowStateHandler(self, State.Actions.Action)) { destination = &State.Actions.Action; }
    else if (ContainsWindowStateHandler(self, State.Actions.Release)) { destination = &State.Actions.Release; }
    else if (ContainsWindowStateHandler(self, State.Actions.Message)) { destination = &State.Actions.Message; }
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

// 0x004027a0
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

// 0x004027f0
VOID ReleaseActionHandler(ACTIONHANDLERLAMBDA lambda)
{
    ReleaseActionHandler(State.Actions.Action, lambda);
}

// 0x00402830
VOID ReleaseWindowActionHandler(WINDOWACTIONHANDLERLAMBDA lambda)
{
    ReleaseActionHandler(State.Actions.Message, (ACTIONHANDLERLAMBDA)lambda);
}