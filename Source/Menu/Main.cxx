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

#include "Activate.hxx"
#include "Logger.hxx"
#include "State.hxx"
#include "Version.hxx"

// 0x1000d0a0
BOOL APIENTRY DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { Activate(); }

    return TRUE;
}

// 0x10020c30
// a.k.a. GetGeckVersion
U32 AcquireModuleVersion() { return MODULE_VERSION_VALUE; }

// 0x1000cf20
// a.k.a. VModule_Init
BOOL InitializeModule(APPPTR state)
{
    State.App = state;
    State.Window = state->Window;
    State.Logger = state->Logger;

    state->ModuleName = "SueMenu: ";

    for (ActionState.Active = ActionState.Activate;
        ActionState.Active != NULL; ActionState.Active = ActionState.Active->Next)
    {
        if (!INVOKEACTIONHANDLERLAMBDA(ActionState.Active->Action)) { return FALSE; }

        if (ActionState.Active->Next == NULL) { break; }
    }

    for (ActionState.Active = ActionState.Initialize;
        ActionState.Active != NULL; ActionState.Active = ActionState.Active->Next)
    {
        if (!INVOKEACTIONHANDLERLAMBDA(ActionState.Active->Action)) { return FALSE; }

        if (ActionState.Active->Next == NULL) { break; }
    }

    LogMessage("Started.\n");

    return TRUE;
}

// 0x1000d040
// a.k.a. VModule_Handle
BOOL MessageModule(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, LRESULT* result)
{
    for (ActionState.Active = ActionState.Message;
        ActionState.Active != NULL; ActionState.Active = ActionState.Active->Next)
    {
        if (!INVOKEWINDOWACTIONHANDLERLAMBDA(ActionState.Active->Action, hwnd, msg, wp, lp, result)) { return *result; }

        if (ActionState.Active->Next == NULL) { break; }
    }

    return TRUE;
}

// 0x1000cfc0
// a.k.a. VModule_Play
BOOL ExecuteModule()
{
    for (ActionState.Active = ActionState.Execute;
        ActionState.Active != NULL; ActionState.Active = ActionState.Active->Next)
    {
        if (!INVOKEACTIONHANDLERLAMBDA(ActionState.Active->Action)) { return FALSE; }

        if (ActionState.Active->Next == NULL) { break; }
    }

    return TRUE;
}

// 0x1000d000
// a.k.a. VModule_Done
BOOL ReleaseModule()
{
    for (ActionState.Active = ActionState.Release;
        ActionState.Active != NULL; ActionState.Active = ActionState.Active->Next)
    {
        if (!INVOKEACTIONHANDLERLAMBDA(ActionState.Active->Action)) { return FALSE; }

        if (ActionState.Active->Next == NULL) { break; }
    }

    LogMessage("Finished.\n");

    return TRUE;
}