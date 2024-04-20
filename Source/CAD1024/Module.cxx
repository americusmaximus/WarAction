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

#include "Module.hxx"
#include "Renderer.hxx"

// 0x10012af0
RENDERERMODULESTATECONTAINER ModuleState;

// 0x10001960
LPRENDERERMODULESTATECONTAINER Initialize()
{
    ModuleState.Surface.Main    = RendererState.Surfaces.Main;
    ModuleState.Surface.Back    = RendererState.Surfaces.Back;
    ModuleState.Surface.Stencil = RendererState.Surfaces.Stencil;

    InitializeRenderer();

    ModuleState.Actions.FUN_10002fb0_0                              = FUN_10002fb0; // TODO
    ModuleState.Actions.FUN_10002fb0_1                              = FUN_10002fb0; // TODO
    ModuleState.Actions.InitializeRenderer                          = InitializeRenderer;
    ModuleState.Actions.InitializeDirectDraw                        = InitializeDirectDraw;
    ModuleState.Actions.RestoreDisplayMode                          = RestoreDisplayMode;
    ModuleState.Actions.InitializeWindow                            = InitializeWindow;
    ModuleState.Actions.SetPixelColorMasks                          = SetPixelColorMasks;
    ModuleState.Actions.ReleaseRendererSurface                      = ReleaseRendererSurface;
    ModuleState.Actions.LockRendererSurface                         = LockRendererSurface;
    ModuleState.Actions.UnlockRendererSurface                       = UnlockRendererSurface;
    ModuleState.Actions.OffsetSurfaces                              = OffsetSurfaces;
    ModuleState.Actions.ConvertColorsExtra                          = ConvertColorsExtra;
    ModuleState.Actions.ConvertColors                               = ConvertColors;
    ModuleState.Actions.FUN_10003320                                = FUN_10003320; // TODO
    ModuleState.Actions.FUN_100033c0                                = FUN_100033c0; // TODO
    ModuleState.Actions.FUN_10003360                                = FUN_10003360; // TODO
    ModuleState.Actions.FUN_10001e90                                = FUN_10001e90; // TODO
    ModuleState.Actions.FUN_10001f10                                = FUN_10001f10; // TODO
    ModuleState.Actions.FUN_10004390                                = FUN_10004390; // TODO
    ModuleState.Actions.FUN_100046b6                                = FUN_100046b6; // TODO
    ModuleState.Actions.FUN_100049e6                                = FUN_100049e6; // TODO
    ModuleState.Actions.FUN_10005e31                                = FUN_10005e31; // TODO
    ModuleState.Actions.FUN_10005ac6                                = FUN_10005ac6; // TODO
    ModuleState.Actions.FUN_1000579c                                = FUN_1000579c; // TODO
    ModuleState.Actions.FUN_10007928                                = FUN_10007928; // TODO
    ModuleState.Actions.WriteBackSurfaceMainSurfaceRectangle        = WriteBackSurfaceMainSurfaceRectangle;
    ModuleState.Actions.DrawBackSurfaceColorPoint                   = DrawBackSurfaceColorPoint;
    ModuleState.Actions.FUN_10001ed0                                = FUN_10001ed0; // TODO
    ModuleState.Actions.FUN_10001f40                                = FUN_10001f40; // TODO
    ModuleState.Actions.FUN_100050df                                = FUN_100050df; // TODO
    ModuleState.Actions.FUN_100064b6                                = FUN_100064b6; // TODO
    ModuleState.Actions.FUN_1000618d                                = FUN_1000618d; // TODO
    ModuleState.Actions.FUN_10004db0                                = FUN_10004db0; // TODO
    ModuleState.Actions.FUN_10006ef8                                = FUN_10006ef8; // TODO
    ModuleState.Actions.FUN_10007292                                = FUN_10007292; // TODO
    ModuleState.Actions.FUN_10006b21                                = FUN_10006b21; // TODO
    ModuleState.Actions.FUN_100067ad                                = FUN_100067ad; // TODO
    ModuleState.Actions.FUN_10007662                                = FUN_10007662; // TODO
    ModuleState.Actions.FUN_10007fbc                                = FUN_10007fbc; // TODO
    ModuleState.Actions.FUN_10007be8                                = FUN_10007be8; // TODO
    ModuleState.Actions.FUN_100053c3                                = FUN_100053c3; // TODO
    ModuleState.Actions.FUN_100017e0                                = FUN_100017e0; // TODO
    ModuleState.Actions.DrawMainSurfaceColorRectangle               = DrawMainSurfaceColorRectangle;
    ModuleState.Actions.DrawMainSurfaceColorBox                     = DrawMainSurfaceColorBox;
    ModuleState.Actions.DrawMainSurfaceHorizontalColorLine          = DrawMainSurfaceHorizontalColorLine;
    ModuleState.Actions.DrawMainSurfaceVerticalColorLine            = DrawMainSurfaceVerticalColorLine;
    ModuleState.Actions.FUN_100016c0                                = FUN_100016c0; // TODO
    ModuleState.Actions.FUN_100023e0                                = FUN_100023e0; // TODO
    ModuleState.Actions.FUN_10002020                                = FUN_10002020; // TODO
    ModuleState.Actions.ReadRendererSurfaceRectangle                = FUN_10002b90;
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceRectangle    = WriteMainSurfaceRendererSurfaceRectangle;
    ModuleState.Actions.FUN_100018a0                                = FUN_100018a0; // TODO
    ModuleState.Actions.FUN_10002780                                = FUN_10002780; // TODO
    ModuleState.Actions.DrawBackSurfaceWindowRectangle              = DrawBackSurfaceWindowRectangle;
    ModuleState.Actions.FUN_10002990                                = FUN_10002990; // TODO
    ModuleState.Actions.FUN_10002a30                                = FUN_10002a30; // TODO
    ModuleState.Actions.FUN_10008ecd                                = FUN_10008ecd; // TODO
    ModuleState.Actions.FUN_1000a4f3                                = FUN_1000a4f3; // TODO
    ModuleState.Actions.FUN_10009eb3                                = FUN_10009eb3; // TODO
    ModuleState.Actions.ReleaseDirectDraw                           = ReleaseDirectDraw;

    return &ModuleState;
}