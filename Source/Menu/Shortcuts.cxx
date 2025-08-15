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

#include "AssetFile.hxx"
#include "Control.hxx"
#include "Dossier.hxx"
#include "Shortcuts.hxx"

#include <stdlib.h>
#include <string>

#define MAX_SHORTCUT_LENGTH     256

SHORTCUTS Shortcuts;

// The shortcut configuration file consists of two sections.
// The first section is the key definitions:
//
//      define	ESC	1B
//      define	S   53
//      define	L   4C
//
// The second section is the shortcut action definition:
//
//      SINGLE0_SINGLEMISSIONS  S
//      SINGLE0_LOAD            L
//      SINGLE0_CANCEL		    ESC
//
// The shortcuts loading process is essentially reading those two lists and mapping,
// and mapping those to the list of predefined actions (events), so that
// there is a clear mapping from a numeric action to a numeric key value.

STATIC VOID MakeShortcut(SHORTCUTSPTR self, std::map<std::string, std::string>& names, std::map<std::string, U32>& keys, LPCSTR name, CONST U32 action)
{
    std::string shortcut = name;
    if (names.find(shortcut) != names.end())
    {
        std::string key = names[shortcut];
        if (keys.find(key) != keys.end())
        {
            Shortcuts.insert(std::map<U32, U32>::value_type(action, keys[key]));
        }
    }
}

