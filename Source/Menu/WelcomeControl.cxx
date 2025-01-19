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

#include "ActionArea.hxx"
#include "Assets.hxx"
#include "ControlCommand.hxx"
#include "Game.hxx"
#include "Input.hxx"
#include "SceneControl.hxx"
#include "Settings.hxx"
#include "Strings.hxx"
#include "WelcomeControl.hxx"

#include <..\Text\Resources.hxx>

#define RENDERER_VIDEO_MODE_640x480         0
#define RENDERER_VIDEO_MODE_800x600         1
#define RENDERER_VIDEO_MODE_1024x768        2
#define RENDERER_VIDEO_MODE_COUNT           3

// 0x1003a674
WELCOMECONTROLSELF WelcomeControlSelfState =
{
    (WELCOMECONTROLTYPEACTION)AcquireControlTypePanel,
    InitializeWelcomeControl,
    (WELCOMECONTROLDISABLEACTION)DisablePanelControl,
    TickWelcomeControl,
    ActionWelcomeControl,
    (WELCOMECONTROLRELEASEACTION)ReleasePanelControl
};

// 0x10014820
WELCOMECONTROLPTR CLASSCALL ActivateWelcomeControl(WELCOMECONTROLPTR self)
{
    ActivateSceneControl((SCENECONTROLPTR)self,
        &AssetsState.Assets.MainBK, &AssetsState.Assets.MainBT, CONTROLACTION_MAIN_SINGLE);

    self->Self = &WelcomeControlSelfState;

    self->Settings.ID = IDS_VIDEO_MODE;

    {
        STRINGVALUE value;
        AcquireSettingsValue(&value, IDS_VIDEO_MODE);

        self->Settings.Value = AcquireSettingsValue(value, RENDERER_VIDEO_MODE_640x480);
    }

    self->Operations = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_MAIN_RESOLUTIONS, &AssetsState.Assets.MainOP, TRUE);
    self->Resolutions = ActivateRadioControl(ALLOCATE(RADIOCONTROL),
        CONTROLACTION_MAIN_RESOLUTIONS2, &AssetsState.Assets.MainRES, TRUE);

    self->Greetings = ActivateButtonControl(ALLOCATE(BUTTONCONTROL),
        &AssetsState.Assets.MenuG, 0, CONTROLACTION_MAIN_GREETINGS);

    SelectRadioControlPosition(self->Operations, 0, 1);
    SelectRadioControlPosition(self->Resolutions, 0, 1);

    AppendPanelControlNode((PANELCONTROLPTR)self, (CONTROLPTR)self->Operations);
    AppendPanelControlNode((PANELCONTROLPTR)self, (CONTROLPTR)self->Resolutions);
    AppendPanelControlNode((PANELCONTROLPTR)self, (CONTROLPTR)self->Greetings);

    self->Unknown = 0;

    return self;
}

// 0x10014970
VOID CLASSCALL InitializeWelcomeControl(WELCOMECONTROLPTR self)
{
    InitializePanelControl((PANELCONTROLPTR)self);

    SelectRadioControlItem(self->Operations, INVALID_RADIO_ITEM_INDEX);
    SelectRadioControlItem(self->Resolutions, self->Settings.Value - 1);

    for (U32 x = 0; x < RENDERER_VIDEO_MODE_COUNT; x++)
    {
        // NOTE. Make the display resolutions read-only.
        {
            CONST TOGGLECONTROLPTR resolution =
                (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self->Resolutions, x);
            resolution->IsClickable = FALSE;
        }

        // NOTE. Extend clickable area by 70 pixels to the left,
        // so that clicking on resolution value, i.e. 640x480 equals clicking on the selector button.
        {
            CONST TOGGLECONTROLPTR operation =
                (TOGGLECONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self->Operations, x);

            ACTIONAREAPTR area = AcquireActionArea(operation->Action);

            // Extend clickable area by 70 pixels to the left,
            // so that clicking on resolution value, i.e. 640x480 equals clicking on the selector button.
            area->X = area->X - 70;
            area->Width = area->Width + 70;
        }
    }
}

// 0x10014b70
VOID CLASSCALL TickWelcomeControl(WELCOMECONTROLPTR self)
{
    TickPanelControl((PANELCONTROLPTR)self);

    SelectFontAssetColor(&AssetsState.Fonts.Main, 0xAA, 0xAA, 0x55);

    STRINGVALUE value;
    STRINGVALUEPTR version = AcquireStringValue(&value, "ver %d.%d", GAME_VERSION_MAJOR, GAME_VERSION_MINOR);

    DrawFontAssetText(&AssetsState.Fonts.Main, 600, 380, version->Value, FONTALIGNMENT_RIGHT);
    ReleaseStringValue(version);
}

// 0x100149d0
U32 CLASSCALL ActionWelcomeControl(WELCOMECONTROLPTR self)
{
    BOOL toggle = FALSE;

    if (InputState.State[DIK_LSHIFT] || InputState.State[DIK_RSHIFT]) { toggle = TRUE; }

    SelectButtonControlVisibility(self->Greetings, toggle);
    SelectButtonControlVisibility(
        (BUTTONCONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self, WELCOME_CREDITS_BUTTON_INDEX), !toggle);

    CONST U32 action = ActionPanelControl((PANELCONTROLPTR)self);

    CONST CONTROLCOMMANDPTR command = DequeueControlCommand(FALSE);

    if (command == NULL || command->Command != CONTROLCOMMAND_UI) { return action; }

    U32 resolution = RENDERER_VIDEO_MODE_640x480;

    if (command->Action == CONTROLACTION_MAIN_RESOLUTIONS && command->Parameter1 == CONTROLACTION_UI_CLICK) { resolution = RENDERER_VIDEO_MODE_800x600; }
    else if (command->Action == CONTROLACTION_MAIN_RESOLUTIONS2 && command->Parameter1 == CONTROLACTION_UI_CLICK) { resolution = RENDERER_VIDEO_MODE_1024x768; }

    if (command->Action == CONTROLACTION_MAIN_RESOLUTIONS && command->Parameter1 == CONTROLACTION_UI_TOGGLE)
    {
        DequeueControlCommand(TRUE);

        SelectRadioControlItem(self->Operations, INVALID_RADIO_ITEM_INDEX);

        return action;
    }

    if (resolution == RENDERER_VIDEO_MODE_800x600 || resolution == RENDERER_VIDEO_MODE_1024x768)
    {
        DequeueControlCommand(TRUE);

        SaveSettingsValue(&self->Settings, command->Parameter2 / 2 + 1);
        SelectRadioControlItem(self->Resolutions, command->Parameter2 / 2);
    }
    else if (resolution == RENDERER_VIDEO_MODE_COUNT)
    {
        DequeueControlCommand(TRUE);

        SelectRadioControlItem(self->Operations, INVALID_RADIO_ITEM_INDEX);
    }

    return action;
}