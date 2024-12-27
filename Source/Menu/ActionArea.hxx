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

#include "Basic.hxx"

#include <ActionArea.hxx>

typedef struct ActionAreaStateModuleContainer
{
    ACTIONAREAPTR   Items;  // 0x1005abd4
} ACTIONAREASTATEMODULECONTAINER, * ACTIONAREASTATEMODULECONTAINERPTR;

EXTERN ACTIONAREASTATEMODULECONTAINER ActionAreaState;

VOID CLASSCALL SelectActionAreaDimensions(ACTIONAREAPTR self, CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height);
VOID CLASSCALL ReleaseActionArea(ACTIONAREAPTR self);

ACTIONAREAPTR AcquireActionArea(CONST U32 action);
VOID ActivateActionArea(CONST S32 x, CONST S32 y, CONST S32 width, CONST S32 height, CONST U32 options, CONST U32 action, CONST U32 priority);
VOID DequeueActionArea(ACTIONAREAPTR self);
VOID EnqueueActionArea(ACTIONAREAPTR area);
VOID CLASSCALL SelectActionAreaAction(ACTIONAREAPTR self, CONST U32 action);
BOOL ReplaceActionAreaAction(CONST U32 current, CONST U32 action);