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
#include "State.hxx"

#include <Mathematics.Basic.hxx>

using namespace Mathematics;

// 0x00402850
VOID AcquireStartArguments()
{
    U32 count = 0;
    U32 length = 0;

    AcquireStartArguments(State.Window->Args, NULL, NULL, &count, &length);

    State.Arguments.All = (CHAR*)malloc(length);
    State.Arguments.Args = (CHAR**)malloc(count * sizeof(CHAR*));

    AcquireStartArguments(State.Window->Args, State.Arguments.Args, State.Arguments.All, &count, &length);

    State.Arguments.Count = count;
}

// 0x004028d0
VOID AcquireStartArguments(LPCSTR value, CHAR** args, CHAR* values, U32* count, U32* length)
{
    BOOL start = FALSE;
    BOOL end = FALSE;

    *length = 0;
    *count = 0;

    if (value == NULL) { return; }

    while (*value != NULL)
    {
        // Skip spaces and tabs.
        for (; *value == ' ' || *value == '\t'; value = value + 1) {}

        if (*value == NULL) { break; }

        if (args != NULL)
        {
            *args = values;
            args = args + 1;
        }

        *count = *count + 1;

        while (TRUE)
        {
            U32 slashes = 0;
            BOOL append = TRUE;
            CHAR current = *value;

            // Count contiguous slashes.
            while (current == '\\')
            {
                value = value + 1;
                slashes = slashes + 1;
                current = *value;
            }

            if (*value == '\"')
            {
                if ((slashes & 1) == 0)
                {
                    if (start && value[1] == '\"')
                    {
                        value = value + 1;
                    }
                    else { append = FALSE; }

                    start = !end;
                    end = start;
                }

                slashes = slashes >> 1;
            }

            for (; slashes != 0; slashes = slashes - 1)
            {
                if (values != NULL)
                {
                    *values = '\\';
                    values = values + 1;
                }

                *length = *length + 1;
            }

            current = *value;

            if (current == NULL || (!start && (current == ' ' || current == '\t'))) { break; }

            if (append)
            {
                if (values != NULL)
                {
                    *values = current;
                    values = values + 1;
                }

                *length = *length + 1;
            }

            value = value + 1;
        }

        if (values != NULL)
        {
            *values = NULL;
            values = values + 1;
        }

        *length = *length + 1;
    }
}

// 0x004029c0
BOOL AcquireStartArguments(LPCSTR name, CHAR* value, CONST U32 length)
{
    for (U32 x = 0; x < State.Arguments.Count; x++)
    {
        CHAR* value = State.Arguments.Args[x];
        CHAR* current = strchr(value, '=');

        if (current != NULL)
        {
            if (strnicmp(value, name, (size_t)current - (size_t)value) == 0)
            {
                current++;

                strncpy(value, current, Min(length, strlen(current)));

                return TRUE;
            }
        }
    }

    return FALSE;
}

// 0x00401130
VOID AcquireIniFileState()
{
    State.IsIniActive = GetPrivateProfileIntA("debug", "useinifile", FALSE, State.App->Ini);
}