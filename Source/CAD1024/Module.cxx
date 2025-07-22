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

#include "Module.hxx"
#include "Renderer.hxx"

// 0x10012af0
RENDERER ModuleState;

// 0x10001960
RENDERERPTR InitializeModule()
{
    ModuleState.Surface.Main    = RendererState.Surfaces.Main;
    ModuleState.Surface.Back    = RendererState.Surfaces.Back;
    ModuleState.Surface.Stencil = RendererState.Surfaces.Stencil;

    Initialize();

    ModuleState.Actions.FUN_10002fb0_0                              = FUN_10002fb0; // TODO
    ModuleState.Actions.FUN_10002fb0_1                              = FUN_10002fb0; // TODO
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
    ModuleState.Actions.AcquireTextLength                           = AcquireTextLength;
    ModuleState.Actions.DrawBackSurfaceText                         = DrawBackSurfaceText;
    ModuleState.Actions.DrawMainSurfaceText                         = DrawMainSurfaceText;
    ModuleState.Actions.DrawBackSurfaceRhomb                        = DrawBackSurfaceRhomb;
    ModuleState.Actions.FUN_10001f10                                = FUN_10001f10; // TODO
    ModuleState.Actions.FUN_10004390                                = FUN_10004390; // TODO
    ModuleState.Actions.FUN_100046b6                                = FUN_100046b6; // TODO
    ModuleState.Actions.FUN_100049e6                                = FUN_100049e6; // TODO
    ModuleState.Actions.DrawBackSurfacePaletteShadeSprite           = DrawBackSurfacePaletteShadeSprite;
    ModuleState.Actions.FUN_10005ac6                                = FUN_10005ac6; // TODO
    ModuleState.Actions.FUN_1000579c                                = FUN_1000579c; // TODO
    ModuleState.Actions.FUN_10007928                                = FUN_10007928; // TODO
    ModuleState.Actions.WriteBackSurfaceMainSurfaceRectangle        = WriteBackSurfaceMainSurfaceRectangle;
    ModuleState.Actions.DrawBackSurfaceColorPoint                   = DrawBackSurfaceColorPoint;
    ModuleState.Actions.FUN_10001ed0                                = FUN_10001ed0; // TODO
    ModuleState.Actions.FUN_10001f40                                = FUN_10001f40; // TODO
    ModuleState.Actions.DrawMainSurfacePaletteSprite                = DrawMainSurfacePaletteSprite;
    ModuleState.Actions.DrawMainSurfaceSprite                       = DrawMainSurfaceSprite;
    ModuleState.Actions.FUN_1000618d                                = FUN_1000618d; // TODO
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteVersion0      = DrawMainSurfaceAnimationSpriteVersion0;
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteVersion1A     = DrawMainSurfaceAnimationSpriteVersion1A;
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteVersion1B     = DrawMainSurfaceAnimationSpriteVersion1B;
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteVersion2      = DrawMainSurfaceAnimationSpriteVersion2;
    ModuleState.Actions.FUN_100067ad                                = FUN_100067ad; // TODO
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteVersion3      = DrawMainSurfaceAnimationSpriteVersion3;
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteVersion4      = DrawMainSurfaceAnimationSpriteVersion4;
    ModuleState.Actions.FUN_10007be8                                = FUN_10007be8; // TODO
    ModuleState.Actions.FUN_100053c3                                = FUN_100053c3; // TODO
    ModuleState.Actions.DrawMainSurfaceColorPoint                   = DrawMainSurfaceColorPoint;
    ModuleState.Actions.DrawMainSurfaceColorRectangle               = DrawMainSurfaceColorRectangle;
    ModuleState.Actions.DrawMainSurfaceColorBox                     = DrawMainSurfaceColorBox;
    ModuleState.Actions.DrawMainSurfaceHorizontalColorLine          = DrawMainSurfaceHorizontalColorLine;
    ModuleState.Actions.DrawMainSurfaceVerticalColorLine            = DrawMainSurfaceVerticalColorLine;
    ModuleState.Actions.DrawMainSurfaceColorShadeRectangle          = DrawMainSurfaceColorShadeRectangle;
    ModuleState.Actions.DrawMainSurfaceColorOutline                 = DrawMainSurfaceColorOutline;
    ModuleState.Actions.DrawMainSurfaceColorEllipse                 = DrawMainSurfaceColorEllipse;
    ModuleState.Actions.FUN_10002b90                                = FUN_10002b90; // TODO
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceRectangle    = WriteMainSurfaceRendererSurfaceRectangle;
    ModuleState.Actions.ReadMainSurfaceSurfaceRectangle             = ReadMainSurfaceSurfaceRectangle;
    ModuleState.Actions.MaskStencilSurfaceRectangle                 = MaskStencilSurfaceRectangle;
    ModuleState.Actions.DrawStencilSurfaceWindowRectangle           = DrawStencilSurfaceWindowRectangle;
    ModuleState.Actions.WriteRendererSurfaceSurfaceRectangle        = WriteRendererSurfaceSurfaceRectangle;
    ModuleState.Actions.WriteSurfaceSurfaceRectangle                = WriteSurfaceSurfaceRectangle;
    ModuleState.Actions.DrawGameUI                                  = DrawGameUI; // TODO
    ModuleState.Actions.FUN_1000a4f3                                = FUN_1000a4f3; // TODO
    ModuleState.Actions.FUN_10009eb3                                = FUN_10009eb3; // TODO
    ModuleState.Actions.ReleaseDirectX                              = ReleaseDirectX;

    return &ModuleState;
}
