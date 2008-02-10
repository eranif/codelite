## 
## CodeLite library makefile
## Author: Eran Ifrah
## 
## This makefile was created to build CodeLite, LiteEditor and its dependencies,
## for the g++ compiler. It was tested using gmake & GCC 4.1
## Testted on Ubunto Dapper, Fedora core 4 & 5
##
## Feel free to modify it for your own use
##
## Read BuildInfo.txt for more information
 
## 
## Set default build set
## as ANSI-deubg
##
WXCFG=--unicode=no --debug=yes 
EXT=d
OBJ_DIR=Debug_gcc
DEBUG= -g 
EXE_NAME=CodeLiteD

##
## Override default settings by typing: make type=[release | release_unicode | debug_unicode]
##
## settings for unicode debug
ifeq ($(type), debug_unicode)
 EXE_NAME=CodeLiteUD
 WXCFG=--unicode=yes --debug=yes
 EXT=ud
 OBJ_DIR=Debug_gcc_unicode
 DEBUG= -g 
 OPT= -DREGEXP_UNICODE
else
 EXE_NAME=CodeLiteUR
 type=release_unicode
 export type
 WXCFG=--unicode=yes --debug=no 
 EXT=u
 OBJ_DIR=Release_gcc_unicode
 DEBUG=
 OPT=-O3 -DREGEXP_UNICODE
endif

OUTPUT_DIR=lib

PROFILER=# -pg 
##
## Define variables, using wx-config tool
##
CMP=g++ -fPIC $(DEBUG) $(OPT) 
LIBP=-L../lib

TRACE_FLAG=## Comment this if you dont want messages in log window
CCFLAGS= -D__WX__ -Wall $(TRACE_FLAG) -I. -DWXUSINGDLL -DWX_PRECOMP -DNO_GCC_PRAGMA -DXTHREADS -D_REENTRANT -DXUSE_MTSAFE_API $(shell wx-config --cxxflags $(WXCFG)) $(PROFILER) -fno-strict-aliasing -DYY_NEVER_INTERACTIVE=1 $(shell pkg-config --cflags gtk+-2.0)

LINK_FLAGS =  -L$(OUTPUT_DIR) -Lsdk/wxflatnotebook/lib -lwxflatnotebook$(EXT) -lplugin$(EXT) -lcodelite$(EXT) -lwxsqlite3$(EXT) -lwxscintilla$(EXT) -Lsdk/sqlite3/lib $(PROFILER) $(shell wx-config --libs $(WXCFG)) $(shell pkg-config --libs gtk+-2.0)

SCI_INCLUDE= -Isdk/wxscintilla/include -Isdk/wxscintilla/src/scintilla/include	 -Isdk/wxscintilla/src/scintilla/src
SQLITE_INCLUDE= -Isdk/wxsqlite3/include -Isdk/wxsqlite3/sqlite3/include
INCLUDES = -I. $(SQLITE_INCLUDE) $(SCI_INCLUDE) -IInterfaces -IDebugger -IPlugin -ICodeLite -ILiteEditor -ILiteEditor/include/ -Isdk/wxflatnotebook/include 

##
## Define the object files
##

