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

#include "Control.hxx"
#include "TextAsset.hxx"

struct GreetingsControl;

typedef CONTROLTYPE(CLASSCALL* GREETINGSCONTROLTYPEACTION)(GreetingsControl* self);
typedef VOID(CLASSCALL* GREETINGSCONTROLINITIALIZEACTION)(GreetingsControl* self);
typedef VOID(CLASSCALL* GREETINGSCONTROLDISABLEACTION)(GreetingsControl* self);
typedef VOID(CLASSCALL* GREETINGSCONTROLTICKACTION)(GreetingsControl* self);
typedef U32(CLASSCALL* GREETINGSCONTROLACTIONACTION)(GreetingsControl* self);
typedef GreetingsControl* (CLASSCALL* GREETINGSCONTROLRELEASEACTION)(GreetingsControl* self, CONST OBJECTRELEASETYPE mode);

// INHERITANCE: ControlSelf
typedef struct GreetingsControlSelf
{
    GREETINGSCONTROLTYPEACTION          Type;
    GREETINGSCONTROLINITIALIZEACTION    Initialize;
    GREETINGSCONTROLDISABLEACTION       Disable;
    GREETINGSCONTROLTICKACTION          Tick;
    GREETINGSCONTROLACTIONACTION        Action;
    GREETINGSCONTROLRELEASEACTION       Release;
} GREETINGSCONTROLSELF, * GREETINGSCONTROLSELFPTR;

EXTERN GREETINGSCONTROLSELF GreetingsControlSelfState;

typedef enum GreetingsType
{
    GREETINGSTYPE_NORMAL        = 0,
    GREETINGSTYPE_STARWARZ      = 1,
    GREETINGSTYPE_FORCE_DWORD   = 0x7FFFFFF
} GREETINGSTYPE, * GREETINGSTYPEPTR;

#pragma pack(push, 1)
// INHERITANCE: Control
typedef struct GreetingsControl
{
    GREETINGSCONTROLSELFPTR     Self;
    U8                          IsActive;
    TEXTASSET                   Text;
    U32                         Ticks;
    GREETINGSTYPE               Type;
    U32                         Item;
} GREETINGSCONTROL, * GREETINGSCONTROLPTR;
#pragma pack(pop)

GREETINGSCONTROLPTR CLASSCALL ActivateGreetingsControl(GREETINGSCONTROLPTR self);
VOID CLASSCALL InitializeGreetingsControl(GREETINGSCONTROLPTR self);
VOID CLASSCALL DisableGreetingsControl(GREETINGSCONTROLPTR self);
VOID CLASSCALL TickGreetingsControl(GREETINGSCONTROLPTR self);
U32 CLASSCALL ActionGreetingsControl(GREETINGSCONTROLPTR self);
GREETINGSCONTROLPTR CLASSCALL ReleaseGreetingsControl(GREETINGSCONTROLPTR self, CONST OBJECTRELEASETYPE mode);
VOID CLASSCALL DisposeGreetingsControl(GREETINGSCONTROLPTR self);