# Compatibility
The original game was written and compiled in 2000, which these days create some inconveniences, not to mention that newer compilers use modern runtime libraries, which also isn't great when attempting to run the new code on the old platforms, such as Windows XP. This document goes over developer and runtime configuration in order to be able to run the game on Windows XP.

![Windows XP](Extras/WinXP.PNG)

## Microsoft Visual C++ 6

### Requirements
1. [Microsoft Visual C++ 6](https://archive.org/details/en_vs6_ent)
2. [Microsoft DirectX 7.0 SDK](https://archive.org/details/dx7sdk-7001)
3. [OGG](https://github.com/xiph/ogg) and [Vorbis](https://github.com/xiph/vorbis)

### Build & Use
1. Install DirectX 7 SDK.
2. Copy `Include` and `Lib` folders from the SDK to `SDK/70`.
3. Open Visual Studio solution `WarAction.dsw` under `Compatibility/MSVC6`.
4. Compile the solution.
5. Pick a file you need in the appropriate sub folder, rename appropriately, and copy to the game folder.
6. Enjoy!

## Thanks
1. [Archive.org](https://archive.org/) for providing [Microsoft Visual C++ 6](https://archive.org/details/en_vs6_ent) and [Microsoft DirectX 7.0 SDK](https://archive.org/details/dx7sdk-7001).

## Legal
Sudden Strike, Windows, Visual Studio, Watcom, DirectX, OpenGL, Vulkan, and others are trademarks of their respective owners.