lib_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard CodeLite/*.cpp)))))
lib_c_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard CodeLite/*.c)))))
sample_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard LiteEditor/*.cpp)))))

## 
## Scintilla related
## 
SCI_DEFS = -DGTK -DSCI_LEXER -DLINK_LEXERS
SCI_OBJ_DIR=$(OBJ_DIR)_scintilla
sci_objects1 := $(addprefix $(SCI_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxscintilla/src/scintilla/src/*.cxx)))))
sci_objects2 := $(addprefix $(SCI_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxscintilla/src/*.cpp)))))
sci_objects = $(sci_objects1) $(sci_objects2) 

##
## SQLite related
##
SQLITE_OBJ_DIR=$(OBJ_DIR)_sqlite3
sqlite_objects2 := $(addprefix $(SQLITE_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxsqlite3/src/*.cpp)))))
sqlite_objects = $(sqlite_objects1) $(sqlite_objects2) 

## our main build target
build : pre_build CodeLite Scintilla SQLite wxFlatNotebook SDK LiteEditor ctags Debugger CodeFormatter Subversion ReferenceAnalyser Gizmos cscope

CodeLite: code_lite_msg $(lib_cpp_objects) $(lib_c_objects)
	ar rcu $(OUTPUT_DIR)/libcodelite$(EXT).a $(lib_cpp_objects)  $(lib_c_objects) 
	ranlib $(OUTPUT_DIR)/libcodelite$(EXT).a

Scintilla: scintilla_msg $(sci_objects)
	ar rcu $(OUTPUT_DIR)/libwxscintilla$(EXT).a $(sci_objects)
	ranlib $(OUTPUT_DIR)/libwxscintilla$(EXT).a
	
SQLite: sqlite_msg sqlite3/Release/sqlite3.o $(sqlite_objects)
	ar rcu $(OUTPUT_DIR)/libwxsqlite3$(EXT).a $(sqlite_objects) sqlite3/Release/sqlite3.o
	ranlib $(OUTPUT_DIR)/libwxsqlite3$(EXT).a

sqlite3/Release/sqlite3.o:
	gcc -c sqlite3/sqlite3.c -o sqlite3/Release/sqlite3.o -O3
			
wxFlatNotebook: wxFlatNotebook_msg
	cd sdk/wxflatnotebook/build/wxFlatNotebook && $(MAKE)

Gizmos: gizmos_msg
	cd Gizmos && $(MAKE)

SDK: sdk_msg
	cd Plugin/ && $(MAKE)

Debugger: debugger_msg
	cd Debugger/ && $(MAKE)

Subversion: svn_msg
	cd Subversion/ && $(MAKE)

cscope: cscope_msg
	cd cscope/ && $(MAKE)

CodeFormatter: formatter_msg
	cd CodeFormatter/ && $(MAKE)

ctags: sdk/ctags/Makefile ctags_msg
	cd sdk/ctags/ && $(MAKE)

ReferenceAnalyser: analyser_msg
	cd ReferenceAnalyser/ && $(MAKE)

sdk/ctags/Makefile:
	@chmod +x sdk/ctags/configure
	@cd sdk/ctags/ && ./configure
	

LiteEditor: lite_editor_msg $(sample_cpp_objects) 
	$(CMP) -o Runtime/$(EXE_NAME) $(sample_cpp_objects) $(LINK_FLAGS)

$(OBJ_DIR)/%.o: CodeLite/%.cpp CodeLite/%.o.d
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

$(OBJ_DIR)/%.o: LiteEditor/%.cpp LiteEditor/%.o.d
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)
	
CodeLite/%.o.d: 
	@$(CMP) $(CCFLAGS) $(INCLUDES) -MT$(OBJ_DIR)/$(basename $(@F)) -MF$(OBJ_DIR)/$(addsuffix .d, $(basename $(@F))) -MM CodeLite/$(addsuffix .cpp, $(basename $(basename $(@F))))

LiteEditor/%.o.d: 
	@$(CMP) $(CCFLAGS) $(INCLUDES) -MT$(OBJ_DIR)/$(basename $(@F)) -MF$(OBJ_DIR)/$(addsuffix .d, $(basename $(@F))) -MM LiteEditor/$(addsuffix .cpp, $(basename $(basename $(@F))))

$(SCI_OBJ_DIR)/%.o: sdk/wxscintilla/src/%.cpp 
	$(CMP) $(CCFLAGS) $(SCI_DEFS) $(INCLUDES) -c $< -o $(SCI_OBJ_DIR)/$(@F)

$(SCI_OBJ_DIR)/%.o: sdk/wxscintilla/src/scintilla/src/%.cxx 
	$(CMP) $(CCFLAGS) $(SCI_DEFS) $(INCLUDES) -c $< -o $(SCI_OBJ_DIR)/$(@F)

$(SQLITE_OBJ_DIR)/%.o: 	sdk/sqlite3/%.c		
	$(CMP) $(CCFLAGS) -DNO_TCL $(INCLUDES) -c $< -o $(SQLITE_OBJ_DIR)/$(@F)

$(SQLITE_OBJ_DIR)/%.o: 	sdk/wxsqlite3/src/%.cpp		
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(SQLITE_OBJ_DIR)/$(@F)

pre_build:
	test -d  $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d  $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	test -d  $(SCI_OBJ_DIR) || mkdir $(SCI_OBJ_DIR)
	test -d  $(SQLITE_OBJ_DIR) || mkdir $(SQLITE_OBJ_DIR)
	chmod +x ./le_makerevision.sh
	./le_makerevision.sh 

ctags_msg:
	@echo ----------------------------------------------------------
	@echo Building ctags-le 
	@echo ---------------------------------------------------------- 

gizmos_msg:
	@echo ----------------------------------------------------------
	@echo Building Gizmos plugin
	@echo ---------------------------------------------------------- 

debugger_msg:
	@echo ----------------------------------------------------------
	@echo Building Debugger extension
	@echo ---------------------------------------------------------- 

cscope_msg:
	@echo ----------------------------------------------------------
	@echo Building cscope extension
	@echo ---------------------------------------------------------- 

svn_msg:
	@echo ----------------------------------------------------------
	@echo Building Suversion extension
	@echo ---------------------------------------------------------- 

formatter_msg:
	@echo ----------------------------------------------------------
	@echo Building CodeFormatter extension -AStyle
	@echo ---------------------------------------------------------- 

analyser_msg:
	@echo ----------------------------------------------------------
	@echo Building ReferenceAnalyser extension
	@echo ---------------------------------------------------------- 

code_lite_msg:
	@echo ----------------------------------------------------------
	@echo Building CodeLite library
	@echo ---------------------------------------------------------- 

sdk_msg:
	@echo ----------------------------------------------------------
	@echo Building SDK library
	@echo ---------------------------------------------------------- 

sqlite_msg:
	@echo ----------------------------------------------------------
	@echo Building SQLite library
	@echo ---------------------------------------------------------- 

scintilla_msg:
	@echo ----------------------------------------------------------
	@echo Building Scintilla library
	@echo ---------------------------------------------------------- 
	
lite_editor_msg:
	@echo ----------------------------------------------------------
	@echo Building LiteEditor program 
	@echo ---------------------------------------------------------- 
		
wxFlatNotebook_msg:
	@echo ----------------------------------------------------------
	@echo Building wxFlatNotebook library
	@echo ---------------------------------------------------------- 
		
clean:
	$(RM) Debug_gcc/*.o Release_gcc/*.o Release_gcc_unicode/*. Debug_gcc_unicode/*.o lib/*.o lib/*.a
	$(RM) Debug_gcc_sqlite3/*.o Release_gcc_sqlite3/*.o Release_gcc_unicode_sqlite3/*. Debug_gcc_unicode_sqlite3/*.o lib/*.o lib/*.a
	$(RM) Debug_gcc_scintilla/*.o Release_gcc_scintilla/*.o Release_gcc_unicode_scintilla/*. Debug_gcc_unicode_sinctilla/*.o lib/*.o lib/*.a
	$(RM) -R Debug_gcc Release_gcc Release_gcc_unicode Debug_gcc_unicode Debug_gcc_sqlite3 Release_gcc_sqlite3 Release_gcc_unicode_sqlite3 Debug_gcc_unicode_sqlite3 Debug_gcc_scintilla Release_gcc_scintilla Release_gcc_unicode_scintilla Debug_gcc_unicode_scintilla	
	cd sdk/wxflatnotebook/build/wxFlatNotebook/ && $(MAKE) clean
	cd Plugin/ && $(MAKE) clean
	cd Debugger/ && $(MAKE) clean
	cd CodeFormatter/ && $(MAKE) clean
	cd Subversion/ && $(MAKE) clean
	cd ReferenceAnalyser/ && $(MAKE) clean
	cd cscope/ && $(MAKE) clean

clean_deps:
	$(RM) Debug_gcc/*.d Release_gcc/*.d Release_gcc_unicode/*.d Debug_gcc_unicode/*.d
	
install: upgrade
	@echo installing CodeLite
	@\rm -f /usr/local/bin/CodeLite
	@echo copying Runtime/$(EXE_NAME) to /usr/local/bin/
	@mkdir -p /usr/local/bin
	@cp Runtime/$(EXE_NAME) /usr/local/bin/
	@ln -s /usr/local/bin/$(EXE_NAME) /usr/local/bin/CodeLite
	@echo cp Runtime/le_dos2unix.sh /usr/local/bin/
	@cp Runtime/le_dos2unix.sh /usr/local/bin/
	@echo cp Runtime/le_exec.sh /usr/local/bin/
	@cp Runtime/le_exec.sh /usr/local/bin/
	@echo cp Runtime/le_killproc.sh /usr/local/bin/
	@cp Runtime/le_killproc.sh /usr/local/bin/
	@chmod +x /usr/local/bin/le_dos2unix.sh
	@chmod +x /usr/local/bin/le_exec.sh
	@chmod +x /usr/local/bin/le_killproc.sh
	@echo copying ctags-le to /usr/local/bin
	@cp sdk/ctags/ctags-le /usr/local/bin/
	@mkdir -p /usr/local/share/codelite
	@mkdir -p /usr/local/share/codelite/config
	@mkdir -p /usr/local/share/codelite/debuggers
	@mkdir -p /usr/local/share/codelite/templates
	@mkdir -p /usr/local/share/codelite/plugins
	@mkdir -p /usr/local/share/codelite/images
	@mkdir -p /usr/local/share/codelite/rc
	@mkdir -p /usr/local/share/codelite/lexers
	@echo Deleting old settings...
	$(RM) $(HOME)/.liteeditor/debuggers/Debugger.so
	$(RM) $(HOME)/.liteeditor/plugins/CodeFormatter.so
	$(RM) $(HOME)/.liteeditor/plugins/Gizmos.so
	$(RM) $(HOME)/.liteeditor/plugins/ReferenceAnalyser.so
	$(RM) $(HOME)/.liteeditor/plugins/Subversion.so
	$(RM) $(HOME)/.liteeditor/plugins/cscope.so
	$(RM) $(HOME)/.liteeditor/config/build_settings.xml
	$(RM) $(HOME)/.liteeditor/config/debuggers.xml
	$(RM) $(HOME)/.liteeditor/config/liteeditor.xml
	$(RM) $(HOME)/.liteeditor/rc/menu.xrc
	$(RM) $(HOME)/.liteeditor/astyle.sample
	@echo Copying new settings...
	@\cp -pr Runtime/config/build_settings.xml /usr/local/share/codelite/config/build_settings.xml
	@\cp -pr Runtime/config/debuggers.xml /usr/local/share/codelite/config/debuggers.xml
	@\cp -pr Runtime/config/liteeditor.xml.sample /usr/local/share/codelite/config/liteeditor.xml
	@\cp -pr Runtime/rc/menu.xrc /usr/local/share/codelite/rc/
	@\cp -pr Runtime/templates/*.project /usr/local/share/codelite/templates/
	@\cp -pr Runtime/templates/*.wizard /usr/local/share/codelite/templates/
	@\cp -pr Runtime/lexers/*.xml /usr/local/share/codelite/lexers/
	@\cp -pr lib/Debugger.so /usr/local/share/codelite/debuggers/
	@\cp -pr lib/CodeFormatter.so /usr/local/share/codelite/plugins/
	@\cp -pr lib/Subversion.so /usr/local/share/codelite/plugins/
	@\cp -pr lib/ReferenceAnalyser.so /usr/local/share/codelite/plugins/
	@\cp -pr lib/ReferenceAnalyser.so /usr/local/share/codelite/plugins/
	@\cp -pr lib/cscope.so /usr/local/share/codelite/plugins/
	@\cp -pr lib/Gizmos.so /usr/local/share/codelite/plugins/
	@\cp -pr Runtime/index.html /usr/local/share/codelite/
	@\cp -pr Runtime/astyle.sample /usr/local/share/codelite/
	@\cp -pr Runtime/images/*.png /usr/local/share/codelite/images/
	@chmod -R 777 /usr/local/share/codelite
	@chmod +x /usr/local/bin/CodeLite
	@echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	@echo  Notice:									
	@echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	@echo Project name is now CodeLite instead of LiteEditor!
	@echo To start the application, type: CodeLite &
	@echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	
upgrade:
	@echo removing old binaries...
	@\rm -fr /usr/local/bin/LiteEditor
	@\rm -fr /usr/local/bin/LiteEditorUD
	@\rm -fr /usr/local/bin/LiteEditorUR
	@\rm -fr /usr/local/bin/CodeLite
	@\rm -fr /usr/local/bin/CodeLiteUR
	@\rm -fr /usr/local/bin/CodeLiteUD
	@\rm -fr /usr/local/bin/le_dos2unix.sh	
	@\rm -fr /usr/local/bin/le_exec.sh	
	@\rm -fr /usr/local/bin/le_killproc.sh
	@\rm -fr /usr/local/bin/le_intgdb.sh
	@\rm -fr /usr/local/bin/$(EXE_NAME) 
	@\rm -fr $(HOME)/.codelite/config/liteeditor.xml
	@\rm -fr $(HOME)/.liteeditor/config/liteeditor.xml

uninstall:
	@echo Uninstalling CodeLite
	@\rm -fr /usr/local/bin/LiteEditor
	@\rm -fr /usr/local/bin/LiteEditorUD
	@\rm -fr /usr/local/bin/LiteEditorUR
	@\rm -fr /usr/local/bin/CodeLite
	@\rm -fr /usr/local/bin/CodeLiteUR
	@\rm -fr /usr/local/bin/CodeLiteUD
	@\rm -fr /usr/local/bin/le_dos2unix.sh	
	@\rm -fr /usr/local/bin/le_exec.sh	
	@\rm -fr /usr/local/bin/le_killproc.sh
	@\rm -fr /usr/local/bin/le_intgdb.sh
	@\rm -fr /usr/local/bin/$(EXE_NAME) 
	@\rm -fr $(HOME)/.liteeditor
	@\rm -fr $(HOME)/.codelite

-include $(OBJ_DIR)/*.d
