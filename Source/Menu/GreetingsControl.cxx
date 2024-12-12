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

#define _USE_MATH_DEFINES
#include <math.h>

#define MAX_GREETING_LENGTH     256
#define MAX_COLOR_VALUE_LENGTH  6

#define DEFAULT_DURATION        2000

// 0x1000dcf0
STATIC F64 sqr(CONST F64 value) { return value * value; }

// 0x1000dd00
STATIC F64 sqr5(CONST F64 value) { return sqr(sqr(sqr(sqr(sqr(value))))); }

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

    CHAR greeting[MAX_GREETING_LENGTH];
    CONST U32 height = AcquireFontAssetHeight(&AssetsState.Fonts.Comic);

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
        else { ticks = AudioPlayerState.Ticks + GetTickCount(); }

        // TODO not implemented

        AcquireTextAssetString(&self->Text, self->Item, greeting);

        // TODO not implemented
        STATIC U32 iStack_11c = 3500; // TODO Fake
        //if (iStack_11c == 0) { iStack_11c = 3500; }// TODO Fake
        iStack_11c = iStack_11c + 10; // TODO Fake
        // TODO ^^

        if (memcmp(greeting, "#starwarz", 10) == 0)
        {
            self->Type = GREETINGSTYPE_STARWARZ;

            self->Ticks = GetTickCount();
            self->Item = self->Item + 1;

            return;
        }

        U32 duration = DEFAULT_DURATION;
        LPSTR item = greeting;

        // !Hello~\FF0000World!

        while (item[0] == '!') { duration = duration * 2; item = item + 1; }

        while (item[0] == '?') { duration = duration / 2; item = item + 1; }

        CONST U32 left = iStack_11c - self->Ticks; // TODO name

        if (duration <= left)
        {
            self->Ticks = self->Ticks + duration;
            self->Item = self->Item + 1;

            return;
        }

        // TODO
        /*
        fVar17 = sin((left / duration) * M_PI);
        minutes = seconds / 1900 + frames / 32;
        uStack_10c = sqrt(sqrt(fVar17));
        
        fVar17 = cos(((left % 1000) * 2) * 0.001 * 2 * M_PI);
        lVar18 = __ftol(fVar17 * minutes);
        iStack_100 = (int)lVar18;
        fVar17 = (float10)sin(extraout_ST1);

        lVar18 = __ftol(fVar17 * extraout_ST0_00);
        iStack_110 = (int)lVar18;*/

        U32 lines = 1;

        for (LPSTR tilde = strchr(item, '~'); tilde != NULL; tilde = strchr(tilde + 1, '~')) { lines = lines + 1; }

        for (U32 x = 0; x < lines; x++) // TODO  incorrect text positions
        {
            LPSTR tilde = strchr(item, '~');
            if (tilde != NULL) { tilde[0] = NULL; }

            U32 r = 0xFF, g = 0xFF, b = 0xFF;

            if (item[0] == '\\')
            {
                // Example: FFFF00

                U32 color = 0;

                for (U32 xx = 0; xx < MAX_COLOR_VALUE_LENGTH; xx++)
                {
                    BYTE value = 0;
                    CONST CHAR i = item[xx + 1];

                    if (i < '0' || '9' < i) { value = i - 55; } else { value = i - 48; }

                    color = color * 16 + value;
                }

                r = (color >> 16) & 0xFF;
                g = (color >> 8) & 0xFF;
                b = (color >> 0) & 0xFF;

                item = item + (MAX_COLOR_VALUE_LENGTH + 1);
            }

            U32 uStack_10c = 1; // TODO PROPER CALCULATION ABOVE

            SelectFontAssetColor(&AssetsState.Fonts.Comic, r * uStack_10c, g * uStack_10c, b * uStack_10c);

            CONST BOOL even = ((self->Item + x) % 2);

            U32 iStack_100 = 1; // TODO PROPER CALCULATION ABOVE
            U32 iStack_110 = 1; // TODO PROPER CALCULATION ABOVE

            DrawFontAssetText(&AssetsState.Fonts.Comic,
                (GRAPHICS_RESOLUTION_640 / 2) + (even ? 1 : -1) + iStack_100,
                ((GRAPHICS_RESOLUTION_480 / 2) - height * (lines - x)) + (even ? -1 : 1) * iStack_110, item);

            if (tilde != NULL) { item = tilde + 1; }
        }

        break;
    }
    case GREETINGSTYPE_STARWARZ:
    {
        //CONST U32 ticks = GRAPHICS_RESOLUTION_480 - (GetTickCount() - self->Ticks) / 7;
        CONST U32 ticks = (GetTickCount() - self->Ticks) % GRAPHICS_RESOLUTION_480; // TODDO ^^

        SelectFontAssetColor(&AssetsState.Fonts.Comic, 0xFF, 0xFF, 0xFF);

        if (self->Text.Count != self->Item)
        {
            for (U32 x = 0; x < self->Text.Count - self->Item; x++)
            {
                U32 iVar11 = height * x + ticks; // TODO
                U32 uVar1 = height / 2 - (GRAPHICS_RESOLUTION_480 / 2) + iVar11;// TODO
                U32 uVar9 = (int)uVar1 >> 0x1f; // TODO

                if ((int)((uVar1 ^ uVar9) - uVar9) < 100 - height / 2) // TODO
                {
                    U32 uVar13 = (int)(iVar11 - (GRAPHICS_RESOLUTION_480 / 2)) >> 0x1f; // TODO
                    U32 minutes = (iVar11 - (GRAPHICS_RESOLUTION_480 / 2) ^ uVar13) - uVar13; // TODO

                    CONST U32 color = (U32)(cos((minutes * M_PI * 0.01) + 1.0) * 127.5);

                    AcquireTextAssetString(&self->Text, self->Item + x, greeting);
                    SelectFontAssetColor(&AssetsState.Fonts.Comic, color, color, color);
                    DrawFontAssetText(&AssetsState.Fonts.Comic, 320, iVar11, greeting);
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