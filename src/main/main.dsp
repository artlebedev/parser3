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
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
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
# ADD CPP /nologo /Za /W3 /O2 /I ".." /I "..\include" /D "NDEBUG" /D "_LIB" /D "WIN32" /FD /TP /c
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
# ADD CPP /nologo /Za /W3 /Gm /Zi /Od /I ".." /I "..\include" /D "_DEBUG" /D "_LIB" /D "WIN32" /FD /TP /GZ /c
# SUBTRACT CPP /Fr
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
# Begin Group "main"

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

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# Begin Custom Build
InputPath=.\compile.y
InputName=compile

"compile.tab.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call VCVARS32 
	d:\cygwin\bin\bison -v $(InputName).y 
	ren compile.tab.c compile.tab.C 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\compile_tools.C
# End Source File
# Begin Source File

SOURCE=.\compile_tools.h
# End Source File
# Begin Source File

SOURCE=.\core.C
# End Source File
# Begin Source File

SOURCE=.\execute.C
# End Source File
# Begin Source File

SOURCE=.\pa_array.C
# End Source File
# Begin Source File

SOURCE=.\pa_cframe.C
# End Source File
# Begin Source File

SOURCE=.\pa_common.C
# End Source File
# Begin Source File

SOURCE=.\pa_exception.C
# End Source File
# Begin Source File

SOURCE=.\pa_hash.C
# End Source File
# Begin Source File

SOURCE=.\pa_pool.C
# End Source File
# Begin Source File

SOURCE=.\pa_string.C
# End Source File
# Begin Source File

SOURCE=.\pa_table.C
# End Source File
# Begin Source File

SOURCE=.\pa_value.C
# End Source File
# Begin Source File

SOURCE=.\pa_vclass.C
# End Source File
# Begin Source File

SOURCE=.\pa_wcontext.C
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\code.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_array.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_bool.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_common.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_double.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_exception.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_hash.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_pool.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_request.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_stack.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_string.h
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

SOURCE=..\include\pa_valiased.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_value.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vbool.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vcframe.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vclass.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vdouble.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vhash.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vjunction.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vmframe.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vobject.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vstring.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_vunknown.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_wcontext.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_wwrapper.h
# End Source File
# End Group
# Begin Group "classes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\classes\root.C
# End Source File
# End Group
# End Target
# End Project
