# Microsoft Developer Studio Project File - Name="types" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=types - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "types.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "types.mak" CFG="types - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "types - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "types - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "types - Win32 Release"

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
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "\parser3project\win32xml\xml-xerces\c\src" /I "\parser3project\win32xml\xml-xalan\c\src" /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "types - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "types___Win32_Debug"
# PROP BASE Intermediate_Dir "types___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "types___Win32_Debug"
# PROP Intermediate_Dir "types___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "\parser3project\win32xml\xml-xerces\c\src" /I "\parser3project\win32xml\xml-xalan\c\src" /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /TP /c
# SUBTRACT CPP /YX
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

# Name "types - Win32 Release"
# Name "types - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pa_valiased.C
# End Source File
# Begin Source File

SOURCE=.\pa_vcookie.C
# End Source File
# Begin Source File

SOURCE=.\pa_vdnode.C
# End Source File
# Begin Source File

SOURCE=.\pa_vdom.C
# End Source File
# Begin Source File

SOURCE=.\pa_vfile.C
# End Source File
# Begin Source File

SOURCE=.\pa_vform.C
# End Source File
# Begin Source File

SOURCE=.\pa_vimage.C
# End Source File
# Begin Source File

SOURCE=.\pa_vrequest.C
# End Source File
# Begin Source File

SOURCE=.\pa_vstateless_class.C
# End Source File
# Begin Source File

SOURCE=.\pa_vstring.C
# End Source File
# Begin Source File

SOURCE=.\pa_vtable.C
# End Source File
# Begin Source File

SOURCE=.\pa_wcontext.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\pa_valiased.h
# End Source File
# Begin Source File

SOURCE=.\pa_value.h
# End Source File
# Begin Source File

SOURCE=.\pa_vbool.h
# End Source File
# Begin Source File

SOURCE=.\pa_vclass.h
# End Source File
# Begin Source File

SOURCE=.\pa_vcode_frame.h
# End Source File
# Begin Source File

SOURCE=.\pa_vcookie.h
# End Source File
# Begin Source File

SOURCE=.\pa_vdate.h
# End Source File
# Begin Source File

SOURCE=.\pa_vdnode.h
# End Source File
# Begin Source File

SOURCE=.\pa_vdom.h
# End Source File
# Begin Source File

SOURCE=.\pa_vdouble.h
# End Source File
# Begin Source File

SOURCE=.\pa_venv.h
# End Source File
# Begin Source File

SOURCE=.\pa_vfile.h
# End Source File
# Begin Source File

SOURCE=.\pa_vform.h
# End Source File
# Begin Source File

SOURCE=.\pa_vhash.h
# End Source File
# Begin Source File

SOURCE=.\pa_vimage.h
# End Source File
# Begin Source File

SOURCE=.\pa_vint.h
# End Source File
# Begin Source File

SOURCE=.\pa_vjunction.h
# End Source File
# Begin Source File

SOURCE=.\pa_vmath.h
# End Source File
# Begin Source File

SOURCE=.\pa_vmethod_frame.h
# End Source File
# Begin Source File

SOURCE=.\pa_vobject.h
# End Source File
# Begin Source File

SOURCE=.\pa_vrequest.h
# End Source File
# Begin Source File

SOURCE=.\pa_vresponse.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstateless_class.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstateless_object.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstring.h
# End Source File
# Begin Source File

SOURCE=.\pa_vtable.h
# End Source File
# Begin Source File

SOURCE=.\pa_vvoid.h
# End Source File
# Begin Source File

SOURCE=.\pa_wcontext.h
# End Source File
# Begin Source File

SOURCE=.\pa_wwrapper.h
# End Source File
# End Group
# Begin Group "ident"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ident.awk

!IF  "$(CFG)" == "types - Win32 Release"

USERDEP__IDENT="*.C"	
# Begin Custom Build - Collecting $Id: types.dsp,v 1.1 2001/09/26 10:32:26 parser Exp $, creating ident.C...
InputPath=..\ident.awk

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET tools=\parser3project\win32tools 
	DEL ident.C 
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new 
	%tools%\cat *.h *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=types >> ident.C.new 
	%tools%\touch 0101000001 ident.C.new 
	REN ident.C.new ident.C 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "types - Win32 Debug"

# Begin Custom Build - Collecting $Id: types.dsp,v 1.1 2001/09/26 10:32:26 parser Exp $, creating ident.C...
InputPath=..\ident.awk

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET tools=\parser3project\win32tools 
	DEL ident.C 
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new 
	%tools%\cat *.h *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=types >> ident.C.new 
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
