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

#include "BinFile.hxx"
#include "Logger.hxx"
#include "State.hxx"

#include <stdio.h>

LOGGERSTATE LoggerState;

VOID CLASSCALL LogMessage(LOGGERPTR self, LPCSTR message);
VOID CLASSCALL LogMessage(LOGGERPTR self, LPCSTR format, va_list args);
VOID CLASSCALL SendLogMessage(LOGGERPTR self, LPCSTR message);

VOID LogError(LPCSTR name, LPCSTR format, va_list args);
VOID LogError(LPCSTR name, CONST U32 id, va_list args);

// 0x1003b8d0
VOID LogError(LPCSTR name, LPCSTR format, ...)
{
    va_list args;
    va_start(args, format);

    LogError(name, format, args);

    va_end(args);
}

// 0x1004ba50
VOID LogError(LPCSTR name, CONST U32 id, ...)
{
    va_list args;
    va_start(args, id);

    LogError(name, id, args);

    va_end(args);
}

// 0x10056e40
VOID LogError(LPCSTR name, LPCSTR format, va_list args)
{
    if (State.Logger !=NULL)
    {
        LogMessage(State.Logger, "---------Error---------\n");

        if (State.Logger != NULL)
        {
            wsprintfA(LoggerState.Message, "%s: %s", name, format);
            LogMessage(State.Logger, LoggerState.Message, args);
        }

        LogMessage(State.Logger, "---------*****---------\n");
    }

    vsprintf(LoggerState.Message, format, args);
    MessageBoxA(State.Window->HWND, LoggerState.Message, name, MB_OK | MB_ICONHAND);
}

// 0x10056ed0
VOID LogError(LPCSTR name, CONST U32 id, va_list args)
{
    LoadStringA(LoggerState.Text, id, LoggerState.Value, MAX_LOG_MESSAGE_LENGTH);

    if (State.Logger != NULL)
    {
        LogMessage(State.Logger, "---------Error---------\n");

        if (State.Logger != NULL)
        {
            wsprintfA(LoggerState.Message, "%s: %s", name, LoggerState.Value);
            LogMessage(State.Logger, LoggerState.Message, args);
        }

        LogMessage(State.Logger, "---------*****---------\n");
    }

    vsprintf(LoggerState.Message, LoggerState.Value, args);
    MessageBoxA(State.Window->HWND, LoggerState.Message, name, MB_OK | MB_ICONHAND);
}

// 0x1008f0e0
VOID LogMessage(LPCSTR format, ...)
{
    if (State.Logger != NULL)
    {
        va_list args;
        va_start(args, format);

        LogMessage(State.Logger, format, args);

        va_end(args);
    }
}

// 0x1008feb0
VOID CLASSCALL LogMessage(LOGGERPTR self, LPCSTR format, va_list args)
{
    CHAR message[MAX_LOG_MESSAGE_LENGTH];

    vsprintf(message, format, args);

    LogMessage(self, message);
}

// 0x1008fe50
VOID CLASSCALL LogMessage(LOGGERPTR self, LPCSTR message)
{
    if (WaitForSingleObject(self->Mutex, 5000) != WAIT_OBJECT_0) { return; }

    if (self->File.Value != INVALID_BINFILE_VALUE) { WriteBinFile(&self->File, message, strlen(message)); }

    OutputDebugStringA(message);

    SendLogMessage(self, message);

    ReleaseMutex(self->Mutex);
}

// 0x1008fdf0
VOID CLASSCALL SendLogMessage(LOGGERPTR self, LPCSTR message)
{
    if (self->HWND != NULL)
    {
        LRESULT result = LB_OKAY;
        while ((result = SendMessageA(self->HWND, LB_ADDSTRING, 0, (LPARAM)message)) == LB_ERR)
        {
            if (SendMessageA(self->HWND, LB_DELETESTRING, 0, 0) == LB_ERR) { return; }
        }

        SendMessageA(self->HWND, LB_SETCURSEL, result, 0);
    }
}