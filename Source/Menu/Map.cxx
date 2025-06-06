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

#include "Map.hxx"
#include "Strings.hxx"
#include "Version.hxx"

#include <stdlib.h>

// 0x10017e00
MAPPTR CLASSCALL ActivateMap(MAPPTR self)
{
    self->Header.Unk00 = 0; // TODO
    self->Header.Unk01 = 0; // TODO
    self->Header.Unk02 = 0; // TODO
    self->Header.Unk03 = 0; // TODO

    self->Descriptor.Actors.Min = 0;
    self->Descriptor.Actors.Max = 0;

    self->Descriptor.Unk02 = 0; // TODO
    self->Descriptor.Unk03 = 0; // TODO
    self->Descriptor.Unk04 = 0; // TODO

    self->Descriptor.Width = 0;
    self->Descriptor.Height = 0;

    self->Description = NULL;
    self->Pixels = NULL;
    self->MissionDescription = NULL;

    return self;
}

// 0x10017e50
VOID CLASSCALL DisposeMap(MAPPTR self)
{
    if (self->Description != NULL)
    {
        free(self->Description);

        self->Description = NULL;
    }

    if (self->MissionDescription != NULL)
    {
        free(self->MissionDescription);

        self->MissionDescription = NULL;
    }

    if (self->Pixels != NULL)
    {
        free(self->Pixels);

        self->Pixels = NULL;
    }
}

