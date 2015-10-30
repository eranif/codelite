ifeq ($(type),release)
  WX_COMMAND=`wx-config --cflags --debug=no`
  FLAGS=-O2 -DNDEBUG -fPIC -std=c++11
  TARGET=precompiled_header_release.h.gch
else
  WX_COMMAND=`wx-config --cflags --debug=no`
  FLAGS=-g -O0 -fPIC -std=c++11
  TARGET=precompiled_header_dbg.h.gch
endif

ifeq ($(os),osx)
  OSX_FLAGS=-mmacosx-version-min=10.8 -stdlib=libc++
else
  OSX_FLAGS=
endif

all: $(TARGET)

precompiled_header_dbg.h.gch: precompiled_header_dbg.h
	@echo -- Building PCH file precompiled_header_dbg.h.gch
	$(CXX) $(FLAGS) $(CFLAGS) $(WX_COMMAND) $(OSX_FLAGS) -c precompiled_header_dbg.h -o precompiled_header_dbg.h.gch

precompiled_header_release.h.gch: precompiled_header_release.h
	@echo -- Building PCH file precompiled_header_release.h.gch
	$(CXX) $(FLAGS) $(CFLAGS) $(WX_COMMAND) $(OSX_FLAGS) -c precompiled_header_release.h -o precompiled_header_release.h.gch

clean:
	$(RM) -f $(TARGET)