// 0x100071c0
VOID CLASSCALL InitializeShortcuts(SHORTCUTSPTR self)
{
    std::map<std::string, U32> keys;
    std::map<std::string, std::string> names;

    ASSETFILE file = { (BFH)INVALID_BINFILE_VALUE };
    if (!OpenAssetFile(&file, "shortcuts")) { return; }

    DOSSIER dossier;
    ActivateDossier(&dossier);

    CHAR content[MAX_SHORTCUT_LENGTH];
    while (AcquireAssetFileString(&file, content, MAX_SHORTCUT_LENGTH) != INVALID_ASSET_FILE_STRING_LENGTH)
    {
        InitializeDossier(&dossier, content);

        if (dossier.Count >= 2)
        {
            if (dossier.Count == 3)
            {
                // define	Q	51
                // define	W	57

                if (strncmp(dossier.Strings[0], "define", 7) == 0)
                {
                    U32 value = 0;
                    for (U32 x = 0; dossier.Lowers[2][x] != NULL; x++)
                    {
                        CONST CHAR c = dossier.Lowers[2][x];

                        CONST U32 i = (c < '0' || '9' < c) ? c - 87 : c - 48;

                        value = value * 16 + i;
                    }

                    std::string key = dossier.Strings[1];

                    keys.insert(std::map<std::string, U32>::value_type(key, value));

                    continue;
                }
            }

            {
                // MAIN_SINGLE	S
                // MAIN_MULTI	M

                std::string key = dossier.Strings[0];
                std::string value = dossier.Strings[1];

                names.insert(std::map<std::string, std::string>::value_type(key, value));
            }
        }
    }

    CloseAssetFile(&file);

    MakeShortcut(self, names, keys, "MAIN_SINGLE",             CONTROLACTION_MAIN_SINGLE);
    MakeShortcut(self, names, keys, "MAIN_MULTI",              CONTROLACTION_MAIN_MULTI);
    MakeShortcut(self, names, keys, "MAIN_INTRO",              CONTROLACTION_MAIN_INTRO);
    MakeShortcut(self, names, keys, "MAIN_CREDITS",            CONTROLACTION_MAIN_CREDITS);
    MakeShortcut(self, names, keys, "MAIN_EXIT",               CONTROLACTION_MAIN_EXIT);
    MakeShortcut(self, names, keys, "MAIN_RATING",             CONTROLACTION_MAIN_RATING);
    MakeShortcut(self, names, keys, "MAIN_GREETINGS",          CONTROLACTION_MAIN_GREETINGS);
    MakeShortcut(self, names, keys, "MAIN_RESOLUTIONS",        CONTROLACTION_MAIN_RESOLUTIONS);
    MakeShortcut(self, names, keys, "MAIN_RESOLUTIONS2",       CONTROLACTION_MAIN_RESOLUTIONS2);
    MakeShortcut(self, names, keys, "SINGLE0_NEW",             CONTROLACTION_SINGLE0_NEW);
    MakeShortcut(self, names, keys, "SINGLE0_SINGLEMISSIONS",  CONTROLACTION_SINGLE0_SINGLEMISSIONS);
    MakeShortcut(self, names, keys, "SINGLE0_LOAD",            CONTROLACTION_SINGLE0_LOAD);
    MakeShortcut(self, names, keys, "SINGLE0_CANCEL",          CONTROLACTION_SINGLE0_CANCEL);
    MakeShortcut(self, names, keys, "SINGLE0_DELETE",          CONTROLACTION_SINGLE0_DELETE);
    MakeShortcut(self, names, keys, "SINGLE0_ADDON_CAMPAIGNS", CONTROLACTION_SINGLE0_ADDON_CAMPAIGNS);
    MakeShortcut(self, names, keys, "SINGLE0_DIFFICULTY",      CONTROLACTION_SINGLE0_DIFFICULTY);
    MakeShortcut(self, names, keys, "SINGLE2_BRITISH",         CONTROLACTION_SINGLE2_BRITISH);
    MakeShortcut(self, names, keys, "SINGLE2_AMERIKOSY",       CONTROLACTION_SINGLE2_AMERICAN);
    MakeShortcut(self, names, keys, "SINGLE2_GERMAN",          CONTROLACTION_SINGLE2_GERMAN);
    MakeShortcut(self, names, keys, "SINGLE2_RUSSIAN",         CONTROLACTION_SINGLE2_RUSSIAN);
    MakeShortcut(self, names, keys, "SINGLE2_CANCEL",          CONTROLACTION_SINGLE2_CANCEL);
    MakeShortcut(self, names, keys, "SINGLE3_LOAD",            CONTROLACTION_SINGLE3_LOAD);
    MakeShortcut(self, names, keys, "SINGLE3_CANCEL",          CONTROLACTION_SINGLE3_CANCEL);
    MakeShortcut(self, names, keys, "SINGLE4_LOAD",            CONTROLACTION_SINGLE4_LOAD);
    MakeShortcut(self, names, keys, "SINGLE4_CANCEL",          CONTROLACTION_SINGLE4_CANCEL);
    MakeShortcut(self, names, keys, "SINGLE5_CONTINUE",        CONTROLACTION_SINGLE5_CONTINUE);
    MakeShortcut(self, names, keys, "SINGLE5_REPLAY",          CONTROLACTION_SINGLE5_REPLAY);
    MakeShortcut(self, names, keys, "SINGLE5_EXIT",            CONTROLACTION_SINGLE5_EXIT);
    MakeShortcut(self, names, keys, "SINGLE5_VICTORY",         CONTROLACTION_SINGLE5_VICTORY);
    MakeShortcut(self, names, keys, "DIAL_DIAL",               CONTROLACTION_DIAL_DIAL);
    MakeShortcut(self, names, keys, "DIAL_CANCEL",             CONTROLACTION_DIAL_CANCEL);
    MakeShortcut(self, names, keys, "DIAL_ADD",                CONTROLACTION_DIAL_ADD);
    MakeShortcut(self, names, keys, "DIAL_REMOVE",             CONTROLACTION_DIAL_REMOVE);
    MakeShortcut(self, names, keys, "MULTI1_INTERNET",         CONTROLACTION_MULTI1_INTERNET);
    MakeShortcut(self, names, keys, "MULTI1_LAN",              CONTROLACTION_MULTI1_LAN);
    MakeShortcut(self, names, keys, "MULTI1_MODEM",            CONTROLACTION_MULTI1_MODEM);
    MakeShortcut(self, names, keys, "MULTI1_CANCEL",           CONTROLACTION_MULTI1_CANCEL);
    MakeShortcut(self, names, keys, "MULTI2_CREATE",           CONTROLACTION_MULTI2_CREATE);
    MakeShortcut(self, names, keys, "MULTI2_JOIN",             CONTROLACTION_MULTI2_JOIN);
    MakeShortcut(self, names, keys, "MULTI2_CANCEL",           CONTROLACTION_MULTI2_CANCEL);
    MakeShortcut(self, names, keys, "MULTI2_DELETE",           CONTROLACTION_MULTI2_DELETE);
    MakeShortcut(self, names, keys, "JMULTI1_OK",              CONTROLACTION_JMULTI1_OK);
    MakeShortcut(self, names, keys, "JMULTI1_CANCEL",          CONTROLACTION_JMULTI1_CANCEL);
    MakeShortcut(self, names, keys, "JMULTI2_JOIN",            CONTROLACTION_JMULTI2_JOIN);
    MakeShortcut(self, names, keys, "JMULTI2_CANCEL",          CONTROLACTION_JMULTI2_CANCEL);
    MakeShortcut(self, names, keys, "MULTI3_OK",               CONTROLACTION_MULTI3_OK);
    MakeShortcut(self, names, keys, "MULTI3_CANCEL",           CONTROLACTION_MULTI3_CANCEL);
    MakeShortcut(self, names, keys, "MULTI3_OPTIONS",          CONTROLACTION_MULTI3_OPTIONS);
    MakeShortcut(self, names, keys, "MULTI4_START",            CONTROLACTION_MULTI4_START);
    MakeShortcut(self, names, keys, "MULTI4_CANCEL",           CONTROLACTION_MULTI4_CANCEL);
    MakeShortcut(self, names, keys, "MULTI4_GREYSTART",        CONTROLACTION_MULTI4_GREYSTART);
    MakeShortcut(self, names, keys, "MULTI4_READY",            CONTROLACTION_MULTI4_READY);
    MakeShortcut(self, names, keys, "GREETINGS_EXIT",          CONTROLACTION_GREETINGS_EXIT);
    MakeShortcut(self, names, keys, "RATING_COMPAIGN",         CONTROLACTION_RATING_CAMPAIGN);
    MakeShortcut(self, names, keys, "RATING_SINGLE",           CONTROLACTION_RATING_SINGLE);
    MakeShortcut(self, names, keys, "RATING_CANCEL",           CONTROLACTION_RATING_CANCEL);
    MakeShortcut(self, names, keys, "BRIEF_OK",                CONTROLACTION_BRIEF_OK);
    MakeShortcut(self, names, keys, "BRIEF_MENU",              CONTROLACTION_BRIEF_MENU);
    MakeShortcut(self, names, keys, "BRIEF_THIRDBUTTON",       CONTROLACTION_BRIEF_THIRDBUTTON);
    MakeShortcut(self, names, keys, "MSGBOX_OK",               CONTROLACTION_MSGBOX_OK);
    MakeShortcut(self, names, keys, "MSGBOX_CANCEL",           CONTROLACTION_MSGBOX_CANCEL);
    MakeShortcut(self, names, keys, "MSTAT_DETAIL",            CONTROLACTION_MSTAT_DETAIL);
    MakeShortcut(self, names, keys, "MSTAT_TOTAL",             CONTROLACTION_MSTAT_TOTAL);
    MakeShortcut(self, names, keys, "MSTAT_EXIT",              CONTROLACTION_MSTAT_EXIT);
    MakeShortcut(self, names, keys, "MSTAT_DESTROYED",         CONTROLACTION_MSTAT_DESTROYED);
    MakeShortcut(self, names, keys, "MSTAT_LOST",              CONTROLACTION_MSTAT_LOST);
}

// 0x10009d40
U32 CLASSCALL AcquireShortcut(SHORTCUTSPTR self, CONST U32 action)
{
    if (self->find(action) != self->end()) { return self->operator[](action); }

    return CONTROLACTION_NONE;
}