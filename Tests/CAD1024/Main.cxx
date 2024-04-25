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

#include "Initialize.hxx"

#include "ConvertColors.hxx"
#include "ConvertColorsExtra.hxx"
#include "DrawBackSurfaceColorPoint.hxx"
#include "DrawMainSurfaceColorBox.hxx"
#include "DrawMainSurfaceColorRectangle.hxx"
#include "DrawMainSurfaceHorizontalColorLine.hxx"
#include "DrawMainSurfaceVerticalColorLine.hxx"
#include "DrawStencilSurfaceWindowRectangle.hxx"
#include "OffsetSurfaces.hxx"
#include "SetPixelColorMasks.hxx"
#include "WriteBackSurfaceMainSurfaceRectangle.hxx"
#include "WriteMainSurfaceRendererSurfaceRectangle.hxx"
#include "WriteRendererSurfaceSurfaceRectangle.hxx"

#include <stdio.h>

#define ACTION(A, S, E)                                                                                                 \
    {                                                                                                                   \
        A(S, E); if (E->Result) { printf(#A "... [OK]\r\n");} else { printf(#A "... [ERROR] @ %s\r\n", E->Action); }    \
    }
    
static VOID Execute(LPRENDERERMODULESTATECONTAINER state, LPMODULEEVENT event)
{
    ACTION(ConvertColors, state, event);
    ACTION(ConvertColorsExtra, state, event);
    ACTION(DrawBackSurfaceColorPoint, state, event);
    ACTION(DrawMainSurfaceColorBox, state, event);
    ACTION(DrawMainSurfaceColorRectangle, state, event);
    ACTION(DrawMainSurfaceHorizontalColorLine, state, event);
    ACTION(DrawMainSurfaceVerticalColorLine, state, event);
    ACTION(DrawStencilSurfaceWindowRectangle, state, event);
    ACTION(OffsetSurfaces, state, event);
    ACTION(SetPixelColorMasks, state, event);
    ACTION(WriteBackSurfaceMainSurfaceRectangle, state, event);
    ACTION(WriteMainSurfaceRendererSurfaceRectangle, state, event);
    ACTION(WriteRendererSurfaceSurfaceRectangle, state, event);

    // TODO
}

S32 main(S32 argc, CHAR** argv)
{
    if (argc <= 1)
    {
        printf("Usage: cad1024.exe <path to module> <path to module> <path to module> ...\r\n");

        return EXIT_FAILURE;
    }

    if (!RegisterWindowClass()) { printf("[ERROR] Unable to register window class.\r\n"); return EXIT_FAILURE; }

    // Initialization.
    U32 count = 0;
    CHAR** names = (CHAR**)malloc(argc * sizeof(CHAR**));

    if (names == NULL) { return EXIT_FAILURE; }

    HMODULE* modules = (HMODULE*)malloc(argc * sizeof(HMODULE));

    if (modules == NULL) { return EXIT_FAILURE; }

    LPRENDERERMODULESTATECONTAINER* states = (LPRENDERERMODULESTATECONTAINER*)malloc(argc * sizeof(LPRENDERERMODULESTATECONTAINER*));

    if (states == NULL) { return EXIT_FAILURE; }

    // Load modules.
    for (U32 x = 1; x < argc; x++)
    {
        printf("Loading %s...   ", argv[x]);

        HMODULE module = LoadLibraryA(argv[x]);

        if (module == NULL) { printf("[ERROR]\r\n"); continue; }

        RENDERERINITACTIONLAMBDA lambda = (RENDERERINITACTIONLAMBDA)GetProcAddress(module, RENDERER_MODULE_INIT_NAME);

        if (module == NULL) { printf("[ERROR]\r\n"); FreeLibrary(module); continue; }

        LPRENDERERMODULESTATECONTAINER state = lambda();

        {
            names[count] = argv[x];
            modules[count] = module;
            states[count] = state;

            count = count + 1;
        }

        printf("[OK]\r\n");
    }

    // Status update.
    printf("Loaded %d module(s).\r\n", count);

    S32 result = EXIT_SUCCESS;

    // Execute tests.
    for (U32 x = 0; x < count; x++)
    {
        printf("Executing %s...\r\n", names[x]);

        MODULEEVENT event;
        ZeroMemory(&event, sizeof(MODULEEVENT));

        event.Name = names[x];
        event.Result = TRUE;

        Execute(states[x], &event);

        if (!event.Result) { result = EXIT_FAILURE; break; }
    }

    // Release modules.
    for (U32 x = 0; x < count; x++) { FreeLibrary(modules[x]); }

    free(names);
    free(modules);
    free(states);

    ReleaseWindowClass();

    return result;
}