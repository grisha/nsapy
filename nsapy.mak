# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=nsapipy - Win32 ns httpd 20 Release
!MESSAGE No configuration specified.  Defaulting to nsapipy - Win32 ns httpd 20\
 Release.
!ENDIF 

!IF "$(CFG)" != "nsapipy - Win32 ns httpd 30 Debug" && "$(CFG)" !=\
 "nsapipy - Win32 ns httpd 30 Release" && "$(CFG)" !=\
 "nsapipy - Win32 ns httpd 20 Debug" && "$(CFG)" !=\
 "nsapipy - Win32 ns httpd 20 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "nsapy.mak" CFG="nsapipy - Win32 ns httpd 20 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nsapipy - Win32 ns httpd 30 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nsapipy - Win32 ns httpd 30 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nsapipy - Win32 ns httpd 20 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nsapipy - Win32 ns httpd 20 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "nsapipy - Win32 ns httpd 30 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nsapipy - Win32 ns httpd 30 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "nsapipy0"
# PROP BASE Intermediate_Dir "nsapipy0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug-30"
# PROP Intermediate_Dir "Debug-30"
# PROP Target_Dir ""
OUTDIR=.\Debug-30
INTDIR=.\Debug-30

ALL : "$(OUTDIR)\_nsapy30.dll" "$(OUTDIR)\nsapy.bsc"

CLEAN : 
	-@erase "$(INTDIR)\NSAPIMOD.OBJ"
	-@erase "$(INTDIR)\NSAPIMOD.SBR"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\_nsapy30.dll"
	-@erase "$(OUTDIR)\_nsapy30.exp"
	-@erase "$(OUTDIR)\_nsapy30.ilk"
	-@erase "$(OUTDIR)\_nsapy30.lib"
	-@erase "$(OUTDIR)\_nsapy30.pdb"
	-@erase "$(OUTDIR)\nsapy.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\suitespot\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "XP_WIN32" /D "PYNSAPIDEBUG" /FR /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\suitespot\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "_DEBUG" /D "PYNSAPIDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /FR /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\suitespot\include" /I\
 "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "_DEBUG" /D\
 "PYNSAPIDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/nsapy.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=".\Debug-30/"
CPP_SBRS=".\Debug-30/"
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/nsapy.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\NSAPIMOD.SBR"

"$(OUTDIR)\nsapy.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/_nsapy.dll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug-30/_nsapy30.dll"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/_nsapy30.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/_nsapy30.dll" /implib:"$(OUTDIR)/_nsapy30.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NSAPIMOD.OBJ"

"$(OUTDIR)\_nsapy30.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 30 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "nsapipy_"
# PROP BASE Intermediate_Dir "nsapipy_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release-30"
# PROP Intermediate_Dir "Release-30"
# PROP Target_Dir ""
OUTDIR=.\Release-30
INTDIR=.\Release-30

ALL : "$(OUTDIR)\_nsapy30.dll"

CLEAN : 
	-@erase "$(INTDIR)\NSAPIMOD.OBJ"
	-@erase "$(OUTDIR)\_nsapy30.dll"
	-@erase "$(OUTDIR)\_nsapy30.exp"
	-@erase "$(OUTDIR)\_nsapy30.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "E:\netscape\suitespot\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "XP_WIN32" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "E:\netscape\suitespot\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "E:\netscape\suitespot\include" /I\
 "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "XP_WIN32" /Fp"$(INTDIR)/nsapy.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=".\Release-30/"
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/nsapy.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/_nsapy.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release-30/_nsapy30.dll"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/_nsapy30.pdb" /machine:I386\
 /out:"$(OUTDIR)/_nsapy30.dll" /implib:"$(OUTDIR)/_nsapy30.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NSAPIMOD.OBJ"

"$(OUTDIR)\_nsapy30.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 20 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "nsapipy1"
# PROP BASE Intermediate_Dir "nsapipy1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug-20"
# PROP Intermediate_Dir "Debug-20"
# PROP Target_Dir ""
OUTDIR=.\Debug-20
INTDIR=.\Debug-20

ALL : "$(OUTDIR)\_nsapy20.dll" "$(OUTDIR)\nsapy.bsc"

