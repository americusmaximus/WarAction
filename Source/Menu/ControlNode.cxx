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

#include "ControlNode.hxx"

#include <stdlib.h>

VOID InitControlNode(CONTROLNODEPTR self, CONTROLPTR node)
{
    self->Value = node;
    self->Next = NULL;
}

// 0x10001a30
VOID CLASSCALL AppendControlNode(CONTROLNODEPTR self, CONTROLPTR node)
{
    CONTROLNODEPTR current = self;
    CONTROLNODEPTR next = current->Next;

    while (next != NULL)
    {
        current = next;
        next = next->Next;
    }

    next = ALLOCATE(CONTROLNODE);

    if (next == NULL) { self->Next = NULL; return; }

    next->Value = node;
    next->Next = NULL;

    self->Next = next;
}

// 0x10001a80
VOID CLASSCALL DisposeControlNode(CONTROLNODEPTR self)
{
    if (self->Value != NULL) { self->Value->Self->Release(self->Value, OBJECTRELEASETYPE_ALLOCATED); }

    if (self->Next != NULL)
    {
        DisposeControlNode(self->Next);

        free(self->Next);
    }
}