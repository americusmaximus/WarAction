# Microsoft Developer Studio Project File - Name="libvorbis" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libvorbis - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libvorbis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libvorbis.mak" CFG="libvorbis - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libvorbis - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libvorbis - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libvorbis - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBVORBIS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\SDK\vorbis\include" /I "..\..\..\SDK\ogg\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBVORBIS_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "libvorbis - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBVORBIS_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\SDK\vorbis\include" /I "..\..\..\SDK\ogg\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBVORBIS_EXPORTS" /FD /GZ  /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "libvorbis - Win32 Release"
# Name "libvorbis - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\analysis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\bitrate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\block.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\codebook.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\envelope.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\floor0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\floor1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\info.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lookup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lpc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lsp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\mapping0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\mdct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\psy.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\registry.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\res0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\sharedbook.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\smallft.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\synthesis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\win32\vorbis.def
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\vorbisenc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\window.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\backends.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\bitrate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\codebook.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\include\vorbis\codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\codec_internal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\envelope.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\floor_all.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\books\floor\floor_books.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\highlevel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lookup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lookup_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lpc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\lsp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\masking.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\mdct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\os.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\psy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\psych_11.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\psych_16.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\psych_44.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\psych_8.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\registry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\books\coupled\res_books_stereo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\books\uncoupled\res_books_uncoupled.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\residue_16.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\residue_44.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\residue_44u.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\residue_8.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\scales.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_11.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_16.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_22.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_32.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_44.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_44u.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\modes\setup_X.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\smallft.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\include\vorbis\vorbisenc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\include\vorbis\vorbisfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDK\vorbis\lib\window.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
