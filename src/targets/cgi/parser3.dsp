# Microsoft Developer Studio Project File - Name="parser3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=parser3 - Win32 Profile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "parser3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "parser3.mak" CFG="parser3 - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "parser3 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "parser3 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "parser3 - Win32 Profile" (based on "Win32 (x86) Console Application")
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
# ADD CPP /nologo /MD /W3 /vmg /GX /Zi /O2 /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\pcre" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /FD /TP /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386 /out:"Y:\parser3project\parser3\www\cgi-bin\parser3.exe"

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
# ADD CPP /nologo /MDd /W3 /Gm /Gi /vmg /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\pcre" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /FD /TP /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Y:\parser3project\parser3\www\cgi-bin\parser3.exe "
# SUBTRACT LINK32 /profile /pdb:none

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "parser3___Win32_Profile"
# PROP BASE Intermediate_Dir "parser3___Win32_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Profile"
# PROP Intermediate_Dir "Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /vmg /GR /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\pcre" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /FR /FD /TP /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /vmg /GX /Zi /Od /I "..\.." /I "..\..\types" /I "..\..\include" /I "..\..\classes" /I "..\..\sql" /I "..\..\pcre" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /D "_PROFILE" /FR /FD /TP /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Y:\parser3project\parser3\www\cgi-bin\parser3.exe " /pdbtype:sept
# SUBTRACT BASE LINK32 /profile /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /profile /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Y:\parser3project\parser3\www\cgi-bin\parser3.exe "

!ENDIF 

# Begin Target

# Name "parser3 - Win32 Release"
# Name "parser3 - Win32 Debug"
# Name "parser3 - Win32 Profile"
# Begin Group "ident"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ident.awk

!IF  "$(CFG)" == "parser3 - Win32 Release"

# Begin Custom Build - Up-to-dating ident.C...
InputPath=..\..\ident.awk

BuildCmds= \
	SET tools=\parser3project\win32\tools \
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new \
	REN ident.C ident.C.old  >nul 2>&1 \
	%tools%\cat *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=parser3 >> ident.C.new \
	REN ident.C.old ident.C  >nul 2>&1 \
	%tools%\diff ident.C ident.C.new  >nul 2>&1 \
	IF ERRORLEVEL 1 COPY ident.C.new ident.C  >nul 2>&1 \
	%tools%\touch 0101000001 ident.C.new \
	

"ident.C.new" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "parser3 - Win32 Debug"

# Begin Custom Build - Up-to-dating ident.C...
InputPath=..\..\ident.awk

BuildCmds= \
	SET tools=\parser3project\win32\tools \
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new \
	REN ident.C ident.C.old  >nul 2>&1 \
	%tools%\cat *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=parser3 >> ident.C.new \
	REN ident.C.old ident.C  >nul 2>&1 \
	%tools%\diff ident.C ident.C.new  >nul 2>&1 \
	IF ERRORLEVEL 1 COPY ident.C.new ident.C  >nul 2>&1 \
	%tools%\touch 0101000001 ident.C.new \
	

"ident.C.new" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ident.C

!IF  "$(CFG)" == "parser3 - Win32 Release"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ident.C.new

!IF  "$(CFG)" == "parser3 - Win32 Release"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\pa_pool.C

!IF  "$(CFG)" == "parser3 - Win32 Release"

!ELSEIF  "$(CFG)" == "parser3 - Win32 Debug"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "parser3 - Win32 Profile"

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
