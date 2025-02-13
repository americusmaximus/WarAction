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

#pragma once

#include "ZipFile.hxx"

#include <Map.hxx>

typedef struct MapFileItem
{
    CHAR        Name[MAX_FILE_NAME_LENGTH];
    FILETIME    Time;
} MAPFILEITEM, * MAPFILEITEMPTR;

typedef struct MapFileInfo
{
    DWORD       Size;
    FILETIME    Time;
    CHAR        Name[MAX_FILE_NAME_LENGTH];
} MAPFILEINFO, * MAPFILEINFOPTR;

MAPPTR CLASSCALL ActivateMap(MAPPTR self);
VOID CLASSCALL DisposeMap(MAPPTR self);

BOOL InitializeSingleMap(LPCSTR name, MAPPTR map);
BOOL InitializeMultiMap(LPCSTR name, MAPPTR map);
BOOL FUN_10017f60(MAPPTR map, ZIPFILEPTR file); // TODO
void script_command_perser(MAPPTR map, LPVOID value); // TODO
BOOL ValidateSingleMapFile(LPCSTR name);
BOOL ValidateMultiMapFile(LPCSTR name);