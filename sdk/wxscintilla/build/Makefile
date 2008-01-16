##############################################################################
## File:        Makefile
## Purpose:     wxScintilla
## Maintainer:  Otto Wyss
## Created:     2004-09-19
## RCS-ID:      $Id: Makefile,v 1.13 2006/03/14 19:10:49 wyo Exp $
## Copyright:   (c) 2004 wxCode
## Licence:     wxWindows
##############################################################################

# list of source files
SRC = PlatWX.cpp \
      ScintillaWX.cpp \
      wxscintilla.cpp \
      AutoComplete.cxx \
      CallTip.cxx \
      CellBuffer.cxx \
      CharClassify.cxx \
      ContractionState.cxx \
      Document.cxx \
      DocumentAccessor.cxx \
      Editor.cxx \
      Indicator.cxx \
      KeyMap.cxx \
      KeyWords.cxx \
      LexAda.cxx \
      LexAPDL.cxx \
      LexAsm.cxx \
      LexAsn1.cxx \
      LexAU3.cxx \
      LexAVE.cxx \
      LexBaan.cxx \
      LexBash.cxx \
      LexBasic.cxx \
      LexBullant.cxx \
      LexCaml.cxx \
      LexCLW.cxx \
      LexConf.cxx \
      LexCPP.cxx \
      LexCrontab.cxx \
      LexCsound.cxx \
      LexCSS.cxx \
      LexEiffel.cxx \
      LexErlang.cxx \
      LexEScript.cxx \
      LexFlagship.cxx \
      LexForth.cxx \
      LexFortran.cxx \
      LexGui4Cli.cxx \
      LexHaskell.cxx \
      LexHTML.cxx \
      LexInno.cxx \
      LexKix.cxx \
      LexLisp.cxx \
      LexLout.cxx \
      LexLua.cxx \
      LexMatlab.cxx \
      LexMetapost.cxx \
      LexMMIXAL.cxx \
      LexMPT.cxx \
      LexMSSQL.cxx \
      LexOpal.cxx \
      LexNsis.cxx \
      LexOthers.cxx \
      LexPascal.cxx \
      LexPB.cxx \
      LexPerl.cxx \
      LexPOV.cxx \
      LexPS.cxx \
      LexPython.cxx \
      LexRebol.cxx \
      LexRuby.cxx \
      LexScriptol.cxx \
      LexSmalltalk.cxx \
      LexSpecman.cxx \
      LexSQL.cxx \
      LexTADS3.cxx \
      LexTCL.cxx \
      LexTeX.cxx \
      LexVB.cxx \
      LexVerilog.cxx \
      LexVHDL.cxx \
      LexYAML.cxx \
      LineMarker.cxx \
      PropSet.cxx \
      RESearch.cxx \
      ScintillaBase.cxx \
      Style.cxx \
      StyleContext.cxx \
      UniConversion.cxx \
      ViewStyle.cxx \
      WindowAccessor.cxx \
      XPM.cxx

# source directories
vpath %.cpp ../src
vpath %.cxx ../src/scintilla/src

# additional header directories
INC = -I../include \
      -I../src/scintilla/include \
      -I../src/scintilla/src

DEF = -D__WX__=1 -DSCI_LEXER -DLINK_LEXERS

# objects
OBJ = $(patsubst %.cxx,%.o,$(SRC:.cpp=.o))

# library
LIB = libwxscintilla


##############################################################################
# implementation, change only if other settings or other targets are needed

ifneq ($(WXDEVEL),)
  WXDEVEL_Release = $(WXDEVEL)/Release/
  WXDEVEL_Debug = $(WXDEVEL)/Debug/
  WXDEVEL_Univ = $(WXDEVEL)/Univ/
endif

ifeq ($(MAKECMDGOALS),)
  WXCONFIG=wx-config
  GTKCONFIG=gtk-config
endif
ifeq ($(MAKECMDGOALS),release)
  WXCONFIG=wx-config
  GTKCONFIG=gtk-config
endif
ifeq ($(MAKECMDGOALS),debug)
  WXCONFIG=wxgtkd-2.4-config
  GTKCONFIG=gtk-config
endif
ifeq ($(MAKECMDGOALS),develdebug)
  WXCONFIG=$(WXDEVEL_Debug)wx-config --inplace
  GTKCONFIG=pkg-config gtk+-2.0
endif
ifeq ($(MAKECMDGOALS),univdebug)
  WXCONFIG=$(WXDEVEL_Univ)wx-config --inplace
  GTKCONFIG=pkg-config gtk+-2.0
endif

%.o:	%.cpp
	g++ -fpic -c $(DEF) $(INC) `$(WXCONFIG) --cxxflags` -g -o $@ $<

%.o:	%.cxx
	g++ -fpic -c $(DEF) $(INC) `$(WXCONFIG) --cxxflags` -g -o $@ $<

all:	$(LIB).so
$(LIB).so:	$(OBJ)
	g++ -fpic --share -o ../lib/$@ $^ `$(WXCONFIG) --libs`

release:	$(LIB).a
$(LIB).a:	$(OBJ)
	ar crs ../lib/$@ $^

debug:	$(LIB).so.d
$(LIB).so.d:	$(OBJ)
	g++ -fpic --share -o ../lib/$@ $^ `$(WXCONFIG) --libs`

develdebug:	$(LIB).so.dd
$(LIB).so.dd:	$(OBJ)
	g++ -fpic --share -o ../lib/$@ $^ `$(WXCONFIG) --libs`

univdebug:	$(LIB).so.ud
$(LIB).so.ud:	$(OBJ)
	g++ -fpic --share -o ../lib/$@ $^ `$(WXCONFIG) --libs`

clean:
	rm -f *.o
