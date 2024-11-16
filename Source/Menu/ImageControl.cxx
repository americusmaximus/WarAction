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

#include "ImageControl.hxx"
#include "State.hxx"
#include "VideoControl.hxx"

// 0x1003a260
IMAGECONTROLSELF ImageControlSelfState =
{
    (IMAGECONTROLTYPEACTION)AcquireControlTypeImage,
    (IMAGECONTROLINITIALIZEACTION)InitializeControl,
    (IMAGECONTROLDISABLEACTION)DisableControl,
    TickImageControl,
    (IMAGECONTROLACTIONACTION)ActionControl,
    (IMAGECONTROLRELEASEACTION)ReleaseVideoControl
};

// 0x10001ab0
IMAGECONTROLPTR CLASSCALL ActivateImageControl(IMAGECONTROLPTR self, BINASSETPTR asset, CONST U32 indx)
{
    self->Self = &ImageControlSelfState;

    self->IsActive = FALSE;

    self->Asset = asset;
    self->Index = indx;

    self->Y = 0;
    self->X = 0;

    return self;
}

// 0x10001af0
VOID CLASSCALL TickImageControl(IMAGECONTROLPTR self)
{
    if (self->Asset != NULL)
    {
        State.Renderer->Actions.DrawMainSurfaceSprite(self->X, self->Y,
            (IMAGESPRITEPTR)AcquireBinAssetContent(self->Asset, self->Index));
    }
}