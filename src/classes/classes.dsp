# Microsoft Developer Studio Project File - Name="classes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CLASSES - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "classes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "classes.mak" CFG="CLASSES - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "classes - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "classes - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "classes - Win32 Profile" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "classes"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "classes - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "gd" /I "..\include" /I "..\types" /I "..\lib\pcre" /I "..\lib\md5" /I "..\sql" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\libxml2-x.x.x\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "classes - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "gd" /I "..\include" /I "..\types" /I "..\lib\pcre" /I "..\lib\md5" /I "..\sql" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\libxml2-x.x.x\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "classes - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "classes___Win32_Profile"
# PROP BASE Intermediate_Dir "classes___Win32_Profile"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Profile"
# PROP Intermediate_Dir "Profile"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "gd" /I "..\include" /I "..\types" /I "..\lib\pcre" /I "..\lib\md5" /I "..\sql" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\libxml2-x.x.x\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "gd" /I "..\include" /I "..\types" /I "..\lib\pcre" /I "..\lib\md5" /I "..\sql" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\libxml2-x.x.x\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "classes - Win32 Release"
# Name "classes - Win32 Debug"
# Name "classes - Win32 Profile"
# Begin Source File

SOURCE=.\classes.awk

!IF  "$(CFG)" == "classes - Win32 Release"

# Begin Custom Build - Scanning classes/, creating classes.inc...
InputPath=.\classes.awk

"..\classes\classes.inc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET CLASSES_INC=..\classes\classes.inc 
	ECHO // do not edit. autogenerated by .dsp, see classes.awk settings > %CLASSES_INC% 
	\parser3project\win32\tools\ls | \parser3project\win32\tools\gawk -f $(InputPath) >> %CLASSES_INC% 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "classes - Win32 Debug"

# Begin Custom Build - Scanning classes/, creating classes.inc...
InputPath=.\classes.awk

"..\classes\classes.inc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET CLASSES_INC=..\classes\classes.inc 
	ECHO // do not edit. autogenerated by .dsp, see classes.awk settings > %CLASSES_INC% 
	\parser3project\win32\tools\ls | \parser3project\win32\tools\gawk -f $(InputPath) >> %CLASSES_INC% 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "classes - Win32 Profile"

# Begin Custom Build - Scanning classes/, creating classes.inc...
InputPath=.\classes.awk

"..\classes\classes.inc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET CLASSES_INC=..\classes\classes.inc 
	ECHO // do not edit. autogenerated by .dsp, see classes.awk settings > %CLASSES_INC% 
	\parser3project\win32\tools\ls | \parser3project\win32\tools\gawk -f $(InputPath) >> %CLASSES_INC% 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\classes.C
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\date.C
# End Source File
# Begin Source File

SOURCE=.\double.C
# End Source File
# Begin Source File

SOURCE=.\file.C
# End Source File
# Begin Source File

SOURCE=.\form.C
# End Source File
# Begin Source File

SOURCE=.\hash.C
# End Source File
# Begin Source File

SOURCE=.\image.C
# End Source File
# Begin Source File

SOURCE=.\int.C
# End Source File
# Begin Source File

SOURCE=.\mail.C
# End Source File
# Begin Source File

SOURCE=.\math.C
# End Source File
# Begin Source File

SOURCE=.\op.C
# End Source File
# Begin Source File

SOURCE=.\response.C
# End Source File
# Begin Source File

SOURCE=.\string.C
# End Source File
# Begin Source File

SOURCE=.\table.C
# End Source File
# Begin Source File

SOURCE=.\void.C
# End Source File
# Begin Source File

SOURCE=.\xdoc.C
# End Source File
# Begin Source File

SOURCE=.\xnode.C
# End Source File
# Begin Source File

SOURCE=.\xnode.h
# End Source File
# End Target
# End Project
