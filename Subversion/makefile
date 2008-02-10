## Author: Eran Ifrah

 
## 
## Set default build set
## as ANSI-deubg
##
WXCFG=--unicode=no --debug=yes 
EXT=d
OBJ_DIR=Debug_gcc
DEBUG= -g 

SOFLAG=-shared 
OSNAME=$(shell uname -s)
ifeq ($(OSNAME), Darwin)
SOFLAG=-dynamiclib
endif

##
## Override default settings by typing: make type=[release | release_unicode | debug_unicode]
##
ifeq ($(type), release)
WXCFG=--unicode=no  --debug=no
EXT=
OBJ_DIR=Release_gcc
OPT=-O3
DEBUG=
endif

ifeq ($(type), release_unicode)
WXCFG=--unicode=yes --debug=no
EXT=u
OBJ_DIR=Release_gcc_unicode
OPT=-O3 -DREGEXP_UNICODE
DEBUG=
endif

ifeq ($(type), debug_unicode)
WXCFG=--unicode=yes --debug=yes
EXT=ud
OBJ_DIR=Debug_gcc_unicode
DEBUG= -g 
OPT=-DREGEXP_UNICODE
endif

WXVER=26
OUTPUT_DIR=../lib

#PROFILER= -pg 
##
## Define variables, using wx-config tool
##
CMP=g++ $(DEBUG) $(OPT) 

CCFLAGS= -DASTYLE_LIB -D__WX__ -Wall $(TRACE_FLAG) -I. -DWXUSINGDLL -DWX_PRECOMP -DNO_GCC_PRAGMA -DXTHREADS -D_REENTRANT -DXUSE_MTSAFE_API $(shell wx-config --cxxflags $(WXCFG)) $(PROFILER) -fno-strict-aliasing -DYY_NEVER_INTERACTIVE=1

SQLITE_INCLUDE= -I../sdk/wxsqlite3/include -I../sdk/wxsqlite3/sqlite3/include
INCLUDES = -I. -I../Interfaces -I../CodeLite  -I../Plugin -I../sdk/wxflatnotebook/include
LINK_FLAGS=$(shell wx-config --libs $(WXCFG)) -L../lib -lplugin$(EXT) -lcodelite$(EXT) -L../sdk/wxflatnotebook/lib -lwxflatnotebook$(EXT)

##
## Define the object files
##
lib_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard *.cpp)))))

## our main build target
build : pre_build Subversion

Subversion: $(lib_cpp_objects)
	$(CMP) $(SOFLAG) -o $(OUTPUT_DIR)/Subversion.so $(lib_cpp_objects) $(LINK_FLAGS)

$(OBJ_DIR)/%.o: %.cpp %.o.d
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

%.o.d: 
	$(CMP) $(CCFLAGS) $(INCLUDES) -MT$(OBJ_DIR)/$(basename $(@F)) -MF$(OBJ_DIR)/$(addsuffix .d, $(basename $(@F))) -MM $(addsuffix .cpp, $(basename $(basename $(@F))))

pre_build:
	test -d  $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d  $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)

clean:
	$(RM) -fR Release_gcc_unicode/ $(OUTPUT_DIR)/Subversion.so
	$(RM) -fR Debug_gcc_unicode/
	
-include $(OBJ_DIR)/*.d
