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

#include "Lock.hxx"

#include <stdlib.h>

LOCKSTATE LockState;

LOCKPTR CLASSCALL ActivateLock(LOCKPTR self)
{
    if (LockState.Count == 0)
    {
        CONST U32 count = InterlockedExchange(&LockState.Count, 1);

        if (count == 0)
        {
            InitializeCriticalSection(&LockState.Section);

            atexit(ActivateReleaseLockAction);

            LockState.Count = 2;

            EnterCriticalSection(&LockState.Section);

            return self;
        }

        if (count == 2) { LockState.Count = 2; }
    }

    while (LockState.Count == 1) { Sleep(1); }

    if (LockState.Count == 2) { EnterCriticalSection(&LockState.Section); }

    return self;
}

// 0x10036dec
VOID CLASSCALL ReleaseLock(LOCKPTR self)
{
    if (LockState.Count == 2) { LeaveCriticalSection(&LockState.Section); }
}

// 0x10036dce
VOID ActivateReleaseLockAction(VOID)
{
    if (InterlockedExchange(&LockState.Count, 3) == 2) { DeleteCriticalSection(&LockState.Section); }
}