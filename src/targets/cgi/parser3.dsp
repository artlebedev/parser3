# Microsoft Developer Studio Project File - Name="parser3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=parser3 - Win32 Profile Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "parser3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "parser3.mak" CFG="parser3 - Win32 Profile Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "parser3 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "parser3 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "parser3 - Win32 Profile Release" (based on "Win32 (x86) Console Application")
!MESSAGE "parser3 - Win32 Profile Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "parser3"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "parser3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /vmg /GX /Zi /O2 /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\pcre" /I "..\..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\..\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FR /FD /TP /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /map /debug /machine:I386

!ELSEIF  "$(CFG)" == "parser3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /vmg /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\pcre" /I "..\..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\..\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /FD /TP /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Debug/parser3.exe "
# SUBTRACT LINK32 /profile /pdb:none

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "parser3___Win32_Profile Release"
# PROP BASE Intermediate_Dir "parser3___Win32_Profile Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ProfileRelease"
# PROP Intermediate_Dir "ProfileRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /vmg /GX /Zi /O2 /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\pcre" /I "..\..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /I "..\..\..\..\win32xml\win32\gnome\libxslt-x.x.x" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FD /TP /c
# ADD CPP /nologo /MD /W3 /vmg /GX /Zi /O2 /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\pcre" /I "..\..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\..\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FD /TP /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /profile /machine:I386

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "parser3___Win32_Profile_Debug"
# PROP BASE Intermediate_Dir "parser3___Win32_Profile_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ProfileDebug"
# PROP Intermediate_Dir "ProfileDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /vmg /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\pcre" /I "..\..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /I "..\..\..\..\win32xml\win32\gnome\libxslt-x.x.x" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /FD /TP /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /vmg /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\lib\pcre" /I "..\..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\..\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /FD /TP /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Debug/parser3.exe "
# SUBTRACT BASE LINK32 /profile /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /profile /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"ProfileDebug/parser3.exe "

!ENDIF 

# Begin Target

# Name "parser3 - Win32 Release"
# Name "parser3 - Win32 Debug"
# Name "parser3 - Win32 Profile Release"
# Name "parser3 - Win32 Profile Debug"
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\getopt.h
# End Source File
# Begin Source File

SOURCE=.\pa_pool.C

!IF  "$(CFG)" == "parser3 - Win32 Release"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Debug"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile Release"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile Debug"

# ADD BASE CPP /GR
# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pa_threads.C
# End Source File
# Begin Source File

SOURCE=.\parser3.C
# End Source File
# Begin Source File

SOURCE=.\pool_storage.h
# End Source File
# End Target
# End Project
