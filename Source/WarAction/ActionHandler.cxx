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

#include "State.hxx"

// 0x00402270
VOID InitializeActionHandler(LPACTIONHANDLER self, LPACTIONHANDLER * destination, U32 priority, ACTIONHANDLERLAMBDA lambda)
{
    self->Invoke = lambda;
    self->Priority = priority;

    LPACTIONHANDLER first = *destination;
    LPACTIONHANDLER current = NULL;
    LPACTIONHANDLER next = first;

    if (first != NULL)
    {
        LPACTIONHANDLER following = NULL;

        do
        {
            if (first == State.Handlers.Release)
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
VOID InitializeActionHandler(LPACTIONHANDLER* destination, U32 priority, ACTIONHANDLERLAMBDA lambda)
{
    if (lambda != NULL)
    {
        LPACTIONHANDLER handler = (LPACTIONHANDLER)malloc(sizeof(ACTIONHANDLER));

        if (handler != NULL) { InitializeActionHandler(handler, destination, priority, lambda); }
    }
}

// 0x004027d0
VOID InitializeActionHandler(U32 priority, ACTIONHANDLERLAMBDA lambda)
{
    InitializeActionHandler(&State.Handlers.Action, priority, lambda);
}

// 0x00402770
VOID InitializeWindowActionHandler(U32 priority, WINDOWACTIONHANDLERLAMBDA lambda)
{
    InitializeActionHandler(&State.Handlers.Message, priority, (ACTIONHANDLERLAMBDA)lambda);
}

// 0x00402390
BOOL ContainsWindowStateHandler(LPACTIONHANDLER self, LPACTIONHANDLER handler)
{
    while (handler != NULL)
    {
        if (self == handler) { return TRUE; }

        handler = handler->Next;
    }

    return FALSE;
}

// 0x004022d0
VOID ReleaseWindowStateHandler(LPACTIONHANDLER self)
{
    if (State.Handlers.Active == self) { State.Handlers.Active = self->Next; }

    LPACTIONHANDLER* destination = NULL;

    if (ContainsWindowStateHandler(self, State.Handlers.Activate)) { destination = &State.Handlers.Activate; }
    else if (ContainsWindowStateHandler(self, State.Handlers.Initialize)) { destination = &State.Handlers.Initialize; }
    else if (ContainsWindowStateHandler(self, State.Handlers.Action)) { destination = &State.Handlers.Action; }
    else if (ContainsWindowStateHandler(self, State.Handlers.Release)) { destination = &State.Handlers.Release; }
    else if (ContainsWindowStateHandler(self, State.Handlers.Message)) { destination = &State.Handlers.Message; }
    else { return; }

    LPACTIONHANDLER current = *destination;

    if (current != self)
    {
        if (current != NULL)
        {
            LPACTIONHANDLER next = NULL;

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
VOID ReleaseActionHandler(LPACTIONHANDLER self, ACTIONHANDLERLAMBDA lambda)
{
    if (self != NULL)
    {
        while (self->Invoke != lambda)
        {
            self = self->Next;

            if (self == NULL) { return; }
        }

        ReleaseWindowStateHandler(self);

        free(self);
    }
}

// 0x004027f0
VOID ReleaseActionHandler(ACTIONHANDLERLAMBDA lambda)
{
    ReleaseActionHandler(State.Handlers.Action, lambda);
}

// 0x00402830
VOID ReleaseWindowActionHandler(WINDOWACTIONHANDLERLAMBDA lambda)
{
    ReleaseActionHandler(State.Handlers.Message, (ACTIONHANDLERLAMBDA)lambda);
}