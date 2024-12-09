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

#include "Assets.hxx"
#include "Audio.hxx"
#include "Cursor.hxx"
#include "GreetingsControl.hxx"
#include "Input.hxx"
#include "State.hxx"

// 0x1003a4fc
GREETINGSCONTROLSELF GreetingsControlSelfState =
{
    (GREETINGSCONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeGreetingsControl,
    DisableGreetingsControl,
    TickGreetingsControl,
    ActionGreetingsControl,
    ReleaseGreetingsControl
};

// 0x1000db40
GREETINGSCONTROLPTR CLASSCALL ActivateGreetingsControl(GREETINGSCONTROLPTR self)
{
    self->Self = &GreetingsControlSelfState;

    self->IsActive = FALSE;

    ActivateTextAsset(&self->Text);

    return self;
}

// 0x1000dc00
VOID CLASSCALL InitializeGreetingsControl(GREETINGSCONTROLPTR self)
{
    InitializeControl((CONTROLPTR)self);

    InitializeTextAsset(&self->Text, "greetings");

    self->Ticks = -500;
    self->Type = GREETINGSTYPE_NORMAL;
    self->Item = 0;

    State.Renderer->Actions.DrawMainSurfaceColorRectangle(0, 0,
        GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480, BLACK_PIXEL);

    InitializeAudioPlayer(&AudioPlayerState);

    AudioPlayerState.IsActive = FALSE;

    while (AcquireAudioPlayerMode(&AudioPlayerState) != AUDIOPLAYERMODE_STOPPED) { Sleep(100); }

    InitializeAudioPlayerEvent(&AudioPlayerState, MAX_AUDIO_TRACK_COUNT);

    AudioPlayerState.Ticks = 0;
}

// 0x1000dca0
VOID CLASSCALL DisableGreetingsControl(GREETINGSCONTROLPTR self)
{
    DisableControl((CONTROLPTR)self);

    InitializeTextAsset(&self->Text, NULL);

    ReleaseAudioPlayerEvent(&AudioPlayerState);
}

// 0x1000dd40
VOID CLASSCALL TickGreetingsControl(GREETINGSCONTROLPTR self)
{
    State.Renderer->Actions.DrawMainSurfaceSprite(0, 0,
        (IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.GreetingsBK, 0));

    switch (self->Type)
    {
    case GREETINGSTYPE_NORMAL:
    {
        U32 ticks = 0;

        if (AudioPlayerState.Ticks == 0)
        {
            U32 hours, minutes, seconds, frames;

            AcquireAudioPlayerPosition(&AudioPlayerState, &hours, &minutes, &seconds, &frames);

            ticks = (frames * 1000) / 75 + (minutes * 60 + seconds) * 1000;

            AudioPlayerState.Ticks = ticks - GetTickCount();
        }
        else { ticks = AudioPlayerState.Ticks = GetTickCount(); }

        // TODO not implemented

        break;
    }
    case GREETINGSTYPE_STARWARZ:
    {
        CONST U32 ticks = 480 - (GetTickCount() - self->Ticks) / 7; // TODO

        SelectFontAssetColor(&AssetsState.Fonts.Comic, 0xFF, 0xFF, 0xFF);

        if (self->Text.Count != self->Item && -1 < self->Text.Count - self->Item)
        {
            CONST U32 heigth = AcquireFontAssetHeight(&AssetsState.Fonts.Comic);
            CHAR message[256]; // TODO 

            for (U32 x = 0; x < self->Text.Count - self->Item; x++)
            {
                // TODO NOT IMPLEMENTED

                if (TRUE /* TODO */)
                {
                    // TODO NOT IMPLEMENTED

                    AcquireTextAssetString(&self->Text, self->Item + x, message);
                    // SelectFontAssetColor(&AssetsState.Fonts.Comic, iVar7, iVar7, iVar7); // TODO
                    //DrawFontAssetText(&AssetsState.Fonts.Comic, 320, iVar11, message); // TODO
                }
            }
        }

        break;
    }
    }
}

// 0x1000dcc0
U32 CLASSCALL ActionGreetingsControl(GREETINGSCONTROLPTR self)
{
    AcquireAudioPlayerMode(&AudioPlayerState);

    return (CursorState.IsLeft || InputState.State[DIK_ESCAPE] || InputState.State[DIK_SPACE])
        ? CONTROLACTION_GREETINGS_EXIT : CONTROLACTION_NONE;
}

// 0x1000db90
GREETINGSCONTROLPTR CLASSCALL ReleaseGreetingsControl(GREETINGSCONTROLPTR self, CONST OBJECTRELEASETYPE mode)
{
    DisposeGreetingsControl(self);

    if (mode & OBJECTRELEASETYPE_ALLOCATED) { free(self); }

    return self;
}

// 0x1000dbb0
VOID CLASSCALL DisposeGreetingsControl(GREETINGSCONTROLPTR self)
{
    DisposeTextAsset(&self->Text);

    self->Self = (GREETINGSCONTROLSELFPTR)&ControlSelfState;
}