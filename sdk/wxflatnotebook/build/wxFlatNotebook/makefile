###################################################################################################################
#
# Project: wxFlatNotebook
# Date: 1/23/2006 12:06AM
# Author: Eran Ifrah 
#
# gcc makefile for building wxFlatNotebook on Linux platform
# Usage:
# to run it in debug mode run
# make clean
# make type=debug
#
# for release build, simply run:
# make clean
# make 
###################################################################################################################


ifeq ($(type),debug)
   WXCFG=--debug=yes --unicode=no
   DEBUG_FLAG=d
   OBJ_DIR = ../../gcc_debug
   UNICODE=
   OPT=-O0
endif 

ifeq ($(type),release)
   WXCFG=--debug=no --unicode=no
   DEBUG_FLAG=
   OBJ_DIR = ../../gcc_release
   UNICODE=
   OPT=-O3
endif

ifeq ($(type),release_unicode)
   WXCFG=--debug=no --unicode=yes
   DEBUG_FLAG=
   OBJ_DIR = ../../gcc_release_unicode
   UNICODE=u
   OPT=-O3
endif

ifeq ($(type),debug_unicode)
   WXCFG=--debug=yes --unicode=yes
   DEBUG_FLAG=d
   OBJ_DIR = ../../gcc_debug_unicode
   UNICODE=u
   OPT=-O0
endif

CMP = g++ $(OPT) 
CXXFLAGS =  -I../../include $(shell wx-config --cxxflags $(WXCFG))

## Libraries required to link the sample program and their path
LIBS =  -L../../lib -lwxflatnotebook$(UNICODE)$(DEBUG_FLAG)  $(shell wx-config --libs $(WXCFG))

## sample program name
OUTPUT  = wxFlatNotebookTest

## library name
LIB_OUTPUT  = ../../lib/libwxflatnotebook$(UNICODE)$(DEBUG_FLAG).a

##
## wxflatnotebook lib and sample objects
##
lib_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard ../../src/wxFlatNotebook/*.cpp)))))
sample_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard ../../samples/wxFlatNotebook/*.cpp)))))

## Rules
wxFlatNotebookTest: pre_build wxFlatNotebook $(sample_objects)
	$(CMP) -o $(OUTPUT) $(sample_objects) $(LIBS)

wxFlatNotebook: $(lib_objects)
	ar rcu $(LIB_OUTPUT) $(lib_objects)
	ranlib $(LIB_OUTPUT)

$(OBJ_DIR)/%.o: ../../src/wxFlatNotebook/%.cpp
	$(CMP) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/$(@F)

$(OBJ_DIR)/%.o: ../../samples/wxFlatNotebook/%.cpp
	$(CMP) $(CXXFLAGS) -c $< -o $(OBJ_DIR)/$(@F)

pre_build:
	test -d ../../gcc_debug || mkdir ../../gcc_debug
	test -d ../../gcc_debug_unicode || mkdir ../../gcc_debug_unicode
	test -d ../../gcc_release || mkdir ../../gcc_release
	test -d ../../gcc_release_unicode || mkdir ../../gcc_release_unicode
	test -d ../../lib || mkdir ../../lib
clean: 
	${RM} ../../gcc_debug/*.o
	${RM} ../../gcc_release/*.o
	${RM} ../../gcc_release_unicode/*.o
	${RM} ../../gcc_debug_unicode/*.o
	${RM} ../../lib/libwxflatnotebookd.a ../../lib/libwxflatnotebook.a ../../lib/libwxflatnotebooku.a ../../lib/libwxflatnotebookud.a
