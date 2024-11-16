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

#include "ZipFile.hxx"

#include <Renderer.Basic.hxx>

#define MAX_MAP_SIZE            128

#define MAX_MAP_STRUCT3_COUNT   16 /* TODO */
#define MAX_MAP_STRUCT4_COUNT   32 /* TODO */

#define MAP_FILE_SINGLE_MAGIC   0x4D535353 /* MSSS */ /* TODO NAME*/
#define MAP_FILE_MULTI_MAGIC    0x4D4D5353 /* MMSS */ /* TODO NAME*/

typedef struct MapStruct1 // TODO
{
    U32 Unk00; // TODO
    U32 Unk01; // TODO
    U32 Unk02; // TODO
    U32 Unk03; // TODO
} MAPSTRUCT1, * MAPSTRUCT1PTR;

typedef struct MapMinMax
{
    U32 Min;
    U32 Max;
} MAPMINMAX, * MAPMINMAXPTR;

typedef struct MapStruct2 // TODO
{
    MAPMINMAX   Actors;
    U32         Unk02; // TODO
    U32         Unk03; // TODO
    U32         Unk04; // TODO
    S32         Width;
    S32         Height;
} MAPSTRUCT2, * MAPSTRUCT2PTR;

typedef struct MapStruct3 // TODO
{
    S16 Unk00; // TODO
    S16 Unk01; // TODO
    U16 Unk02; // TODO
} MAPSTRUCT3, * MAPSTRUCT3PTR;

typedef struct MapStruct4 // TODO
{
    U16 Unk00; // TODO
    U16 Unk01; // TODO
} MAPSTRUCT4, * MAPSTRUCT4PTR;

typedef struct Map
{
    MAPSTRUCT1  Unk00; // TODO
    MAPSTRUCT2  Unk01; // TODO
    MAPSTRUCT3  Unk02[MAX_MAP_STRUCT3_COUNT]; // TODO
    MAPSTRUCT4  Unk03[MAX_MAP_STRUCT4_COUNT]; // TODO
    LPSTR       Description;
    PIXEL*      Pixels; // TODO
    LPSTR       Unk0x154; // TODO
} MAP, * MAPPTR;

MAPPTR CLASSCALL ActivateMap(MAPPTR self);
VOID CLASSCALL DisposeMap(MAPPTR self);

BOOL InitializeSingleMap(LPCSTR name, MAPPTR map);
BOOL InitializeMultiMap(LPCSTR name, MAPPTR map);
BOOL FUN_10017f60(MAPPTR map, ZIPFILEPTR file); // TODO