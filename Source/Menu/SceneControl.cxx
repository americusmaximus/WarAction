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

#include "ButtonControl.hxx"
#include "ImageControl.hxx"
#include "SceneControl.hxx"

#include <stdlib.h>

// 0x1003a2d8
SCENECONTROLSELF SceneControlSelfState =
{
    (SCENECONTROLTYPEACTION)AcquireControlTypePanel,
    (SCENECONTROLINITIALIZEACTION)InitializePanelControl,
    (SCENECONTROLDISABLEACTION)DisablePanelControl,
    (SCENECONTROLTICKACTION)TickPanelControl,
    (SCENECONTROLACTIONACTION)ActionPanelControl,
    (SCENECONTROLRELEASEACTION)ReleasePanelControl,
};

// 0x10002200
SCENECONTROLPTR CLASSCALL ActivateSceneControl(SCENECONTROLPTR self, BINASSETPTR background, BINASSETPTR buttons, CONST U32 action)
{
    self->Self = &SceneControlSelfState;

    self->IsActive = FALSE;
    self->Nodes = NULL;

    self->Background = background;
    self->Buttons = buttons;

    AppendPanelControlNode((PANELCONTROLPTR)self,
        (CONTROLPTR)ActivateImageControl(ALLOCATE(IMAGECONTROL), background, 0));

    CONST U32 count = AcquireBinAssetImageCount(buttons) / 2;

    for (U32 x = 0; x < count; x++)
    {
        BUTTONCONTROLPTR button = ActivateButtonControl(ALLOCATE(BUTTONCONTROL), buttons, x * 2, action + x);

        button->Click = "push";
        button->Unclick = "pull";

        AppendPanelControlNode((PANELCONTROLPTR)self, (CONTROLPTR)button);
    }

    return self;
}

// 0x100022e0
U32 CLASSCALL AcquireSceneButtonCount(SCENECONTROLPTR self)
{
    return AcquireBinAssetImageCount(self->Buttons) / 2;
}

// 0x100022f0
BUTTONCONTROLPTR CLASSCALL AcquireSceneControlButton(SCENECONTROLPTR self, CONST U32 indx)
{
    return (BUTTONCONTROLPTR)AcquirePanelControlNode((PANELCONTROLPTR)self, indx + 1);
}