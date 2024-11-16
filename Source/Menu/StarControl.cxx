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
#include "BinAsset.hxx"
#include "StarControl.hxx"
#include "State.hxx"
#include "VideoControl.hxx"

// 0x1003a308
STARCONTROLSELF StarControlSelfState =
{
    (STARCONTROLTYPEACTION)AcquireControlTypeControl,
    InitializeStarControl,
    (STARCONTROLDISABLEACTION)DisableControl,
    TickStarControl,
    (STARCONTROLACTIONACTION)ActionControl,
    (STARCONTROLRELEASEACTION)ReleaseVideoControl
};

// 0x10046f88
STARCONTROL StarControlState;

// 0x10004d60
STARCONTROLPTR CLASSCALL ActivateStarControl(STARCONTROLPTR self)
{
    self->Self = &StarControlSelfState;

    self->IsActive = FALSE;
    self->Area = NULL;

    return self;
}

// 0x100025e0
VOID CLASSCALL InitializeStarControl(STARCONTROLPTR self)
{
    self->Index = INVALID_STAR_INDEX;
    self->Asset = STAR_CONTROL_STARS_ASSET;
}

// 0x10002630
VOID CLASSCALL TickStarControl(STARCONTROLPTR self)
{
    S32 indx = self->Index;

    if (INVALID_STAR_INDEX < indx && indx < MAX_STAR_INDEX)
    {
        BINASSETPTR asset = NULL;

        switch (self->Asset)
        {
        case STAR_CONTROL_STARS_ASSET: { asset = &AssetsState.Assets.Zvezdy; break; }
        case STAR_CONTROL_STARS_GOLD_ASSET:
        {
            CONST U32 ticks = GetTickCount();

            CONST S32 item = (S32)(ticks - self->Area->Ticks) / 150;

            if (item < 0 || 8 < item) // TODO
            {
                asset = &AssetsState.Assets.Zvezdy;

                if (item < 21) // TODO
                {
                    if (8 < item) { self->Area->Ticks = (rand() * 10) / 0x7FFF + ticks; } // TODO
                }
                else { self->Area->Ticks = ticks; }
            }
            else
            {
                CONST S32 element = (S32)(4U - item) >> 0x1F; // TODO

                indx = (self->Index * 5 - ((4U - item ^ element) - element)) + 4; // TODO

                asset = &AssetsState.Assets.ZVGold;
            }

            break;
        }
        case STAR_CONTROL_STARS_MEDAL_ASSET: { asset = &AssetsState.Assets.ZVM; break; }
        }

        {
            IMAGESPRITEPTR image = (IMAGESPRITEPTR)AcquireBinAssetContent(asset, indx);

            image->X = 0;
            image->Y = 0;

            State.Renderer->Actions.DrawMainSurfaceSprite(
                self->Width - AcquireStarControlAssetWidth(self) / 2,
                self->Bottom - AcquireStarControlAssetHeight(self) / 2, image);
        }
    }
}

// 0x10002610
U32 CLASSCALL AcquireStarControlAssetHeight(STARCONTROLPTR self)
{
    return ((IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Zvezdy, 0))->Height;
}

// 0x100025f0
U32 CLASSCALL AcquireStarControlAssetWidth(STARCONTROLPTR self)
{
    return ((IMAGESPRITEPTR)AcquireBinAssetContent(&AssetsState.Assets.Zvezdy, 0))->Width;
}