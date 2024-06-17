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

// 0x10001970
RENDERERMODULESTATECONTAINERPTR InitializeModule()
{
    ModuleState.Surface.Main    = RendererState.Surfaces.Main;
    ModuleState.Surface.Back    = RendererState.Surfaces.Back;
    ModuleState.Surface.Stencil = RendererState.Surfaces.Stencil;

    Initialize();

    ModuleState.Actions.FUN_10002fb0_0                              = FUN_10002fc0; // TODO
    ModuleState.Actions.FUN_10002fb0_1                              = FUN_10002fc0; // TODO
    ModuleState.Actions.Initialize                                  = Initialize;
    ModuleState.Actions.InitializeDirectX                           = InitializeDirectX;
    ModuleState.Actions.RestoreDisplayMode                          = RestoreDisplayMode;
    ModuleState.Actions.InitializeWindow                            = InitializeWindow;
    ModuleState.Actions.SetPixelColorMasks                          = SetPixelColorMasks;
    ModuleState.Actions.ReleaseRendererSurface                      = ReleaseRendererSurface;
    ModuleState.Actions.LockRendererSurface                         = LockRendererSurface;
    ModuleState.Actions.UnlockRendererSurface                       = UnlockRendererSurface;
    ModuleState.Actions.OffsetSurfaces                              = OffsetSurfaces;
    ModuleState.Actions.ConvertVisibleColors                        = ConvertVisibleColors;
    ModuleState.Actions.ConvertAllColors                            = ConvertAllColors;
    ModuleState.Actions.FUN_10003320                                = FUN_10003330; // TODO
    ModuleState.Actions.FUN_100033c0                                = FUN_100033d0; // TODO
    ModuleState.Actions.FUN_10003360                                = FUN_10003370; // TODO
    ModuleState.Actions.FUN_10001e90                                = FUN_10001ea0; // TODO
    ModuleState.Actions.FUN_10001f10                                = FUN_10001f20; // TODO
    ModuleState.Actions.FUN_10004390                                = FUN_100043a0; // TODO
    ModuleState.Actions.FUN_100046b6                                = FUN_100046c6; // TODO
    ModuleState.Actions.FUN_100049e6                                = FUN_100049f6; // TODO
    ModuleState.Actions.FUN_10005e31                                = FUN_10005e41; // TODO
    ModuleState.Actions.FUN_10005ac6                                = FUN_10005ad6; // TODO
    ModuleState.Actions.FUN_1000579c                                = FUN_100057ac; // TODO
    ModuleState.Actions.FUN_10007928                                = FUN_10007938; // TODO
    ModuleState.Actions.WriteBackSurfaceMainSurfaceRectangle        = WriteBackSurfaceMainSurfaceRectangle;
    ModuleState.Actions.DrawBackSurfaceColorPoint                   = DrawBackSurfaceColorPoint;
    ModuleState.Actions.FUN_10001ed0                                = FUN_10001ee0; // TODO
    ModuleState.Actions.FUN_10001f40                                = FUN_10001f50; // TODO
    ModuleState.Actions.DrawMainSurfacePaletteSprite                = DrawMainSurfacePaletteSprite;
    ModuleState.Actions.DrawMainSurfaceSprite                       = DrawMainSurfaceSprite;
    ModuleState.Actions.FUN_1000618d                                = FUN_1000619d; // TODO
    ModuleState.Actions.FUN_10004db0                                = FUN_10004dc0; // TODO
    ModuleState.Actions.FUN_10006ef8                                = FUN_10006f08; // TODO
    ModuleState.Actions.FUN_10007292                                = FUN_100072a2; // TODO
    ModuleState.Actions.DrawMainSurfaceCursor                       = DrawMainSurfaceCursor;
    ModuleState.Actions.FUN_100067ad                                = FUN_100067bd; // TODO
    ModuleState.Actions.FUN_10007662                                = FUN_10007672; // TODO
    ModuleState.Actions.FUN_10007fbc                                = FUN_10007fcc; // TODO
    ModuleState.Actions.FUN_10007be8                                = FUN_10007bf8; // TODO
    ModuleState.Actions.FUN_100053c3                                = FUN_100053d3; // TODO
    ModuleState.Actions.FUN_100017e0                                = FUN_100017f0; // TODO
    ModuleState.Actions.DrawMainSurfaceColorRectangle               = DrawMainSurfaceColorRectangle;
    ModuleState.Actions.DrawMainSurfaceColorBox                     = DrawMainSurfaceColorBox;
    ModuleState.Actions.DrawMainSurfaceHorizontalColorLine          = DrawMainSurfaceHorizontalColorLine;
    ModuleState.Actions.DrawMainSurfaceVerticalColorLine            = DrawMainSurfaceVerticalColorLine;
    ModuleState.Actions.DrawMainSurfaceColorShadeRectangle          = DrawMainSurfaceColorShadeRectangle;
    ModuleState.Actions.DrawMainSurfaceColorOutline                 = DrawMainSurfaceColorOutline;
    ModuleState.Actions.FUN_10002020                                = FUN_10002030; // TODO
    ModuleState.Actions.FUN_10002b90                                = FUN_10002ba0; // TODO
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceRectangle    = WriteMainSurfaceRendererSurfaceRectangle;
    ModuleState.Actions.ReadMainSurfaceSurfaceRectangle             = ReadMainSurfaceSurfaceRectangle;
    ModuleState.Actions.MaskStencilSurfaceRectangle                 = MaskStencilSurfaceRectangle;
    ModuleState.Actions.DrawStencilSurfaceWindowRectangle           = DrawStencilSurfaceWindowRectangle;
    ModuleState.Actions.WriteRendererSurfaceSurfaceRectangle        = WriteRendererSurfaceSurfaceRectangle;
    ModuleState.Actions.WriteSurfaceSurfaceRectangle                = WriteSurfaceSurfaceRectangle;
    ModuleState.Actions.FUN_10008ecd                                = FUN_10008edd; // TODO
    ModuleState.Actions.FUN_1000a4f3                                = FUN_1000a503; // TODO
    ModuleState.Actions.FUN_10009eb3                                = FUN_10009ec3; // TODO
    ModuleState.Actions.ReleaseDirectX                              = ReleaseDirectX;

    return &ModuleState;
}