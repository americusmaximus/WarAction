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
#include "ControlCommand.hxx"
#include "Cursor.hxx"

CURSORSTATEMODULECONTAINER CursorState;

// 0x1008e8d0
VOID CursorMessageHandler(CONST U32 action)
{
    for (ACTIONAREAPTR area = ActionAreaState.Items; area != NULL; area = area->Next)
    {
        if (!(area->Options & CONTROLCOMMANDACTION_IGNORE)
            && area->X <= CursorState.X && area->Y <= CursorState.Y
            && CursorState.X < area->Width + area->X && CursorState.Y < area->Height + area->Y
            && (action & area->Options))
        {
            EnqueueControlCommand(area->Action, action, CursorState.X - area->X, CursorState.Y - area->Y);

            if (area->Options & CONTROLCOMMANDACTION_SCROLL) { return; }
        }
    }
}

// 0x1008e960
VOID SelectCursorCoordinates(CONST S32 ox, CONST S32 oy, CONST S32 nx, CONST S32 ny)
{
    for (ACTIONAREAPTR area = ActionAreaState.Items; area != NULL; area = area->Next)
    {
        if (!(area->Options & CONTROLCOMMANDACTION_IGNORE)
            && (nx < area->X || ny < area->Y || area->Width + area->X <= nx || area->Height + area->Y <= ny)
            && area->X <= ox && area->Y <= oy && ox < area->Width + area->X && oy < area->Height + area->Y
            && (area->Options & CONTROLCOMMANDACTION_MOUSE_LEAVE))
        {
            EnqueueControlCommand(area->Action, CONTROLCOMMANDACTION_MOUSE_LEAVE, nx - area->X, ny - area->Y);
        }
    }

    BOOL accept = TRUE;
    for (ACTIONAREAPTR area = ActionAreaState.Items; area != NULL; area = area->Next)
    {
        if (!(area->Options & CONTROLCOMMANDACTION_IGNORE)
            && area->X <= nx && area->Y <= ny && nx < area->Width + area->X && ny < area->Height + area->Y)
        {
            if ((area->Options & CONTROLCOMMANDACTION_MOUSE_ENTER)
                && (ox < area->X || oy < area->Y || area->Width + area->X <= ox || area->Height + area->Y <= oy))
            {
                EnqueueControlCommand(area->Action, CONTROLCOMMANDACTION_MOUSE_ENTER, nx - area->X, ny - area->Y);
            }

            if (accept && (area->Options & CONTROLCOMMANDACTION_MOUSE_HOVER))
            {
                EnqueueControlCommand(area->Action, CONTROLCOMMANDACTION_MOUSE_HOVER, nx - area->X, ny - area->Y);

                if (area->Options & CONTROLCOMMANDACTION_SCROLL) { accept = FALSE; }
            }
        }
    }
}