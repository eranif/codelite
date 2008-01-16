# Microsoft Developer Studio Project File - Name="wxsqlite3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxsqlite3 - Win32 Release Multilib
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "wxsqlite3_wxsqlite3.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "wxsqlite3_wxsqlite3.mak" CFG="wxsqlite3 - Win32 Release Multilib"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "wxsqlite3 - Win32 DLL Unicode Debug Monolithic" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Unicode Debug Multilib" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Unicode Release Monolithic" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Unicode Release Multilib" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Debug Monolithic" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Debug Multilib" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Release Monolithic" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 DLL Release Multilib" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxsqlite3 - Win32 Unicode Debug Monolithic" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Unicode Debug Multilib" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Unicode Release Monolithic" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Unicode Release Multilib" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Debug Monolithic" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Debug Multilib" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Release Monolithic" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "wxsqlite3 - Win32 Release Multilib" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "wxsqlite3 - Win32 DLL Unicode Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjud_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjud_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxmsw26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3ud.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxmsw26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3ud.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Unicode Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjud_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjud_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=1 /D WXSQLITE3_HAVE_CODEC=1 /D WXSQLITE3_HAVE_LOAD_EXTENSION=1 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3ud.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3ud.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Unicode Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prju_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prju_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxmsw26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3u.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxmsw26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3u.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Unicode Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prju_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prju_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3u.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3u.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjd_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjd_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxmsw26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3d.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxmsw26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3d.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjd_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjd_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "_DEBUG" /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3d.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /debug /machine:I386 /out:"..\lib\wxsqlite3d.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prj_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prj_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxmsw26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxmsw26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 DLL Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prj_dll\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prj_dll\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /D "WXMAKINGDLL_WXSQLITE3" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
# ADD RSC /l 0x409 /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\include" /i "..\sqlite3\include" /d "WXUSINGDLL" /d "__WXMSW__" /d "WXMAKINGDLL_WXSQLITE3" /d wxUSE_DYNAMIC_SQLITE3_LOAD=0 /d WXSQLITE3_HAVE_METADATA=0 /d WXSQLITE3_HAVE_CODEC=0 /d WXSQLITE3_HAVE_LOAD_EXTENSION=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sqlite3.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"
# ADD LINK32 sqlite3.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /dll /machine:I386 /out:"..\lib\wxsqlite3.dll" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\lib" /libpath:"..\sqlite3\lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Unicode Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjud_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjud_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3ud.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3ud.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Unicode Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjud_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjud_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3ud.pdb" /FD /EHsc /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3ud.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3ud.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Unicode Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prju_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prju_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3u.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3u.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Unicode Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prju_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prju_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3u.pdb" /FD /EHsc /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3u.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3u.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjd_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjd_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3d.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3d.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prjd_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prjd_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXDEBUG__" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3d.pdb" /FD /EHsc /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3d.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3d.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prj_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prj_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3.lib"

!ELSEIF  "$(CFG)" == "wxsqlite3 - Win32 Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "msvc6prj_lib\wxsqlite3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "msvc6prj_lib\wxsqlite3"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /w /W0 /GR /O2 /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /I "..\include" /I "..\sqlite3\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_DYNAMIC_SQLITE3_LOAD=0 /D WXSQLITE3_HAVE_METADATA=0 /D WXSQLITE3_HAVE_CODEC=0 /D WXSQLITE3_HAVE_LOAD_EXTENSION=0 /Fd"..\lib\wxsqlite3.pdb" /FD /EHsc /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxsqlite3.lib"
# ADD LIB32 /nologo /out:"..\lib\wxsqlite3.lib"

!ENDIF 

# Begin Target

# Name "wxsqlite3 - Win32 DLL Unicode Debug Monolithic"
# Name "wxsqlite3 - Win32 DLL Unicode Debug Multilib"
# Name "wxsqlite3 - Win32 DLL Unicode Release Monolithic"
# Name "wxsqlite3 - Win32 DLL Unicode Release Multilib"
# Name "wxsqlite3 - Win32 DLL Debug Monolithic"
# Name "wxsqlite3 - Win32 DLL Debug Multilib"
# Name "wxsqlite3 - Win32 DLL Release Monolithic"
# Name "wxsqlite3 - Win32 DLL Release Multilib"
# Name "wxsqlite3 - Win32 Unicode Debug Monolithic"
# Name "wxsqlite3 - Win32 Unicode Debug Multilib"
# Name "wxsqlite3 - Win32 Unicode Release Monolithic"
# Name "wxsqlite3 - Win32 Unicode Release Multilib"
# Name "wxsqlite3 - Win32 Debug Monolithic"
# Name "wxsqlite3 - Win32 Debug Multilib"
# Name "wxsqlite3 - Win32 Release Monolithic"
# Name "wxsqlite3 - Win32 Release Multilib"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\src\wxsqlite3.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\include\wx\wxsqlite3.h
# End Source File
# Begin Source File

SOURCE=.\..\include\wx\wxsqlite3def.h
# End Source File
# Begin Source File

SOURCE=.\..\include\wx\wxsqlite3dyn.h
# End Source File
# Begin Source File

SOURCE=.\..\include\wx\wxsqlite3opt.h
# End Source File
# End Group
# End Target
# End Project
