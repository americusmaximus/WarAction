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

#include "Dossier.hxx"

#include <stdlib.h>

// 0x10023b70
DOSSIERPTR ActivateDossier(DOSSIERPTR self)
{
    self->Value = (CHAR)0xFF;
    self->IsValue = FALSE;
    self->IsQuotes = FALSE;

    strcpy(self->Separators, " \t,=\x1A"); // SUB

    return self;
}

// 0x10023c70
VOID InitializeDossier(DOSSIERPTR self, LPCSTR value)
{
    for (U32 x = 0; x < MAX_DOSSIER_COUNT; x++)
    {
        self->Integers[x] = 0;
        self->Values[x] = 0.0f;
        self->Strings[x][0] = NULL;
        self->Lowers[x][0] = NULL;
        self->Locations[x] = 0;
    }

    self->Count = 0;
    self->IsValue = FALSE;
    self->IsQuotes = FALSE;

    U32 x = 0, skip = 0;

    while (TRUE)
    {
        switch (AcquireDossierCategory(self, value[x]))
        {
        case DOSSIERCATEGORY_SEPARATOR: { skip = 1; x = x + 1; break; }
        case DOSSIERCATEGORY_QUOTE: { skip = value[x] == '\"' ? 1 : 0; x = x + 1; break; }
        case DOSSIERCATEGORY_SYMBOL:
        {
            self->Locations[self->Count] = x - skip;

            U32 xx = 0;

            while (TRUE)
            {
                DOSSIERCATEGORY cat;

                while ((cat = AcquireDossierCategory(self, value[x])) == DOSSIERCATEGORY_QUOTE) { x = x + 1; }

                if (cat != DOSSIERCATEGORY_SYMBOL) break;

                self->Strings[self->Count][xx] = value[x];
                self->Lowers[self->Count][xx] = tolower(value[x]);

                x = x + 1;
                xx = xx + 1;
            }

            self->Strings[self->Count][xx] = NULL;
            self->Lowers[self->Count][xx] = NULL;

            self->Integers[self->Count] = atoi(self->Strings[self->Count]);
            self->Values[self->Count] = (F32)atof(self->Strings[self->Count]);

            self->Count = self->Count + 1;

            break;
        }
        case DOSSIERCATEGORY_TERMINATOR: { return; }
        }
    }
}

// 0x10023bb0
DOSSIERCATEGORY AcquireDossierCategory(DOSSIERPTR self, CONST CHAR value)
{
    if (value == NULL)
    {
        self->IsQuotes = FALSE;
        return DOSSIERCATEGORY_TERMINATOR;
    }

    if (self->IsValue)
    {
        if (value == self->Value) { self->IsValue = FALSE; }

        return DOSSIERCATEGORY_QUOTE;
    }

    if (!self->IsQuotes)
    {
        if (value == self->Value)
        {
            self->IsValue = TRUE;
            return DOSSIERCATEGORY_QUOTE;
        }

        if (value == '\"')
        {
            self->IsQuotes = TRUE;
            return DOSSIERCATEGORY_QUOTE;
        }

        for (U32 x = 0; self->Separators[x] != NULL; x++)
        {
            if (value == self->Separators[x]) { return DOSSIERCATEGORY_SEPARATOR; };
        }

        return DOSSIERCATEGORY_SYMBOL;
    }
    else if (value == '\"')
    {
        self->IsQuotes = FALSE;
        return DOSSIERCATEGORY_QUOTE;
    }

    return DOSSIERCATEGORY_SYMBOL;
}