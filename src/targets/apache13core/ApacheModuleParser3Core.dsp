# Microsoft Developer Studio Project File - Name="ApacheModuleParser3Core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ApacheModuleParser3Core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ApacheModuleParser3Core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ApacheModuleParser3Core.mak" CFG="ApacheModuleParser3Core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ApacheModuleParser3Core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ApacheModuleParser3Core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ApacheModuleParser3Core"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ApacheModuleParser3Core - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\apache13" /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\gc\include" /I "..\..\lib\cord\include" /I "..\..\lib\pcre" /I "..\..\lib\md5" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\gnome-xml\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /TP /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ApacheModuleParser3Core - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ApacheModuleParser3Core___Win32_Debug"
# PROP BASE Intermediate_Dir "ApacheModuleParser3Core___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\apache13" /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\gc\include" /I "..\..\lib\cord\include" /I "..\..\lib\pcre" /I "..\..\lib\md5" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\gnome-xml\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /TP /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ApacheModuleParser3Core - Win32 Release"
# Name "ApacheModuleParser3Core - Win32 Debug"
# Begin Source File

SOURCE=.\mod_parser3.C
# End Source File
# Begin Source File

SOURCE=.\pa_threads.C
# End Source File
# End Target
# End Project
