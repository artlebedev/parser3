# Microsoft Developer Studio Project File - Name="ApacheModuleParser3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ApacheModuleParser3 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ApacheModuleParser3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ApacheModuleParser3.mak" CFG="ApacheModuleParser3 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ApacheModuleParser3 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ApacheModuleParser3 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ApacheModuleParser3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ApacheModuleParser3R"
# PROP Intermediate_Dir "ApacheModuleParser3R"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# ADD CPP /nologo /MD /W3 /O2 /I "\apache-1.3\src\include" /I "\apache-1.3\src\os\win32" /I "..\..\..\..\include" /I "..\..\..\.." /I "..\..\..\..\types" /I "..\..\..\..\classes" /I "..\..\..\..\sql" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SHARED_MODULE" /FD /TP /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ApacheModuleParser3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ApacheModuleParser3D"
# PROP Intermediate_Dir "ApacheModuleParser3D"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "..\..\..\..\sql" /I "\apache-1.3\src\include" /I "\apache-1.3\src\os\win32" /I "..\..\..\..\include" /I "..\..\..\.." /I "..\..\..\..\types" /I "..\..\..\..\classes" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SHARED_MODULE" /FD /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 user32.lib kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /pdb:none /debug /machine:I386
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "ApacheModuleParser3 - Win32 Release"
# Name "ApacheModuleParser3 - Win32 Debug"
# Begin Source File

SOURCE=..\..\modules\extra\mod_parser3.C
# End Source File
# Begin Source File

SOURCE=..\..\modules\extra\pa_pool.C
# End Source File
# End Target
# End Project
