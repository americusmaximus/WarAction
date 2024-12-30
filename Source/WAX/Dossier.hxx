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

#include "Basic.hxx"
#include "Native.Basic.hxx"

#define MAX_DOSSIER_COUNT               32
#define MAX_DOSSIER_STRING_LENGTH       64
#define MAX_DOSSIER_SEPARATOR_LENGTH    256

typedef enum DossierCategory
{
    DOSSIERCATEGORY_SEPARATOR   = 0,
    DOSSIERCATEGORY_QUOTE       = 1,
    DOSSIERCATEGORY_SYMBOL      = 2,
    DOSSIERCATEGORY_TERMINATOR  = 3,
    DOSSIERCATEGORY_FORCE_DWORD = 0x7FFFFFFF
} DOSSIERCATEGORY, * DOSSIERCATEGORYPTR;

#pragma pack(push, 1)
typedef struct Dossier
{
    U32     Count;
    S32     Integers[MAX_DOSSIER_COUNT];
    F32     Values[MAX_DOSSIER_COUNT];
    CHAR    Strings[MAX_DOSSIER_COUNT][MAX_DOSSIER_STRING_LENGTH];
    CHAR    Lowers[MAX_DOSSIER_COUNT][MAX_DOSSIER_STRING_LENGTH];
    U32     Locations[MAX_DOSSIER_COUNT];
    CHAR    Separators[MAX_DOSSIER_SEPARATOR_LENGTH];
    CHAR    Value;
    BOOL    IsQuotes;
    BOOL    IsValue;
} DOSSIER, * DOSSIERPTR;
#pragma pack(pop)