CLEAN : 
	-@erase "$(INTDIR)\NSAPIMOD.OBJ"
	-@erase "$(INTDIR)\NSAPIMOD.SBR"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\_nsapy20.dll"
	-@erase "$(OUTDIR)\_nsapy20.exp"
	-@erase "$(OUTDIR)\_nsapy20.ilk"
	-@erase "$(OUTDIR)\_nsapy20.lib"
	-@erase "$(OUTDIR)\_nsapy20.pdb"
	-@erase "$(OUTDIR)\nsapy.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\suitespot\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "XP_WIN32" /D "PYNSAPIDEBUG" /FR /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\server\nsapi\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "_DEBUG" /D "PYNSAPIDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /FR /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\server\nsapi\include"\
 /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "_DEBUG"\
 /D "PYNSAPIDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/nsapy.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=".\Debug-20/"
CPP_SBRS=".\Debug-20/"
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/nsapy.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\NSAPIMOD.SBR"

"$(OUTDIR)\nsapy.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/_nsapy.dll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib libhttpd.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug-20/_nsapy20.dll"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib python14.lib libhttpd.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/_nsapy20.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/_nsapy20.dll" /implib:"$(OUTDIR)/_nsapy20.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NSAPIMOD.OBJ"

"$(OUTDIR)\_nsapy20.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 20 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "nsapipy2"
# PROP BASE Intermediate_Dir "nsapipy2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release-20"
# PROP Intermediate_Dir "Release-20"
# PROP Target_Dir ""
OUTDIR=.\Release-20
INTDIR=.\Release-20

ALL : "$(OUTDIR)\_nsapy20.dll" "$(OUTDIR)\nsapy.bsc"

CLEAN : 
	-@erase "$(INTDIR)\NSAPIMOD.OBJ"
	-@erase "$(INTDIR)\NSAPIMOD.SBR"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\_nsapy20.dll"
	-@erase "$(OUTDIR)\_nsapy20.exp"
	-@erase "$(OUTDIR)\_nsapy20.lib"
	-@erase "$(OUTDIR)\nsapy.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\suitespot\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "XP_WIN32" /D "PYNSAPIDEBUG" /FR /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\server\nsapi\include" /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "_DEBUG" /D "PYNSAPIDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /FR /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "E:\netscape\server\nsapi\include"\
 /I "E:\devlpmnt\python-1.4\include" /I "E:\devlpmnt\python-1.4\pc" /D "_DEBUG"\
 /D "PYNSAPIDEBUG" /D "WIN32" /D "_WINDOWS" /D "XP_WIN32" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/nsapy.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=".\Release-20/"
CPP_SBRS=".\Release-20/"
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/nsapy.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\NSAPIMOD.SBR"

"$(OUTDIR)\nsapy.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib ns-httpd30.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/_nsapy.dll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python14.lib libhttpd.lib /nologo /subsystem:windows /dll /incremental:no /machine:I386 /out:"Release-20/_nsapy20.dll"
# SUBTRACT LINK32 /pdb:none /debug
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib python14.lib libhttpd.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/_nsapy20.pdb" /machine:I386\
 /out:"$(OUTDIR)/_nsapy20.dll" /implib:"$(OUTDIR)/_nsapy20.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NSAPIMOD.OBJ"

"$(OUTDIR)\_nsapy20.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "nsapipy - Win32 ns httpd 30 Debug"
# Name "nsapipy - Win32 ns httpd 30 Release"
# Name "nsapipy - Win32 ns httpd 20 Debug"
# Name "nsapipy - Win32 ns httpd 20 Release"

!IF  "$(CFG)" == "nsapipy - Win32 ns httpd 30 Debug"

!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 30 Release"

!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 20 Debug"

!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 20 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\NSAPIMOD.C

!IF  "$(CFG)" == "nsapipy - Win32 ns httpd 30 Debug"

