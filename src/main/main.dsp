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
# ADD CPP /nologo /MD /W3 /O2 /I "..\libltdl" /I "..\include" /I "..\types" /I "..\classes" /I "..\classes\gd" /I "..\pcre" /I "..\sql" /D "NDEBUG" /D "_LIB" /D "WIN32" /FD /TP /c
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
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "..\libltdl" /I "..\classes\gd" /I "..\include" /I "..\types" /I "..\classes" /I "..\pcre" /I "..\sql" /D "_DEBUG" /D "_LIB" /D "WIN32" /FD /TP /GZ /c
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
# Begin Group "include"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=..\include\pa_table.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_threads.h
# End Source File
# Begin Source File

SOURCE=..\include\pa_types.h
# End Source File
# End Group
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

# Begin Custom Build - Compiling grammar...
InputDir=.
InputPath=.\compile.y
InputName=compile

"compile.tab.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(InputDir) 
	..\win32tools\bison -v $(InputName).y -o compile.tab.C 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# Begin Custom Build - Compiling grammar...
InputDir=.
InputPath=.\compile.y
InputName=compile

"compile.tab.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(InputDir) 
	..\win32tools\bison -v $(InputName).y -o compile.tab.C 
	
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

SOURCE=.\execute.C
# End Source File
# Begin Source File

SOURCE=.\pa_array.C
# End Source File
# Begin Source File

SOURCE=.\pa_common.C
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

SOURCE=.\untaint.C
# End Source File
# End Group
# Begin Group "types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\types\pa_valiased.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_valiased.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_value.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vbool.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vclass.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vcode_frame.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vcookie.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vcookie.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vdouble.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_venv.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vfile.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vfile.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vform.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vform.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vhash.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vimage.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vimage.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vint.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vjunction.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vmath.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vmethod_frame.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vobject.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vrequest.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vrequest.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vresponse.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vstateless_class.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vstateless_class.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vstateless_object.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vstring.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_vstring.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vtable.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_vvoid.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_wcontext.C
# End Source File
# Begin Source File

SOURCE=..\types\pa_wcontext.h
# End Source File
# Begin Source File

SOURCE=..\types\pa_wwrapper.h
# End Source File
# End Group
# Begin Group "classes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\classes\classes.awk

!IF  "$(CFG)" == "main - Win32 Release"

# Begin Custom Build - Scanning classes/, creating classes.inc...
InputPath=..\classes\classes.awk

"..\classes\classes.inc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\classes 
	classes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

# Begin Custom Build - Scanning classes/, creating classes.inc...
InputPath=..\classes\classes.awk

"..\classes\classes.inc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\classes 
	classes 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\classes\classes.C
# End Source File
# Begin Source File

SOURCE=..\classes\classes.h
# End Source File
# Begin Source File

SOURCE=..\classes\double.C
# End Source File
# Begin Source File

SOURCE=..\classes\file.C
# End Source File
# Begin Source File

SOURCE=..\classes\form.C
# End Source File
# Begin Source File

SOURCE=..\classes\hash.C
# End Source File
# Begin Source File

SOURCE=..\classes\image.C
# End Source File
# Begin Source File

SOURCE=..\classes\int.C
# End Source File
# Begin Source File

SOURCE=..\classes\mail.C
# End Source File
# Begin Source File

SOURCE=..\classes\math.C
# End Source File
# Begin Source File

SOURCE=..\classes\op.C
# End Source File
# Begin Source File

SOURCE=..\classes\response.C
# End Source File
# Begin Source File

SOURCE=..\classes\string.C
# End Source File
# Begin Source File

SOURCE=..\classes\table.C
# End Source File
# Begin Source File

SOURCE=..\classes\void.C
# End Source File
# End Group
# Begin Group "sql"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sql\pa_sql_driver.h
# End Source File
# End Group
# End Target
# End Project
