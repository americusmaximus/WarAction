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

typedef struct WindowContainer
{
    WINDOW                  Window;     // 0x10059380

    U32                     Ticks;      // 0x10059594

    BOOL                    IsActive;   // 0x1005abd8

    WINDOWINPUTSTATE        State;      // 0x10059bb8


    struct
    {
        U32                 Ticks;      // 0x1005abdc

        struct
        {
            S32             X;          // 0x1005abe0
            S32             Y;          // 0x1005abe4
        } Left;

        struct
        {
            S32             X;          // 0x1005abe8
            S32             Y;          // 0x1005abec
        } Right;
    } Cursor;

    U32                     Key;        // 0x1005abf0
} WINDOWCONTAINER, * WINDOWCONTAINERPTR;

EXTERN WINDOWCONTAINER WindowState;

WINDOWPTR CLASSCALL ActivateWindowState(WINDOWPTR self, ACTIONHANDLERLAMBDA initialize, ACTIONHANDLERLAMBDA execute, ACTIONHANDLERLAMBDA complete);
VOID CLASSCALL InitializeWindowState(WINDOWPTR self);

BOOL WindowMessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result);
VOID InitializeWindowActionHandler(CONST U32 priority, WINDOWACTIONHANDLERLAMBDA action);
VOID ReleaseWindowActionHandler(WINDOWACTIONHANDLERLAMBDA lambda);
VOID SelectWindowInputState(CONST WINDOWINPUTSTATE state);

BOOL IsAnsiCharacter(U8 value);
U32 AcquireUnicodeCharacter(CONST U32 value);