DEP_CPP_NSAPI=\
	"..\..\..\Netscape\SuiteSpot\include\base\systems.h"\
	"..\..\..\Netscape\SuiteSpot\include\nsapi.h"\
	"..\..\Python-1.4\Include\abstract.h"\
	"..\..\Python-1.4\Include\accessobject.h"\
	"..\..\Python-1.4\Include\allobjects.h"\
	"..\..\Python-1.4\Include\bltinmodule.h"\
	"..\..\Python-1.4\Include\ceval.h"\
	"..\..\Python-1.4\Include\classobject.h"\
	"..\..\Python-1.4\Include\cobject.h"\
	"..\..\Python-1.4\Include\complexobject.h"\
	"..\..\Python-1.4\Include\fileobject.h"\
	"..\..\Python-1.4\Include\floatobject.h"\
	"..\..\Python-1.4\Include\funcobject.h"\
	"..\..\Python-1.4\Include\import.h"\
	"..\..\Python-1.4\Include\intobject.h"\
	"..\..\Python-1.4\Include\intrcheck.h"\
	"..\..\Python-1.4\Include\listobject.h"\
	"..\..\Python-1.4\Include\longobject.h"\
	"..\..\Python-1.4\Include\mappingobject.h"\
	"..\..\Python-1.4\Include\methodobject.h"\
	"..\..\Python-1.4\Include\modsupport.h"\
	"..\..\Python-1.4\Include\moduleobject.h"\
	"..\..\Python-1.4\Include\mymalloc.h"\
	"..\..\Python-1.4\Include\myproto.h"\
	"..\..\Python-1.4\Include\object.h"\
	"..\..\Python-1.4\Include\objimpl.h"\
	"..\..\Python-1.4\Include\pydebug.h"\
	"..\..\Python-1.4\Include\pyerrors.h"\
	"..\..\Python-1.4\Include\rangeobject.h"\
	"..\..\Python-1.4\Include\rename2.h"\
	"..\..\Python-1.4\Include\sliceobject.h"\
	"..\..\Python-1.4\Include\stringobject.h"\
	"..\..\Python-1.4\Include\sysmodule.h"\
	"..\..\Python-1.4\Include\thread.h"\
	"..\..\Python-1.4\Include\traceback.h"\
	"..\..\Python-1.4\Include\tupleobject.h"\
	"\devlpmnt\python-1.4\include\Python.h"\
	"\devlpmnt\python-1.4\include\pythonrun.h"\
	"\devlpmnt\python-1.4\pc\config.h"\
	"\netscape\suitespot\include\base\crit.h"\
	"\netscape\suitespot\include\base\pblock.h"\
	"\netscape\suitespot\include\base\session.h"\
	"\netscape\suitespot\include\base\systhr.h"\
	"\netscape\suitespot\include\frame\http.h"\
	"\netscape\suitespot\include\frame\log.h"\
	"\netscape\suitespot\include\frame\protocol.h"\
	"\netscape\suitespot\include\frame\req.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\NSAPIMOD.OBJ" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"

"$(INTDIR)\NSAPIMOD.SBR" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 30 Release"

DEP_CPP_NSAPI=\
	"..\..\..\Netscape\SuiteSpot\include\base\systems.h"\
	"..\..\..\Netscape\SuiteSpot\include\nsapi.h"\
	"..\..\Python-1.4\Include\abstract.h"\
	"..\..\Python-1.4\Include\accessobject.h"\
	"..\..\Python-1.4\Include\allobjects.h"\
	"..\..\Python-1.4\Include\bltinmodule.h"\
	"..\..\Python-1.4\Include\ceval.h"\
	"..\..\Python-1.4\Include\classobject.h"\
	"..\..\Python-1.4\Include\cobject.h"\
	"..\..\Python-1.4\Include\complexobject.h"\
	"..\..\Python-1.4\Include\fileobject.h"\
	"..\..\Python-1.4\Include\floatobject.h"\
	"..\..\Python-1.4\Include\funcobject.h"\
	"..\..\Python-1.4\Include\import.h"\
	"..\..\Python-1.4\Include\intobject.h"\
	"..\..\Python-1.4\Include\intrcheck.h"\
	"..\..\Python-1.4\Include\listobject.h"\
	"..\..\Python-1.4\Include\longobject.h"\
	"..\..\Python-1.4\Include\mappingobject.h"\
	"..\..\Python-1.4\Include\methodobject.h"\
	"..\..\Python-1.4\Include\modsupport.h"\
	"..\..\Python-1.4\Include\moduleobject.h"\
	"..\..\Python-1.4\Include\mymalloc.h"\
	"..\..\Python-1.4\Include\myproto.h"\
	"..\..\Python-1.4\Include\object.h"\
	"..\..\Python-1.4\Include\objimpl.h"\
	"..\..\Python-1.4\Include\pydebug.h"\
	"..\..\Python-1.4\Include\pyerrors.h"\
	"..\..\Python-1.4\Include\rangeobject.h"\
	"..\..\Python-1.4\Include\rename2.h"\
	"..\..\Python-1.4\Include\sliceobject.h"\
	"..\..\Python-1.4\Include\stringobject.h"\
	"..\..\Python-1.4\Include\sysmodule.h"\
	"..\..\Python-1.4\Include\thread.h"\
	"..\..\Python-1.4\Include\traceback.h"\
	"..\..\Python-1.4\Include\tupleobject.h"\
	"\devlpmnt\python-1.4\include\Python.h"\
	"\devlpmnt\python-1.4\include\pythonrun.h"\
	"\devlpmnt\python-1.4\pc\config.h"\
	"\netscape\suitespot\include\base\crit.h"\
	"\netscape\suitespot\include\base\pblock.h"\
	"\netscape\suitespot\include\base\session.h"\
	"\netscape\suitespot\include\base\systhr.h"\
	"\netscape\suitespot\include\frame\http.h"\
	"\netscape\suitespot\include\frame\log.h"\
	"\netscape\suitespot\include\frame\protocol.h"\
	"\netscape\suitespot\include\frame\req.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\NSAPIMOD.OBJ" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 20 Debug"

