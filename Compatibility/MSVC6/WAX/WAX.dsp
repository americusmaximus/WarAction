# Microsoft Developer Studio Project File - Name="WAX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WAX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WAX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WAX.mak" CFG="WAX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WAX - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WAX - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WAX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\SDK\DX70\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WAX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\SDK\DX70\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "WAX - Win32 Release"
# Name "WAX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\Source\WAX\App.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Basic.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\BinAsset.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\DirectDraw.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\DirectInput.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\DirectPlay.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\DirectSound.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Game.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Graphics.Basic.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Logger.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Mathematics.Basic.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Module.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Native.Basic.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Renderer.Basic.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\RendererModule.Basic.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\RendererModule.Export.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\RendererModule.Import.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Sound.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\VisualModule.Export.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\VisualModule.Import.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\WAX\Window.hxx
# End Source File
# End Group
# End Target
# End Project
