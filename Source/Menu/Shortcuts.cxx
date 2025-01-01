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

    {
        std::string shortcut = "MAIN_SINGLE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_SINGLE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_MULTI";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_MULTI, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_INTRO";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_INTRO, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_CREDITS";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_CREDITS, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_EXIT";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_EXIT, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_RATING";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_RATING, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_GREETINGS";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_GREETINGS, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_RESOLUTIONS";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_RESOLUTIONS, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MAIN_RESOLUTIONS2";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MAIN_RESOLUTIONS2, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_NEW";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_NEW, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_SINGLEMISSIONS";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_SINGLEMISSIONS, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_LOAD";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_LOAD, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_DELETE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_DELETE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_ADDON_CAMPAIGNS";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_ADDON_CAMPAIGNS, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE0_DIFFICULTY";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE0_DIFFICULTY, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE2_BRITISH";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE2_BRITISH, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE2_AMERIKOSY";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE2_AMERICAN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE2_GERMAN";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE2_GERMAN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE2_RUSSIAN";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE2_RUSSIAN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE2_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE2_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE3_LOAD";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE3_LOAD, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE3_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE3_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE4_LOAD";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE4_LOAD, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE4_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE4_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE5_CONTINUE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE5_CONTINUE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE5_REPLAY";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE5_REPLAY, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE5_EXIT";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE5_EXIT, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "SINGLE5_VICTORY";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_SINGLE5_VICTORY, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "DIAL_DIAL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_DIAL_DIAL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "DIAL_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_DIAL_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "DIAL_ADD";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_DIAL_ADD, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "DIAL_REMOVE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_DIAL_REMOVE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI1_INTERNET";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI1_INTERNET, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI1_LAN";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI1_LAN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI1_MODEM";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI1_MODEM, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI1_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI1_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI2_CREATE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI2_CREATE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI2_JOIN";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI2_JOIN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI2_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI2_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI2_DELETE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI2_DELETE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "JMULTI1_OK";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_JMULTI1_OK, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "JMULTI1_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_JMULTI1_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "JMULTI2_JOIN";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_JMULTI2_JOIN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "JMULTI2_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_JMULTI2_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI3_OK";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI3_OK, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI3_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI3_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI3_OPTIONS";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI3_OPTIONS, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI4_START";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI4_START, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI4_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI4_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI4_GREYSTART";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI4_GREYSTART, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MULTI4_READY";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MULTI4_READY, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "GREETINGS_EXIT";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_GREETINGS_EXIT, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "RATING_COMPAIGN";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_RATING_CAMPAIGN, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "RATING_SINGLE";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_RATING_SINGLE, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "RATING_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_RATING_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "BRIEF_OK";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_BRIEF_OK, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "BRIEF_MENU";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_BRIEF_MENU, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "BRIEF_THIRDBUTTON";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_BRIEF_THIRDBUTTON, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSGBOX_OK";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSGBOX_OK, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSGBOX_CANCEL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSGBOX_CANCEL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSTAT_DETAIL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSTAT_DETAIL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSTAT_TOTAL";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSTAT_TOTAL, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSTAT_EXIT";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSTAT_EXIT, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSTAT_DESTROYED";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSTAT_DESTROYED, keys[key]));
            }
        }
    }

    {
        std::string shortcut = "MSTAT_LOST";
        if (names.find(shortcut) != names.end())
        {
            std::string key = names[shortcut];
            if (keys.find(key) != keys.end())
            {
                Shortcuts.insert(std::map<U32, U32>::value_type(CONTROLACTION_MSTAT_LOST, keys[key]));
            }
        }
    }
}

// 0x10009d40
U32 CLASSCALL AcquireShortcut(SHORTCUTSPTR self, CONST U32 action)
{
    if (self->find(action) != self->end()) { return self->operator[](action); }

    return CONTROLACTION_NONE;
}