DEP_CPP_NSAPI=\
	"..\..\..\Netscape\Server\nsapi\include\base\net.h"\
	"..\..\..\Netscape\Server\nsapi\include\base\pool.h"\
	"..\..\..\Netscape\Server\nsapi\include\base\sem.h"\
	"..\..\..\Netscape\Server\nsapi\include\base\systems.h"\
	"..\..\..\Netscape\Server\nsapi\include\version.h"\
	"..\..\Python-1.4\Include\abstract.h"\
	"..\..\Python-1.4\Include\accessobject.h"\
	"..\..\Python-1.4\Include\allobjects.h"\
	"..\..\Python-1.4\Include\bltinmodule.h"\
	"..\..\Python-1.4\Include\ceval.h"\
	"..\..\Python-1.4\Include\classobject.h"\
	"..\..\Python-1.4\Include\cobject.h"\
	"..\..\Python-1.4\Include\complexobject.h"\
	"..\..\Python-1.4\Include\fileobject.h"\
	"..\..\Python-1.4\Include\floatobject.h"\
	"..\..\Python-1.4\Include\funcobject.h"\
	"..\..\Python-1.4\Include\import.h"\
	"..\..\Python-1.4\Include\intobject.h"\
	"..\..\Python-1.4\Include\intrcheck.h"\
	"..\..\Python-1.4\Include\listobject.h"\
	"..\..\Python-1.4\Include\longobject.h"\
	"..\..\Python-1.4\Include\mappingobject.h"\
	"..\..\Python-1.4\Include\methodobject.h"\
	"..\..\Python-1.4\Include\modsupport.h"\
	"..\..\Python-1.4\Include\moduleobject.h"\
	"..\..\Python-1.4\Include\mymalloc.h"\
	"..\..\Python-1.4\Include\myproto.h"\
	"..\..\Python-1.4\Include\object.h"\
	"..\..\Python-1.4\Include\objimpl.h"\
	"..\..\Python-1.4\Include\pydebug.h"\
	"..\..\Python-1.4\Include\pyerrors.h"\
	"..\..\Python-1.4\Include\rangeobject.h"\
	"..\..\Python-1.4\Include\rename2.h"\
	"..\..\Python-1.4\Include\sliceobject.h"\
	"..\..\Python-1.4\Include\stringobject.h"\
	"..\..\Python-1.4\Include\sysmodule.h"\
	"..\..\Python-1.4\Include\thread.h"\
	"..\..\Python-1.4\Include\traceback.h"\
	"..\..\Python-1.4\Include\tupleobject.h"\
	"\devlpmnt\python-1.4\include\Python.h"\
	"\devlpmnt\python-1.4\include\pythonrun.h"\
	"\devlpmnt\python-1.4\pc\config.h"\
	"\netscape\server\nsapi\include\base\buffer.h"\
	"\netscape\server\nsapi\include\base\crit.h"\
	"\netscape\server\nsapi\include\base\ereport.h"\
	"\netscape\server\nsapi\include\base\file.h"\
	"\netscape\server\nsapi\include\base\pblock.h"\
	"\netscape\server\nsapi\include\base\session.h"\
	"\netscape\server\nsapi\include\base\systhr.h"\
	"\netscape\server\nsapi\include\frame\http.h"\
	"\netscape\server\nsapi\include\frame\log.h"\
	"\netscape\server\nsapi\include\frame\object.h"\
	"\netscape\server\nsapi\include\frame\objset.h"\
	"\netscape\server\nsapi\include\frame\protocol.h"\
	"\netscape\server\nsapi\include\frame\req.h"\
	"\netscape\server\nsapi\include\netsite.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\NSAPIMOD.OBJ" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"

