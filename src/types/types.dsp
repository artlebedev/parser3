# Microsoft Developer Studio Project File - Name="types" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=types - Win32 Profile Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "types.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "types.mak" CFG="types - Win32 Profile Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "types - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "types - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "types - Win32 Profile Release" (based on "Win32 (x86) Static Library")
!MESSAGE "types - Win32 Profile Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "types"
# PROP Scc_LocalPath "."
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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /I "..\lib\pcre" /I "..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /I "..\..\..\win32\psapi" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
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
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /I "..\lib\pcre" /I "..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /I "..\..\..\win32\psapi" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "types - Win32 Profile Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "types___Win32_Profile Release"
# PROP BASE Intermediate_Dir "types___Win32_Profile Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ProfileRelease"
# PROP Intermediate_Dir "ProfileRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /I "..\lib\pcre" /I "\parser3project\win32\psapi" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\libxml2-x.x.x\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /I "\parser3project\win32mailreceive\win32\gnome\gmime-x.x.x" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /I "..\lib\pcre" /I "..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /I "..\..\..\win32\psapi" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "types - Win32 Profile Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "types___Win32_Profile_Debug"
# PROP BASE Intermediate_Dir "types___Win32_Profile_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ProfileDebug"
# PROP Intermediate_Dir "ProfileDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /I "..\lib\pcre" /I "\parser3project\win32\psapi" /I "\parser3project\win32xml\win32\gnome\glib\include" /I "\parser3project\win32xml\win32\gnome\glib\include\glib" /I "\parser3project\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "\parser3project\win32xml\win32\gnome\libxml2-x.x.x\include" /I "\parser3project\win32xml\win32\gnome\libxslt-x.x.x" /I "\parser3project\win32mailreceive\win32\gnome\gmime-x.x.x" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /TP /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "..\classes\gd" /I "..\include" /I "..\classes" /I "..\sql" /I "..\lib\pcre" /I "..\..\..\win32xml\win32\gnome\glib\include" /I "..\..\..\win32xml\win32\gnome\glib\include\glib" /I "..\..\..\win32xml\win32\gnome\gdome2-x.x.x\libgdome" /I "..\..\..\\win32xml\win32\gnome\libxslt-x.x.x" /I "..\..\..\win32xml\win32\gnome\libxml2-x.x.x\include" /I "..\..\..\win32\psapi" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /TP /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
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
# Name "types - Win32 Profile Release"
# Name "types - Win32 Profile Debug"
# Begin Group "psapi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\win32\psapi\Psapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\win32\psapi\Psapi.Lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\pa_value.C
# End Source File
# Begin Source File

SOURCE=.\pa_value.h
# End Source File
# Begin Source File

SOURCE=.\pa_vbool.h
# End Source File
# Begin Source File

SOURCE=.\pa_vclass.C
# End Source File
# Begin Source File

SOURCE=.\pa_vclass.h
# End Source File
# Begin Source File

SOURCE=.\pa_vcode_frame.h
# End Source File
# Begin Source File

SOURCE=.\pa_vcookie.C
# End Source File
# Begin Source File

SOURCE=.\pa_vcookie.h
# End Source File
# Begin Source File

SOURCE=.\pa_vdate.h
# End Source File
# Begin Source File

SOURCE=.\pa_vdouble.h
# End Source File
# Begin Source File

SOURCE=.\pa_venv.h
# End Source File
# Begin Source File

SOURCE=.\pa_vfile.C
# End Source File
# Begin Source File

SOURCE=.\pa_vfile.h
# End Source File
# Begin Source File

SOURCE=.\pa_vform.C
# End Source File
# Begin Source File

SOURCE=.\pa_vform.h
# End Source File
# Begin Source File

SOURCE=.\pa_vhash.h
# End Source File
# Begin Source File

SOURCE=.\pa_vimage.C
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

SOURCE=.\pa_vmail.C
# End Source File
# Begin Source File

SOURCE=.\pa_vmail.h
# End Source File
# Begin Source File

SOURCE=.\pa_vmath.h
# End Source File
# Begin Source File

SOURCE=.\pa_vmethod_frame.h
# End Source File
# Begin Source File

SOURCE=.\pa_vobject.C
# End Source File
# Begin Source File

SOURCE=.\pa_vobject.h
# End Source File
# Begin Source File

SOURCE=.\pa_vrequest.C
# End Source File
# Begin Source File

SOURCE=.\pa_vrequest.h
# End Source File
# Begin Source File

SOURCE=.\pa_vresponse.C
# End Source File
# Begin Source File

SOURCE=.\pa_vresponse.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstateless_class.C
# End Source File
# Begin Source File

SOURCE=.\pa_vstateless_class.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstateless_object.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstatus.C
# End Source File
# Begin Source File

SOURCE=.\pa_vstatus.h
# End Source File
# Begin Source File

SOURCE=.\pa_vstring.C
# End Source File
# Begin Source File

SOURCE=.\pa_vstring.h
# End Source File
# Begin Source File

SOURCE=.\pa_vtable.C
# End Source File
# Begin Source File

SOURCE=.\pa_vtable.h
# End Source File
# Begin Source File

SOURCE=.\pa_vvoid.h
# End Source File
# Begin Source File

SOURCE=.\pa_vxdoc.C
# End Source File
# Begin Source File

SOURCE=.\pa_vxdoc.h
# End Source File
# Begin Source File

SOURCE=.\pa_vxnode.C
# End Source File
# Begin Source File

SOURCE=.\pa_vxnode.h
# End Source File
# Begin Source File

SOURCE=.\pa_wcontext.C
# End Source File
# Begin Source File

SOURCE=.\pa_wcontext.h
# End Source File
# Begin Source File

SOURCE=.\pa_wwrapper.h
# End Source File
# End Target
# End Project
