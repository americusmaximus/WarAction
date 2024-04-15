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

#pragma once

#include "RendererModule.Basic.hxx"
#include "SoundState.hxx"

typedef struct ModuleStateContainer
{
    HMODULE Handle;
    S32 Unknown0x04; // TODO
    S32 Unknown0x08; // TODO
    S32 Unknown0x0c; // TODO
    S32 Unknown0x10; // TODO
    S32 Unknown0x14; // TODO
    S32 Unknown0x18; // TODO
    S32 Unknown0x1c; // TODO
    S32 Unknown0x20; // TODO
    S32 Unknown0x24; // TODO
    S32 Unknown0x28; // TODO
    S32 Unknown0x2c; // TODO
    S32 Unknown0x30; // TODO

    BYTE Unknown[1420]; // TODO

    LPSOUNDSTATECONTAINER SoundState;
    LPRENDERERMODULESTATECONTAINER RendererState;
    HMODULE TextModule;
    LPUNKNOWN Unknown0x5cc; // TODO
} MODULESTATECONTAINER, * LPMODULESTATECONTAINER;