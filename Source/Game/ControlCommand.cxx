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

#include "ActionArea.hxx"
#include "Control.hxx"
#include "ControlCommand.hxx"

#include <stdlib.h>

CONTROLCOMMANDSTATEMODULECONTAINER CommandControlState;

// 0x1008e7d0
VOID EnqueueControlCommand(CONST U32 command, CONST U32 action, CONST U32 param1, CONST U32 param2)
{
    CommandControlState.Items[CommandControlState.WriteIndex].Command = command;
    CommandControlState.Items[CommandControlState.WriteIndex].Action = action;
    CommandControlState.Items[CommandControlState.WriteIndex].Parameter1 = param1;
    CommandControlState.Items[CommandControlState.WriteIndex].Parameter2 = param2;

    CommandControlState.WriteIndex = (CommandControlState.WriteIndex + 1) % MAX_CONTROL_COMMAND_ITEMS_COUNT;
}

// 0x1008e840
BOOL DequeueControlCommand(CONTROLCOMMANDPTR command, CONST BOOL remove)
{
    while (CommandControlState.WriteIndex != CommandControlState.ReadIndex)
    {
        if (CommandControlState.Items[CommandControlState.ReadIndex].Command != CONTROLACTION_UNKNOWN)
        {
            command->Command = CommandControlState.Items[CommandControlState.ReadIndex].Command;
            command->Action = CommandControlState.Items[CommandControlState.ReadIndex].Action;
            command->Parameter1 = CommandControlState.Items[CommandControlState.ReadIndex].Parameter1;
            command->Parameter2 = CommandControlState.Items[CommandControlState.ReadIndex].Parameter2;

            if (remove)
            {
                CommandControlState.ReadIndex = (CommandControlState.ReadIndex + 1) % MAX_CONTROL_COMMAND_ITEMS_COUNT;
            }

            return TRUE;
        }

        CommandControlState.ReadIndex = (CommandControlState.ReadIndex + 1) % MAX_CONTROL_COMMAND_ITEMS_COUNT;
    }

    return FALSE;
}