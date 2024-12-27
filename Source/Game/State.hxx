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

#include <App.hxx>

typedef struct StateModuleContainer
{
    APPPTR                          App;                            // 0x10294d34

    BOOL IsIniActive;                                               // 0x10349724

    SOUNDPTR                        Sound;                          // 0x1038446c
    RENDERERPTR                     Renderer;                       // 0x10384474
    MODULEPTR                       Module;                         // 0x10384478
    
    WINDOWPTR                       Window;                         // 0x103854d4
    LOGGERPTR                       Logger;                         // 0x103854d8
} STATEMODULECONTAINER, * STATEMODULECONTAINERPTR;

EXTERN STATEMODULECONTAINER State;