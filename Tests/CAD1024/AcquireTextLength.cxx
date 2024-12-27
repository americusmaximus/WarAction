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

#include "AcquireTextLength.hxx"
#include "File.hxx"

#include <stdio.h>

#define MAX_TEXT_LENGTH 128

static VOID Execute(RENDERERPTR state, MODULEEVENTPTR event, BINASSETCOLLECTIONCONTENTPTR asset, LPSTR text, U32 width)
{
    event->Result = state->Actions.AcquireTextLength(text, asset) == width;
}

#define EXECUTE(A, S, E, F, T, W) { E->Action = A; Execute(S, E, F, T, W); if (!E->Result) { return; } }

VOID AcquireTextLength(RENDERERPTR state, MODULEEVENTPTR event)
{
    // Initialize.
    BINASSETCOLLECTIONCONTENTPTR font = NULL;
    if (!AcquireFile("..\\..\\..\\..\\Content\\TXTA.PCK", (LPVOID*)&font)) { event->Result = FALSE; return; }

    {
        CHAR message[MAX_TEXT_LENGTH];
        strcpy_s(message, MAX_TEXT_LENGTH, "A");
        EXECUTE("A", state, event, font, message, 7);
    }

    {
        CHAR message[MAX_TEXT_LENGTH];
        strcpy_s(message, MAX_TEXT_LENGTH, "Hello!");
        EXECUTE("Hello!", state, event, font, message, 37);
    }

    {
        CHAR message[MAX_TEXT_LENGTH];
        strcpy_s(message, MAX_TEXT_LENGTH, "Hello, World!");
        EXECUTE("Hello, World!", state, event, font, message, 82);
    }

    // Finalize.
    free(font);
}
