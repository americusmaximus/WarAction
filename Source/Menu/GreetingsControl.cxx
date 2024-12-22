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

#define MAX_GREETING_LENGTH         256
#define MAX_COLOR_VALUE_LENGTH      6

#define DEFAULT_DURATION            2000
#define MAX_DURATION_COUNT          29

#define GREETING_AUDIO_TRACK_INDEX  11

#define MAX_STARWARZ_HEIGHT         100

typedef struct Duration
{
    U32     Item1;
    U32     Item2;
} DURATION, * DURATIONPTR;

// 0x1000dcf0
STATIC F64 sqr(CONST F64 value) { return value * value; }

// 0x1000dd00
STATIC F64 sqr5(CONST F64 value) { return sqr(sqr(sqr(sqr(sqr(value))))); }

// 0x1003f8d0
STATIC DURATION Durations[MAX_DURATION_COUNT] =
{
    { 0xB, 0x0 },       { 0x124, 0x0 },     { 0x3E23, 0x40 },   { 0x7B18, 0x80 },
    { 0x9984, 0xA0 },   { 0xB370, 0xBB },   { 0xF377, 0xFE },   { 0x12CD3, 0x13A },
    { 0x1ACFB, 0x1C0 }, { 0x1BC6E, 0x1D0 }, { 0x1DA9F, 0x1F0 }, { 0x1F925, 0x210 },
    { 0x23602, 0x250 }, { 0x2BD81, 0x2E0 }, { 0x30A15, 0x330 }, { 0x31988, 0x340 },
    { 0x32505, 0x34C }, { 0x33C2A, 0x364 }, { 0x3759A, 0x3A0 }, { 0x3EFED, 0x420 },
    { 0x41D52, 0x450 }, { 0x43006, 0x463 }, { 0x469D7, 0x4A2 }, { 0x4A36A, 0x4E0 },
    { 0x4E036, 0x522 }, { 0x5196A, 0x560 }, { 0x553A6, 0x5A0 }, { 0x570C4, 0x5C0 },
    { 0x0, 0x0 }
};

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

    InitializeAudioPlayerEvent(&AudioPlayerState, GREETING_AUDIO_TRACK_INDEX);

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
    CONST S32 height = AcquireFontAssetHeight(&AssetsState.Fonts.Comic);

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

        U32 indx = 0;
        for (U32 result = Durations[1].Item1; result < ticks; result = Durations[indx + 2].Item1, indx++) {}

        if (indx != 0 && Durations[indx + 1].Item1 != 0)
        {
            // TODO: Very rough wiggle of the text, not smooth like in the original.


            // Display/wait durations.

            CONST F64 value1 = (ticks - Durations[indx].Item1) * (Durations[indx + 1].Item2 - Durations[indx].Item2);
            CONST f64 value2 = Durations[indx + 1].Item1 - Durations[indx].Item1;

            CONST F64 value3 = (value1 / value2 + (F64)Durations[indx].Item2) * 250.0;
            CONST F64 value4 = cos(0.012566370614 * value3);

            CONST F64 value5 = sqr5(value4);
            CONST F64 value6 = value5 * 255.0;

            CONST S32 todo1 = (S32)value6; // TODO seconds

            CONST F64 value7 = cos(value5 * 255.0 * M_PI / 1000.0);
            CONST F64 value8 = sqr5(value7);
            CONST F64 value9 = value8 * 255.0;

            CONST S32 todo2 = (S32)value9; // TODO frames

            AcquireTextAssetString(&self->Text, self->Item, greeting);

            if (memcmp(greeting, "#starwarz", 10) == 0)
            {
                self->Type = GREETINGSTYPE_STARWARZ;

                self->Ticks = GetTickCount();
                self->Item = self->Item + 1;

                return;
            }

            S32 duration = DEFAULT_DURATION;
            LPSTR item = greeting;

            // Example: !Hello~\FF0000World!

            while (item[0] == '!') { duration = duration * 2; item = item + 1; }

            while (item[0] == '?') { duration = duration / 2; item = item + 1; }

            CONST S32 left = (S32)value3 - self->Ticks;

            if (duration <= left)
            {
                self->Ticks = self->Ticks + duration;
                self->Item = self->Item + 1;

                return;
            }

            // Visual effects and position offsets.

            CONST F64 fVar17 = sin(((F64)left / (F64)duration) * M_PI);
            CONST F64 value11 = todo1 / 512 + todo2 / 32;
            CONST F64 value13 = cos((F64)((left % 1000) * 2) * 0.001 * 2.0 * M_PI);
            CONST F64 value14 = value13 * value11;
            CONST S32 iStack_100 = (S32)value14;

            CONST F64 value16 = sin(value14);
            CONST F64 value17 = value14 * value16;
            CONST S32 iStack_110 = (S32)value17;

            S32 lines = 1;

            for (LPSTR tilde = strchr(item, '~'); tilde != NULL; tilde = strchr(tilde + 1, '~')) { lines = lines + 1; }

            for (S32 xx = 0; xx < lines; xx++)
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

                        value = (i < '0' || '9' < i) ? i - 55 : i - 48;

                        color = color * 16 + value;
                    }

                    r = (color >> 16) & 0xFF;
                    g = (color >> 8) & 0xFF;
                    b = (color >> 0) & 0xFF;

                    item = item + (MAX_COLOR_VALUE_LENGTH + 1);
                }

                {
                    CONST F64 modifier = sqrt(sqrt(fVar17));

                    SelectFontAssetColor(&AssetsState.Fonts.Comic,
                        (U32)(r * modifier), (U32)(g * modifier), (U32)(b * modifier));
                }

                {
                    CONST BOOL even = ((self->Item + xx) % 2);

                    CONST S32 x = (even ? 1 : -1) * iStack_100;
                    CONST S32 y = (height * (lines - 2 * xx)) / 2 - (even ? 1 : -1) * iStack_110;

                    DrawFontAssetText(&AssetsState.Fonts.Comic,
                        GRAPHICS_RESOLUTION_640 / 2 + x, GRAPHICS_RESOLUTION_480 / 2 - y, item);
                }

                if (tilde != NULL) { item = tilde + 1; }
            }
        }

        break;
    }
    // NOTE. Never used in the game itself.
    case GREETINGSTYPE_STARWARZ:
    {
        CONST U32 ticks = GRAPHICS_RESOLUTION_480 - (GetTickCount() - self->Ticks) / 7;

        SelectFontAssetColor(&AssetsState.Fonts.Comic, 0xFF, 0xFF, 0xFF);

        if (self->Item < self->Text.Count)
        {
            for (U32 x = 0; x < self->Text.Count - self->Item; x++)
            {
                CONST S32 offset = height * x + ticks;
                CONST U32 delta = abs(offset + (height - GRAPHICS_RESOLUTION_480) / 2);

                if (delta < MAX_STARWARZ_HEIGHT - height / 2)
                {
                    CONST U32 color = (U32)((cos(abs(offset - (GRAPHICS_RESOLUTION_480 / 2)) * M_PI * 0.01) + 1.0) * 127.5);

                    AcquireTextAssetString(&self->Text, self->Item + x, greeting);
                    SelectFontAssetColor(&AssetsState.Fonts.Comic, color, color, color);
                    DrawFontAssetText(&AssetsState.Fonts.Comic, GRAPHICS_RESOLUTION_640 / 2, offset, greeting);
                }
            }

            break;
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