// 0x1001aaa0
BOOL InitializeSingleMap(LPCSTR name, MAPPTR map)
{
    DisposeMap(map);

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, ZIPFILE_OPEN_READ)) { return FALSE; }

    {
        U32 magic = 0;
        ReadZipFile(&zip, &magic, sizeof(U32));

        if (magic != MAP_FILE_SINGLE_MAGIC)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    {
        U32 version = 0;
        ReadZipFile(&zip, &version, sizeof(U32));

        if (version != MODULE_VERSION_VALUE)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    ReadZipFile(&zip, &map->Header, sizeof(MAPHEADER));
    ReadZipFile(&zip, &map->Descriptor, sizeof(MAPDESCRIPTOR));

    map->Descriptor.Actors.Min = 1;
    map->Descriptor.Actors.Max = 1;
    map->Descriptor.Unk02 = 1; // TODO
    map->Descriptor.Unk03 = 1; // TODO

    {
        U32 length = 0;
        ReadZipFile(&zip, &length, sizeof(U32));

        LPSTR description = (LPSTR)malloc(length + 1);
        description[length] = NULL;

        ReadZipFile(&zip, description, length);

        map->Description = AcquireCleanUnicodeString(description, TRUE);
    }

    if (map->Description[0] == '$')
    {
        U32 x = 0;
        LPSTR description = map->Description;

        while (description[x] != '\n' && description[x] != '\r') { x++; }

        description[x] = NULL;

        do { do { x++; } while (description[x] == '\n'); } while (description[x] == '\r');

        map->Description = (LPSTR)malloc(strlen(&description[x]) + 1);

        strcpy(map->Description, &description[x]);

        map->MissionDescription = (LPSTR)malloc(strlen(description));

        strcpy(map->MissionDescription, description);

        free(description);
    }
    else { map->MissionDescription = NULL; }

    for (U32 x = 0; map->Description[x] != NULL; x++)
    {
        if (map->Description[x] == '#') { map->Description[x] = NULL; break; }
    }

    {
        CONST U32 size = map->Descriptor.Width <= MAX_MAP_SIZE && map->Descriptor.Height <= MAX_MAP_SIZE
            ? map->Descriptor.Height * map->Descriptor.Width * sizeof(PIXEL)
            : map->Descriptor.Height * map->Descriptor.Width / 2;

        map->Pixels = (PIXEL*)malloc(size);
        ReadZipFile(&zip, map->Pixels, size);
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10018240
BOOL InitializeMultiMap(LPCSTR name, MAPPTR map)
{
    DisposeMap(map);

    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, ZIPFILE_OPEN_READ)) { return FALSE; }

    {
        U32 magic = 0;
        ReadZipFile(&zip, &magic, sizeof(U32));

        if (magic != MAP_FILE_MULTI_MAGIC)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    {
        U32 version = 0;
        ReadZipFile(&zip, &version, sizeof(U32));
        if (version != MODULE_MULTI_MAP_VERSION_VALUE)
        {
            CloseZipFile(&zip);

            return FALSE;
        }
    }

    ReadZipFile(&zip, &map->Header, sizeof(MAPHEADER));
    ReadZipFile(&zip, &map->Descriptor.Actors, sizeof(MAPMINMAX));

    ReadZipFile(&zip, &map->Unk02, MAX_MAP_STRUCT3_COUNT * sizeof(MAPSTRUCT3));

    for (U32 x = 0; x < MAX_MAP_STRUCT3_COUNT; x++)
    {
        if (map->Unk02[x].Unk00 < 0 || map->Unk02[x].Unk01 < 0
            || map->Descriptor.Width <= map->Unk02[x].Unk00 || map->Descriptor.Height <= map->Unk02[x].Unk01
            || map->Unk02[x].Unk02 < 0 || (MAX_MAP_SIZE - 1) < map->Unk02[x].Unk02)
        {
            map->Unk02[x].Unk00 = 0;
            map->Unk02[x].Unk02 = 0;
            map->Unk02[x].Unk01 = 0;
        }
    }

    ReadZipFile(&zip, &map->Descriptor.Unk04, 3 * sizeof(U32) /* TODO */);

    for (U32 x = 0; x < MAX_MAP_STRUCT4_COUNT; x++)
    {
        ReadZipFile(&zip, &map->Unk03[x].Unk00, sizeof(U32)); // TODO
        map->Unk03[x].Unk01 = 0xffff; // TODO
    }

    {
        CHAR buffer[272]; // TODO
        ReadZipFile(&zip, buffer, 272); // TODO
    }

    FUN_10017f60(map, &zip);

    {
        U32 length = 0;
        ReadZipFile(&zip, &length, sizeof(U32));

        LPSTR description = (LPSTR)malloc(length + 1);
        description[length] = NULL;

        ReadZipFile(&zip, description, length);

        map->Description = AcquireCleanUnicodeString(description, TRUE);
    }

    if (map->Description[0] == '$')
    {
        U32 x = 0;
        LPSTR description = map->Description;

        while (description[x] != '\n' && description[x] != '\r') { x++; }

        description[x] = NULL;

        do { do { x++; } while (description[x] == '\n'); } while (description[x] == '\r');

        map->Description = (LPSTR)malloc(strlen(&description[x]) + 1);

        strcpy(map->Description, &description[x]);

        map->MissionDescription = (LPSTR)malloc(strlen(description));

        strcpy(map->MissionDescription, description);

        free(description);
    }
    else { map->MissionDescription = NULL; }

    for (U32 x = 0; map->Description[x] != NULL; x++)
    {
        if (map->Description[x] == '#') { map->Description[x] = NULL; break; }
    }

    {
        CONST U32 size = map->Descriptor.Width <= MAX_MAP_SIZE && map->Descriptor.Height <= MAX_MAP_SIZE
            ? map->Descriptor.Height * map->Descriptor.Width * sizeof(PIXEL)
            : map->Descriptor.Height * map->Descriptor.Width / 2; // TODO

        map->Pixels = (PIXEL*)malloc(size);
        ReadZipFile(&zip, map->Pixels, size);
    }

    CloseZipFile(&zip);

    return TRUE;
}

// 0x10017f60
BOOL FUN_10017f60(MAPPTR map, ZIPFILEPTR file) // TODO
{
    U32 count = 0;
    ReadZipFile(file, &count, sizeof(U32));

    for (U32 x = 0; x < count; x++)
    {
        U32 length = 0;
        ReadZipFile(file, &length, sizeof(U32));

        if (length != 0)
        {
            LPVOID value = malloc(length); // TODO
            ReadZipFile(file, value, length);
            // FUN_10017fe0(map, value); // TODO
            free(value);
        }
    }

    return TRUE;
}

// 0x1001a9f0
BOOL ValidateSingleMapFile(LPCSTR name)
{
    ZIPFILE zip;
    ZeroMemory(&zip, sizeof(ZIPFILE));

    if (!OpenZipFile(&zip, name, 0)) { return FALSE; }

    U32 magic = 0;
    ReadZipFile(&zip, &magic, sizeof(U32));

    if (magic != MAP_FILE_SINGLE_MAGIC)
    {
        CloseZipFile(&zip);

        return FALSE;
    }

    U32 version = 0;
    ReadZipFile(&zip, &version, sizeof(U32));

    if (version != MODULE_VERSION_VALUE)
    {
        CloseZipFile(&zip);

        return FALSE;
    }

    CloseZipFile(&zip);

    return TRUE;
}