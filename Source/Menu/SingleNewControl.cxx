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

#include "Assets.hxx"
#include "Game.hxx"
#include "Saves.hxx"
#include "Settings.hxx"
#include "SingleNewControl.hxx"
#include "State.hxx"

#include <..\Text\Resources.hxx>

// 0x1003a6bc
SINGLENEWCONTROLSELF SingleNewControlSelfState =
{
    (SINGLENEWCONTROLTYPEACTION)AcquireControlTypePanel,
    (SINGLENEWCONTROLINITIALIZEACTION)InitializePanelControl,
    (SINGLENEWCONTROLDISABLEACTION)DisablePanelControl,
    (SINGLENEWCONTROLTICKACTION)TickPanelControl,
    ActionSingleNewControl,
    (SINGLENEWCONTROLRELEASEACTION)ReleasePanelControl
};

// 0x10015d80
SINGLENEWCONTROLPTR CLASSCALL ActivateSingleNewControl(SINGLENEWCONTROLPTR self, CONST BOOL addon)
{
    self->Self = &SingleNewControlSelfState;

    ActivateSceneControl((SCENECONTROLPTR)self,
        addon ? &AssetsState.Assets.Single2BK : &AssetsState.Assets.SinOld2BK,
        addon ? &AssetsState.Assets.Single2BT : &AssetsState.Assets.SinOld2BT, CONTROLACTION_SINGLE2_BRITISH);

    self->IsAddon = addon;
    self->Nation = INVALID_GAME_NATION;

    return self;
}

// 0x10015de0
U32 CLASSCALL ActionSingleNewControl(SINGLENEWCONTROLPTR self)
{
    CONST U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    switch (action)
    {
    case CONTROLACTION_SINGLE2_BRITISH: { self->Nation = GAME_NATION_BRITISH; break; }
    case CONTROLACTION_SINGLE2_GERMAN: { self->Nation = GAME_NATION_GERMAN; break; }
    case CONTROLACTION_SINGLE2_RUSSIAN: { self->Nation = GAME_NATION_RUSSIAN; break; }
    case CONTROLACTION_SINGLE2_AMERICAN: { self->Nation = GAME_NATION_AMERICAN; break; }
    }

    return action;
}

// 0x10015e40
BOOL CLASSCALL SaveSingleNewControlState(SINGLENEWCONTROLPTR self)
{
    S32 map = DEFAULT_GAME_MAP_INDEX;
    S32 mission = DEFAULT_GAME_MISSION_INDEX;

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, self->IsAddon ? IDS_NEW_MAP_INDEX : IDS_MAP_INDEX, self->Nation + 1);
        map = AcquireSettingsValue(value, DEFAULT_GAME_MAP_INDEX);
    }

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, self->IsAddon ? IDS_NEW_MISSION_INDEX : IDS_MISSION_INDEX, self->Nation + 1);
        mission = AcquireSettingsValue(value, DEFAULT_GAME_MISSION_INDEX);
    }

    if (map != DEFAULT_GAME_MAP_INDEX && mission != DEFAULT_GAME_MISSION_INDEX)
    {
        if (SaveMapMission(map, mission))
        {
            SaveState.Path[0] = NULL;

            return TRUE;
        }
    }

    return FALSE;
}