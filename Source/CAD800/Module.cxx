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

    ModuleState.Actions.BlendMainSurfaceWithFogOfWarA               = BlendMainSurfaceWithFogOfWar;
    ModuleState.Actions.BlendMainSurfaceWithFogOfWarB               = BlendMainSurfaceWithFogOfWar;
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
    ModuleState.Actions.DrawBackSurfaceRhombsPaletteSpriteA         = DrawBackSurfaceRhombsPaletteSpriteA;
    ModuleState.Actions.DrawBackSurfaceRhombsPaletteSpriteB         = DrawBackSurfaceRhombsPaletteSpriteB;
    ModuleState.Actions.DrawBackSurfaceRhombsPaletteShadedSprite    = DrawBackSurfaceRhombsPaletteShadedSprite;
    ModuleState.Actions.DrawBackSurfacePaletteShadeSprite           = DrawBackSurfacePaletteShadeSprite;
    ModuleState.Actions.DrawBackSurfacePaletteSpriteAndStencil      = DrawBackSurfacePaletteSpriteAndStencil;
    ModuleState.Actions.DrawBackSurfacePaletteSprite                = DrawBackSurfacePaletteSprite;
    ModuleState.Actions.DrawBackSurfaceShadowSprite                 = DrawBackSurfaceShadowSprite;
    ModuleState.Actions.WriteBackSurfaceMainSurfaceRectangle        = WriteBackSurfaceMainSurfaceRectangle;
    ModuleState.Actions.DrawBackSurfaceColorPoint                   = DrawBackSurfaceColorPoint;
    ModuleState.Actions.ShadeMainSurfaceRhomb                       = ShadeMainSurfaceRhomb;
    ModuleState.Actions.CleanMainSurfaceRhomb                       = CleanMainSurfaceRhomb;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteCompact         = DrawMainSurfacePaletteSpriteCompact;
    ModuleState.Actions.DrawMainSurfaceSprite                       = DrawMainSurfaceSprite;
    ModuleState.Actions.DrawMainSurfacePaletteBlendSprite           = DrawMainSurfacePaletteBlendSprite;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteStencil         = DrawMainSurfacePaletteSpriteStencil;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteFrontStencil    = DrawMainSurfacePaletteSpriteFrontStencil;
    ModuleState.Actions.DrawMainSurfacePaletteSpriteBackStencil     = DrawMainSurfacePaletteSpriteBackStencil;
    ModuleState.Actions.DrawMainSurfaceAnimationSpriteStencil       = DrawMainSurfaceAnimationSpriteStencil;
    ModuleState.Actions.DrawMainSurfaceAnimationSprite              = DrawMainSurfaceAnimationSprite;
    ModuleState.Actions.DrawMainSurfaceShadowSprite                 = DrawMainSurfaceShadowSprite;
    ModuleState.Actions.DrawMainSurfaceActualSprite                 = DrawMainSurfaceActualSprite;
    ModuleState.Actions.DrawMainSurfaceAdjustedSprite               = DrawMainSurfaceAdjustedSprite;
    ModuleState.Actions.DrawMainSurfaceVanishingPaletteSprite       = DrawMainSurfaceVanishingPaletteSprite;
    ModuleState.Actions.DrawMainSurfaceColorPoint                   = DrawMainSurfaceColorPoint;
    ModuleState.Actions.DrawMainSurfaceColorRectangle               = DrawMainSurfaceColorRectangle;
    ModuleState.Actions.DrawMainSurfaceColorBox                     = DrawMainSurfaceColorBox;
    ModuleState.Actions.DrawMainSurfaceHorizontalColorLine          = DrawMainSurfaceHorizontalColorLine;
    ModuleState.Actions.DrawMainSurfaceVerticalColorLine            = DrawMainSurfaceVerticalColorLine;
    ModuleState.Actions.DrawMainSurfaceColorShadeRectangle          = DrawMainSurfaceColorShadeRectangle;
    ModuleState.Actions.DrawMainSurfaceColorOutline                 = DrawMainSurfaceColorOutline;
    ModuleState.Actions.DrawMainSurfaceColorEllipse                 = DrawMainSurfaceColorEllipse;
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceWithFogOfWar = WriteMainSurfaceRendererSurfaceWithFogOfWar;
    ModuleState.Actions.WriteMainSurfaceRendererSurfaceRectangle    = WriteMainSurfaceRendererSurfaceRectangle;
    ModuleState.Actions.ReadMainSurfaceSurfaceRectangle             = ReadMainSurfaceSurfaceRectangle;
    ModuleState.Actions.MaskStencilSurfaceRectangle                 = MaskStencilSurfaceRectangle;
    ModuleState.Actions.DrawStencilSurfaceWindowRectangle           = DrawStencilSurfaceWindowRectangle;
    ModuleState.Actions.WriteRendererSurfaceSurfaceRectangle        = WriteRendererSurfaceSurfaceRectangle;
    ModuleState.Actions.WriteSurfaceSurfaceRectangle                = WriteSurfaceSurfaceRectangle;
    ModuleState.Actions.DrawUISprite                                = DrawUISprite;
    ModuleState.Actions.DrawVanishingUISprite                       = DrawVanishingUISprite;
    ModuleState.Actions.MarkUISprite                                = MarkUISprite;
    ModuleState.Actions.ReleaseDirectX                              = ReleaseDirectX;

    return &ModuleState;
}
