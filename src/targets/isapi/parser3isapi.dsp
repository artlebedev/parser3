# Microsoft Developer Studio Project File - Name="parser3isapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=parser3isapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "parser3isapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "parser3isapi.mak" CFG="parser3isapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "parser3isapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "parser3isapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "parser3isapi"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "parser3isapi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PARSER3ISAPI_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\sql" /I "..\..\..\..\include" /I "..\..\..\.." /I "..\..\..\..\types" /I "..\..\..\..\classes" /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\pcre" /I "\parser3project\win32xml\glib\include" /I "\parser3project\win32xml\glib\include\glib" /I "\parser3project\win32xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\libxml2-x.x.x\include" /I "\parser3project\win32xml\libxslt-x.x.x" /D "WIN32" /FD /TP /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /machine:I386 /out:"W:\nestle\parser3\parser3isapi.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Stopping inetinfo...
PreLink_Cmds=CD \parser3project\win32tools	istop
PostBuild_Desc=Starting inetinfo...
PostBuild_Cmds=CD \parser3project\win32tools	istart
# End Special Build Tool

!ELSEIF  "$(CFG)" == "parser3isapi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PARSER3ISAPI_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\pcre" /I "\parser3project\win32xml\glib\include" /I "\parser3project\win32xml\glib\include\glib" /I "\parser3project\win32xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\libxml2-x.x.x\include" /I "\parser3project\win32xml\libxslt-x.x.x" /D "_DEBUG" /D "WIN32" /FD /GZ /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"W:\nestle\parser3\parser3isapi.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Stopping inetinfo...
PreLink_Cmds=CD \parser3project\win32tools	istop
PostBuild_Desc=Starting inetinfo...
PostBuild_Cmds=CD \parser3project\win32tools
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "parser3isapi - Win32 Release"
# Name "parser3isapi - Win32 Debug"
# Begin Group "ident"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ident.awk

!IF  "$(CFG)" == "parser3isapi - Win32 Release"

# Begin Custom Build - Up-to-dating ident.C...
InputPath=..\..\ident.awk

BuildCmds= \
	SET tools=\parser3project\win32tools \
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new \
	REN ident.C ident.C.old  >nul 2>&1 \
	%tools%\cat *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=parser3isapi >> ident.C.new \
	REN ident.C.old ident.C  >nul 2>&1 \
	%tools%\diff ident.C ident.C.new  >nul 2>&1 \
	IF ERRORLEVEL 1 COPY ident.C.new ident.C  >nul 2>&1 \
	%tools%\touch 0101000001 ident.C.new \
	

"ident.C.new" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "parser3isapi - Win32 Debug"

# Begin Custom Build - Up-to-dating ident.C...
InputPath=..\..\ident.awk

BuildCmds= \
	SET tools=\parser3project\win32tools \
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new \
	REN ident.C ident.C.old  >nul 2>&1 \
	%tools%\cat *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=parser3isapi >> ident.C.new \
	REN ident.C.old ident.C  >nul 2>&1 \
	%tools%\diff ident.C ident.C.new  >nul 2>&1 \
	IF ERRORLEVEL 1 COPY ident.C.new ident.C  >nul 2>&1 \
	%tools%\touch 0101000001 ident.C.new \
	

"ident.C.new" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ident.C
# End Source File
# Begin Source File

SOURCE=.\ident.C.new
# End Source File
# End Group
# Begin Source File

SOURCE=.\pa_pool.C
# End Source File
# Begin Source File

SOURCE=.\pa_threads.C
# End Source File
# Begin Source File

SOURCE=.\parser3isapi.C

!IF  "$(CFG)" == "parser3isapi - Win32 Release"

# ADD CPP /GR- /GX

!ELSEIF  "$(CFG)" == "parser3isapi - Win32 Debug"

# ADD CPP /GX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\parser3isapi.def
# End Source File
# Begin Source File

SOURCE=.\pool_storage.h
# End Source File
# End Target
# End Project