"$(INTDIR)\NSAPIMOD.SBR" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nsapipy - Win32 ns httpd 20 Release"

DEP_CPP_NSAPI=\
	"..\..\..\Netscape\Server\nsapi\include\base\net.h"\
	"..\..\..\Netscape\Server\nsapi\include\base\sem.h"\
	"..\..\..\Netscape\Server\nsapi\include\base\systems.h"\
	"..\..\..\Netscape\Server\nsapi\include\version.h"\
	"..\..\Python-1.4\Include\abstract.h"\
	"..\..\Python-1.4\Include\accessobject.h"\
	"..\..\Python-1.4\Include\allobjects.h"\
	"..\..\Python-1.4\Include\bltinmodule.h"\
	"..\..\Python-1.4\Include\ceval.h"\
	"..\..\Python-1.4\Include\classobject.h"\
	"..\..\Python-1.4\Include\cobject.h"\
	"..\..\Python-1.4\Include\complexobject.h"\
	"..\..\Python-1.4\Include\fileobject.h"\
	"..\..\Python-1.4\Include\floatobject.h"\
	"..\..\Python-1.4\Include\funcobject.h"\
	"..\..\Python-1.4\Include\import.h"\
	"..\..\Python-1.4\Include\intobject.h"\
	"..\..\Python-1.4\Include\intrcheck.h"\
	"..\..\Python-1.4\Include\listobject.h"\
	"..\..\Python-1.4\Include\longobject.h"\
	"..\..\Python-1.4\Include\mappingobject.h"\
	"..\..\Python-1.4\Include\methodobject.h"\
	"..\..\Python-1.4\Include\modsupport.h"\
	"..\..\Python-1.4\Include\moduleobject.h"\
	"..\..\Python-1.4\Include\mymalloc.h"\
	"..\..\Python-1.4\Include\myproto.h"\
	"..\..\Python-1.4\Include\object.h"\
	"..\..\Python-1.4\Include\objimpl.h"\
	"..\..\Python-1.4\Include\pydebug.h"\
	"..\..\Python-1.4\Include\pyerrors.h"\
	"..\..\Python-1.4\Include\rangeobject.h"\
	"..\..\Python-1.4\Include\rename2.h"\
	"..\..\Python-1.4\Include\sliceobject.h"\
	"..\..\Python-1.4\Include\stringobject.h"\
	"..\..\Python-1.4\Include\sysmodule.h"\
	"..\..\Python-1.4\Include\thread.h"\
	"..\..\Python-1.4\Include\traceback.h"\
	"..\..\Python-1.4\Include\tupleobject.h"\
	"\devlpmnt\python-1.4\include\Python.h"\
	"\devlpmnt\python-1.4\include\pythonrun.h"\
	"\devlpmnt\python-1.4\pc\config.h"\
	"\netscape\server\nsapi\include\base\buffer.h"\
	"\netscape\server\nsapi\include\base\crit.h"\
	"\netscape\server\nsapi\include\base\ereport.h"\
	"\netscape\server\nsapi\include\base\file.h"\
	"\netscape\server\nsapi\include\base\pblock.h"\
	"\netscape\server\nsapi\include\base\session.h"\
	"\netscape\server\nsapi\include\base\systhr.h"\
	"\netscape\server\nsapi\include\frame\http.h"\
	"\netscape\server\nsapi\include\frame\log.h"\
	"\netscape\server\nsapi\include\frame\object.h"\
	"\netscape\server\nsapi\include\frame\objset.h"\
	"\netscape\server\nsapi\include\frame\protocol.h"\
	"\netscape\server\nsapi\include\frame\req.h"\
	"\netscape\server\nsapi\include\netsite.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\NSAPIMOD.OBJ" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"

"$(INTDIR)\NSAPIMOD.SBR" : $(SOURCE) $(DEP_CPP_NSAPI) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
