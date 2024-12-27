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

#include "Module.hxx"
#include "Sound.hxx"
#include "State.hxx"
#include "Window.hxx"

#define MAX_MODULE_FILE_NAME_LENGTH 512

// 0x10065090
VOID AcquireIniFileState(VOID)
{
    State.IsIniActive = GetPrivateProfileIntA("debug", "useinifile", FALSE, State.App->Ini);
}

// 0x1008e390
BOOL InitializeModuleAction(VOID)
{
    State.Module = State.App->Module;
    State.Renderer = State.Module->Renderer;
    State.Sound = State.Module->Sound;

    AcquireIniFileState();

    State.Module->Handle = NULL;

    SoundState.Sound = State.Sound;

    State.Renderer->Actions.Initialize();

    if (!State.Renderer->Actions.InitializeWindow(State.Renderer->Surface.Width, State.Renderer->Surface.Height)) { return FALSE; }

    SelectWindowInputState((WINDOWINPUTSTATE)(WINDOWINPUTSTATE_ACCEPT_KEYBOARD | WINDOWINPUTSTATE_ACCEPT_MOUSE));

    if (!FUN_1008e320()) { return FALSE; }

    InitializeExecuteActionHandler(DEFAULT_ACTION_HANDLER_PRIORITY, FUN_1008e4c0);

    return TRUE;
}

// 0x10083eb0
BOOL FUN_10083eb0(VOID) // TODO
{
    // TODO NOT IMPLEMENTED

    return TRUE; // TODO
}

// 0x10084390
BOOL FUN_10084390(VOID) // TODO
{
    // TODO NOT IMPLEMENTED

    return TRUE; // TODO
}

// 0x1008e120
BOOL FUN_1008e120(VOID) // TODO
{
    // TODO NOT IMPLEMENTED

    return TRUE; // TODO
}

// 0x1008e320
BOOL FUN_1008e320(VOID) // TODO
{
    if (State.Module->Game.Unk07 == 0) { return FUN_1008e120(); } // TODO

    if (State.Module->Game.IsNetwork) { return FUN_10083eb0(); }

    return FUN_10084390();
}

// 0x1008e4c0
BOOL FUN_1008e4c0(VOID)
{
    // TODO NOT IMPLEMENTED

    return TRUE;
}

// 0x1008fd70
VOID InitializeExecuteActionHandler(CONST U32 priority, ACTIONHANDLERLAMBDA action)
{
    InitializeActionHandler(&ActionState.Execute, priority, action);
}

// 0x1008fd90
VOID ReleaseExecuteActionHandler(ACTIONHANDLERLAMBDA action)
{
    ReleaseActionHandler(ActionState.Execute, action);
}