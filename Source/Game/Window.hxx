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

#include "Action.hxx"

#include <Window.hxx>

typedef struct WindowStateModuleContainer
{
    WINDOW                  Window;     // TODO

    U32                     Ticks;      // TODO

    BOOL                    IsActive;   // 0x103854a0

    WINDOWINPUTSTATE        State;      // 0x10384480


    struct
    {
        U32                 Ticks;      // 0x103854a4

        struct
        {
            S32             X;          // 0x103854a8
            S32             Y;          // 0x103854ac
        } Left;

        struct
        {
            S32             X;          // 0x103854b0
            S32             Y;          // 0x103854b4
        } Right;
    } Cursor;

    U32                     Key;        // 0x103854b8
} WINDOWSTATEMODULECONTAINER, * WINDOWSTATEMODULECONTAINERPTR;

EXTERN WINDOWSTATEMODULECONTAINER WindowState;

WINDOWPTR CLASSCALL ActivateWindowState(WINDOWPTR self, ACTIONHANDLERLAMBDA initialize, ACTIONHANDLERLAMBDA execute, ACTIONHANDLERLAMBDA complete);
VOID CLASSCALL InitializeWindowState(WINDOWPTR self);

BOOL WindowMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result);
VOID InitializeWindowActionHandler(CONST U32 priority, WINDOWACTIONHANDLERLAMBDA action);
VOID ReleaseWindowActionHandler(WINDOWACTIONHANDLERLAMBDA lambda);
VOID SelectWindowInputState(CONST WINDOWINPUTSTATE state);

BOOL IsAnsiCharacter(U8 value);
U32 AcquireUnicodeCharacter(CONST U32 value);