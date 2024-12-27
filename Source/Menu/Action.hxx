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

#include <Action.hxx>

typedef struct ActionStateModuleContainer
{
    ACTIONHANDLERPTR Activate;      // 0x1005abf4
    ACTIONHANDLERPTR Initialize;    // 0x1005abf8
    ACTIONHANDLERPTR Execute;       // 0x1005abfc
    ACTIONHANDLERPTR Release;       // 0x1005ac00
    ACTIONHANDLERPTR Message;       // 0x1005ac04
    ACTIONHANDLERPTR Active;        // 0x1005ac08
} ACTIONSTATEMODULECONTAINER, * ACTIONSTATEMODULECONTAINERPTR;

EXTERN ACTIONSTATEMODULECONTAINER ActionState;

VOID CLASSCALL ReleaseActionHandler(ACTIONHANDLERPTR self);
BOOL CLASSCALL ContainsActionHandler(ACTIONHANDLERPTR self, ACTIONHANDLERPTR handler);

VOID ReleaseActionHandler(ACTIONHANDLERPTR self, ACTIONHANDLERLAMBDA action);
VOID ReleaseWindowActionHandler(WINDOWACTIONHANDLERLAMBDA lambda);