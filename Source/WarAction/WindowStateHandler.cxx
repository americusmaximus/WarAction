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

#define INITIALIZEWINDOWSTATEHANDLER(I, A)                                                                                          \
    if (A != NULL) {                                                                                                                \
        LPWINDOWSTATECONTAINERHANDLER handler = (LPWINDOWSTATECONTAINERHANDLER)malloc(sizeof(WINDOWSTATECONTAINERHANDLER));         \
        if (handler != NULL) {                                                                                                      \
            InitializeWindowStateHandler(handler, &State.Window.Handlers[I], WINDOW_STATE_HANDLER_DEFAULT_PRIORITY_VALUE, A);       \
        }                                                                                                                           \
    }

// 0x00402270
VOID InitializeWindowStateHandler(LPWINDOWSTATECONTAINERHANDLER self, LPWINDOWSTATECONTAINERHANDLER* destination, U32 priority, WINDOWSTATEHANDLERLAMBDA lambda)
{
    self->Invoke = lambda;
    self->Priority = priority;

    LPWINDOWSTATECONTAINERHANDLER first = *destination;
    LPWINDOWSTATECONTAINERHANDLER current = NULL;
    LPWINDOWSTATECONTAINERHANDLER next = first;

    if (first != NULL)
    {
        LPWINDOWSTATECONTAINERHANDLER following = NULL;

        do
        {
            if (first == State.Window.Handlers[WINDOW_STATE_RELEASE_HANDLER_INDEX])
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
VOID InitializeWindowStateHandler(LPWINDOWSTATECONTAINERHANDLER* destination, U32 priority, WINDOWSTATEHANDLERLAMBDA lambda)
{
    if (lambda != NULL)
    {
        LPWINDOWSTATECONTAINERHANDLER handler = (LPWINDOWSTATECONTAINERHANDLER)malloc(sizeof(WINDOWSTATECONTAINERHANDLER));

        if (handler != NULL)
        {
            InitializeWindowStateHandler(handler, destination, priority, lambda);
        }
    }
}

// 0x004027d0
VOID InitializeWindowStateActionHandler(U32 priority, WINDOWSTATEHANDLERLAMBDA lambda)
{
    InitializeWindowStateHandler(&State.Window.Handlers[WINDOW_STATE_ACTION_HANDLER_INDEX], priority, lambda);
}

// 0x00402770
VOID InitializeWindowStateMessageHandler(U32 priority, WINDOWSTATEMESSAGEHANDLERLAMBDA lambda)
{
    InitializeWindowStateHandler(&State.Window.Handlers[WINDOW_STATE_MESSAGE_HANDLER_INDEX], priority, (WINDOWSTATEHANDLERLAMBDA)lambda);
}

// 0x00402390
BOOL IsContainsWindowStateHandler(LPWINDOWSTATECONTAINERHANDLER self, LPWINDOWSTATECONTAINERHANDLER handler)
{
    while (handler != NULL)
    {
        if (self == handler) { return TRUE; }

        handler = handler->Next;
    }

    return FALSE;
}

// 0x004022d0
VOID ReleaseWindowStateHandler(LPWINDOWSTATECONTAINERHANDLER self)
{
    if (State.Window.ActiveHandler == self) { State.Window.ActiveHandler = self->Next; }

    LPWINDOWSTATECONTAINERHANDLER* destination = NULL;

    if (IsContainsWindowStateHandler(self, State.Window.Handlers[WINDOW_STATE_START_HANDLER_INDEX])) { destination = &State.Window.Handlers[WINDOW_STATE_START_HANDLER_INDEX]; }
    else if (IsContainsWindowStateHandler(self, State.Window.Handlers[WINDOW_STATE_INITIALIZE_HANDLER_INDEX])) { destination = &State.Window.Handlers[WINDOW_STATE_INITIALIZE_HANDLER_INDEX]; }
    else if (IsContainsWindowStateHandler(self, State.Window.Handlers[WINDOW_STATE_ACTION_HANDLER_INDEX])) { destination = &State.Window.Handlers[WINDOW_STATE_ACTION_HANDLER_INDEX]; }
    else if (IsContainsWindowStateHandler(self, State.Window.Handlers[WINDOW_STATE_RELEASE_HANDLER_INDEX])) { destination = &State.Window.Handlers[WINDOW_STATE_RELEASE_HANDLER_INDEX]; }
    else if (IsContainsWindowStateHandler(self, State.Window.Handlers[WINDOW_STATE_MESSAGE_HANDLER_INDEX])) { destination = &State.Window.Handlers[WINDOW_STATE_MESSAGE_HANDLER_INDEX]; }
    else { return; }

    LPWINDOWSTATECONTAINERHANDLER current = *destination;

    if (current != self)
    {
        if (current != NULL)
        {
            LPWINDOWSTATECONTAINERHANDLER next = NULL;

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
VOID ReleaseWindowStateHandler(LPWINDOWSTATECONTAINERHANDLER self, WINDOWSTATEHANDLERLAMBDA lambda)
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
VOID ReleaseWindowStateActionHandler(WINDOWSTATEHANDLERLAMBDA lambda)
{
    ReleaseWindowStateHandler(State.Window.Handlers[WINDOW_STATE_ACTION_HANDLER_INDEX], lambda);
}

VOID ReleaseWindowStateMessageHandler(WINDOWSTATEMESSAGEHANDLERLAMBDA lambda)
{
    ReleaseWindowStateHandler(State.Window.Handlers[WINDOW_STATE_MESSAGE_HANDLER_INDEX], (WINDOWSTATEHANDLERLAMBDA)lambda);
}