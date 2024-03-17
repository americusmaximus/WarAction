# War Action

## Project
War Action is an open source implementation of Fireglow Games' [Sudden Strike](https://en.wikipedia.org/wiki/Sudden_Strike) from 2000, with the 1.2.1 patch.

## Goals
There are multiple goals this project tries to achieve:
1. Learn how games were made in an era when things had to be done in the code, and not through fancy game engines like today.
2. Have a fully playable game implemented end-to-end, including resource management, audio, and video rendering, as well as support of large screen resolutions.
3. Eventually, to support 64-bit compilation for modern systems, and provide an avenue for development of modern renderer implementations such as DirectX or Vulkan, and, eventually, to port to Linux or Mac.

## Requirements & Dependencies
1. [Microsoft Visual Studio](https://visualstudio.microsoft.com/downloads/)
2. [OGG](https://github.com/xiph/ogg)
3. [Vorbis](https://github.com/xiph/vorbis)

## Notes
1. I used GOG.com's Sudden Strike Gold 1.2.1 version [13578](https://www.google.com/search?q=setup_sudden_strike_1.21_(13578).exe) of the game.
2. The game is structured in a way so that the executable is just a glue for separate menu and game modules, which are in their turn can use different renderer modules.
3. The GOG.com's version of the game uses Ogg Vorbis for the game's music. It looks like this was a recent change and a requirement for the game publishing, I believe original game shipped with mp3 files. In order to support Ogg Vorbis, the games has its own WinMM.dll library to support the file format, otherwise it is mostly a wrapper of the system library.

### Compatibility, Code Style & Standards
In order to be able to run the code not only in the modern operating systems, but also in legacy environments, such as Windows XP, the code base has to abstain from usage of modern C++ language features.

Modern Visual Studio makes the binary dependent on modern runtime libraries that are not available in the legacy systems, therefore the game won't work. In order to make the code work on legacy systems please see [detailed instructions on compatibility](COMPATIBILITY.MD).

## Thanks
1. [Hifi](https://github.com/hifi-unmaintained/ogg-winmm) for the original implementation of WinMM.dll with the Ogg Vorbis support. Even though the game's version is very customized, access to the original code was very useful.

## Legal
1. This is not a complete game. Please purchase software you like!
2. The source code in this repository is mostly produced by reverse engineering the original binaries. There are a couple of exceptions for reverse engineering under DMCA -- documentation, interoperability, fair use. See goals section for the interoperability and fair use cases. The documentation is needed to support those. Also please see an article about [software preservation](https://en.wikipedia.org/wiki/Digital_preservation).
3. Fireglow Games, Sudden Strike, Ogg Vorbis, DirectX, OpenGL, Vulkan, and others are trademarks of their respective owners.
