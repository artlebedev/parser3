# Microsoft Developer Studio Project File - Name="main" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=main - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "main.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "main.mak" CFG="main - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "main - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "main - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "main"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "main - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\libltdl" /I "..\classes\gd" /I "..\include" /I "..\types" /I "..\classes" /I "..\pcre" /I "..\sql" /I "\parser3project\win32xml\xml-xerces\c\src" /I "\parser3project\win32xml\xml-xalan\c\src" /D "NDEBUG" /D "_LIB" /D "WIN32" /FD /TP /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "\parser3project\win32xml\xml-xerces\c\src" /I "\parser3project\win32xml\xml-xalan\c\src" /I "..\libltdl" /I "..\classes\gd" /I "..\include" /I "..\types" /I "..\classes" /I "..\pcre" /I "..\sql" /D "_DEBUG" /D "_LIB" /D "WIN32" /FD /TP /GZ /c
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

# Name "main - Win32 Release"
# Name "main - Win32 Debug"
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\pa_array.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_common.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_config_fixed.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_config_includes.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_dictionary.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_dir.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_exception.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_exec.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_globals.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_hash.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_opcode.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_pool.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_request.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_sapi.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_socks.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_sql_connection.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_sql_driver_manager.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_stack.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_string.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_stylesheet_connection.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_table.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_threads.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_types.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_xslt_stylesheet_manager.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "C;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\compile.C
# End Source File
# Begin Source File

SOURCE=.\compile.tab.C
# End Source File
# Begin Source File

SOURCE=.\compile.y

!IF  "$(CFG)" == "main - Win32 Release"

# Begin Custom Build - Compiling grammar...
InputDir=.
InputPath=.\compile.y
InputName=compile

"compile.tab.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(InputDir) 
	..\..\..\win32tools\bison -v $(InputName).y -o compile.tab.C 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# Begin Custom Build - Compiling grammar...
InputDir=.
InputPath=.\compile.y
InputName=compile

"compile.tab.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(InputDir) 
	..\..\..\win32tools\bison -v $(InputName).y -o compile.tab.C 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\compile_tools.C
# End Source File
# Begin Source File

SOURCE=.\execute.C
# End Source File
# Begin Source File

SOURCE=.\pa_array.C
# End Source File
# Begin Source File

SOURCE=.\pa_common.C
# End Source File
# Begin Source File

SOURCE=.\pa_dictionary.C
# End Source File
# Begin Source File

SOURCE=.\pa_dir.C
# End Source File
# Begin Source File

SOURCE=.\pa_exception.C
# End Source File
# Begin Source File

SOURCE=.\pa_exec.C
# End Source File
# Begin Source File

SOURCE=.\pa_globals.C

!IF  "$(CFG)" == "main - Win32 Release"

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# ADD CPP /GX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pa_hash.C
# End Source File
# Begin Source File

SOURCE=.\pa_pool.C
# End Source File
# Begin Source File

SOURCE=.\pa_request.C
# End Source File
# Begin Source File

SOURCE=.\pa_socks.C
# End Source File
# Begin Source File

SOURCE=.\pa_sql_driver_manager.C
# End Source File
# Begin Source File

SOURCE=.\pa_string.C
# End Source File
# Begin Source File

SOURCE=.\pa_table.C
# End Source File
# Begin Source File

SOURCE=.\pa_xslt_stylesheet_manager.C

!IF  "$(CFG)" == "main - Win32 Release"

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# ADD CPP /GX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\untaint.C
# End Source File
# End Group
# Begin Group "sql"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sql\pa_sql_driver.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\compile_tools.h
# End Source File
# End Group
# Begin Group "ident"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ident.awk

!IF  "$(CFG)" == "main - Win32 Release"

USERDEP__IDENT="*.C"	
# Begin Custom Build - Collecting $Id: main.dsp,v 1.124 2001/09/26 10:32:26 parser Exp $, creating ident.C...
InputPath=..\ident.awk

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET tools=\parser3project\win32tools 
	DEL ident.C 
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new 
	%tools%\cat *.h *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=main >> ident.C.new 
	%tools%\touch 0101000001 ident.C.new 
	REN ident.C.new ident.C 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# Begin Custom Build - Collecting $Id: main.dsp,v 1.124 2001/09/26 10:32:26 parser Exp $, creating ident.C...
InputPath=..\ident.awk

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET tools=\parser3project\win32tools 
	DEL ident.C 
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new 
	%tools%\cat *.h *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=main >> ident.C.new 
	%tools%\touch 0101000001 ident.C.new 
	REN ident.C.new ident.C 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ident.C
# End Source File
# End Group
# End Target
# End Project
