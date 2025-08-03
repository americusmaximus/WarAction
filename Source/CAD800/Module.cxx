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

// 0x10001970
RENDERERPTR InitializeModule()
{
    ModuleState.Surface.Main    = RendererState.Surfaces.Main;
    ModuleState.Surface.Back    = RendererState.Surfaces.Back;
    ModuleState.Surface.Stencil = RendererState.Surfaces.Stencil;

    Initialize();

    ModuleState.Actions.BlendMainSurfaceWithFogOfWarA                              = FUN_10002fc0; // TODO
    ModuleState.Actions.BlendMainSurfaceWithFogOfWarB                              = FUN_10002fc0; // TODO
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
    ModuleState.Actions.DrawMainSurfaceMaskRhomb                    = DrawMainSurfaceMaskRhomb;
    ModuleState.Actions.DrawBackSurfaceRhombsPaletteSpriteA                                = FUN_100043a0; // TODO
    ModuleState.Actions.DrawBackSurfaceRhombsPaletteSpriteB                                = FUN_100046c6; // TODO
    ModuleState.Actions.DrawBackSurfaceRhombsPaletteShadedSprite                                = FUN_100049f6; // TODO
    ModuleState.Actions.DrawBackSurfacePaletteShadeSprite           = DrawBackSurfacePaletteShadeSprite;
    ModuleState.Actions.DrawBackSurfacePaletteSpriteAndStencil                                = FUN_10005ad6; // TODO
    ModuleState.Actions.DrawBackSurfacePalletteSprite                                = FUN_100057ac; // TODO
    ModuleState.Actions.DrawBackSurfaceShadowSprite                 = DrawBackSurfaceShadowSprite;
    ModuleState.Actions.WriteBackSurfaceMainSurfaceRectangle        = WriteBackSurfaceMainSurfaceRectangle;
    ModuleState.Actions.DrawBackSurfaceColorPoint                   = DrawBackSurfaceColorPoint;
    ModuleState.Actions.ShadeMainSurfaceRhomb                       = ShadeMainSurfaceRhomb;
    ModuleState.Actions.CleanMainSurfaceRhomb                       = CleanMainSurfaceRhomb;
    ModuleState.Actions.DrawMainSurfacePaletteSprite                = DrawMainSurfacePaletteSpriteCompact;
    ModuleState.Actions.DrawMainSurfaceSprite                       = DrawMainSurfaceSprite;
    ModuleState.Actions.DrawMainSurfacePaletteBlendSprite           = DrawMainSurfacePaletteBlendSprite;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteStencil         = DrawMainSurfacePaletteSpriteStencil;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteFrontStencil    = DrawMainSurfacePaletteSpriteFrontStencil;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteBackStencil     = DrawMainSurfacePaletteSpriteBackStencil;
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteStencil       = DrawMainSurfaceAnimationSpriteStencil;
    ModuleState.Actions.DrawMainSurfaceAnimationSprite                                = FUN_100067bd; // TODO
    ModuleState.Actions.DrawMainSurfaceShadowSprite                 = DrawMainSurfaceShadowSprite;
    ModuleState.Actions.DrawMainSurfaceActualSprite      = DrawMainSurfaceActualSprite; // TODO
    ModuleState.Actions.DrawMainSurfaceAdjustedSprite                                = FUN_10007bf8; // TODO
    ModuleState.Actions.DrawMainSurfaceVanishingPaletteSprite                                = FUN_100053d3; // TODO
    ModuleState.Actions.DrawMainSurfaceColorPoint                   = DrawMainSurfaceColorPoint;
    ModuleState.Actions.DrawMainSurfaceColorRectangle               = DrawMainSurfaceColorRectangle;
    ModuleState.Actions.DrawMainSurfaceColorBox                     = DrawMainSurfaceColorBox;
    ModuleState.Actions.DrawMainSurfaceHorizontalColorLine          = DrawMainSurfaceHorizontalColorLine;
    ModuleState.Actions.DrawMainSurfaceVerticalColorLine            = DrawMainSurfaceVerticalColorLine;
    ModuleState.Actions.DrawMainSurfaceColorShadeRectangle          = DrawMainSurfaceColorShadeRectangle;
    ModuleState.Actions.DrawMainSurfaceColorOutline                 = DrawMainSurfaceColorOutline;
    ModuleState.Actions.DrawMainSurfaceColorEllipse                 = DrawMainSurfaceColorEllipse;
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceWithFogOfWar                                = FUN_10002ba0; // TODO
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceRectangle    = WriteMainSurfaceRendererSurfaceRectangle;
    ModuleState.Actions.ReadMainSurfaceSurfaceRectangle             = ReadMainSurfaceSurfaceRectangle;
    ModuleState.Actions.MaskStencilSurfaceRectangle                 = MaskStencilSurfaceRectangle;
    ModuleState.Actions.DrawStencilSurfaceWindowRectangle           = DrawStencilSurfaceWindowRectangle;
    ModuleState.Actions.WriteRendererSurfaceSurfaceRectangle        = WriteRendererSurfaceSurfaceRectangle;
    ModuleState.Actions.WriteSurfaceSurfaceRectangle                = WriteSurfaceSurfaceRectangle;
    ModuleState.Actions.DrawUISprite                                = DrawUISprite;
    ModuleState.Actions.DrawVanishingUISprite                                = FUN_1000a503; // TODO
    ModuleState.Actions.MarkUISprite                                = FUN_10009ec3; // TODO
    ModuleState.Actions.ReleaseDirectX                              = ReleaseDirectX;

    return &ModuleState;
}
