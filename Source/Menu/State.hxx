/*
Copyright (c) 2024 - 2025 Americus Maximus

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

#include "MainControl.hxx"
#include "Window.hxx"

#include <App.hxx>

typedef struct StateModuleContainer
{
    APPPTR                          App;                            // 0x10046b58

    struct
    {
        LPSTR                       All;                            // 0x1005ae5c
        LPSTR*                      Args;                           // 0x1005ae60
        U32                         Count;                          // 0x1005ae64
    } Arguments;

    BOOL IsIniActive;                                               // 0x100470bc

    MODULEPTR                       Module;                         // 0x10059500
    RENDERERPTR                     Renderer;                       // 0x100596d8
    WINDOWPTR                       Window;                         // 0x1005ac0c
    LOGGERPTR                       Logger;                         // 0x1005ac10

    MAINCONTROLPTR                  Main;                           // 0x10059bac

    CHAR                            Name[MAX_PLAYER_NAME_LENGTH];   // 0x10048860

    MAPFILEINFO                     Map;                            // 0x10048880
} STATEMODULECONTAINER, * STATEMODULECONTAINERPTR;

EXTERN STATEMODULECONTAINER State;