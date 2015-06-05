# Find Clang
#
# It defines the following variables
# CLANG_FOUND        - True if Clang found.
# CLANG_INCLUDE_DIRS - where to find Clang include files
# CLANG_LIBS         - list of clang libs
# CLANG_LDFLAGS      - flags passing for ld

if (NOT LLVM_INCLUDE_DIRS OR NOT LLVM_LIBRARY_DIRS)
   message(FATAL_ERROR "No LLVM and Clang support requires LLVM")
else (NOT LLVM_INCLUDE_DIRS OR NOT LLVM_LIBRARY_DIRS)

MACRO(FIND_AND_ADD_CLANG_LIB _libname_)
find_library(CLANG_${_libname_}_LIB ${_libname_} ${LLVM_LIBRARY_DIRS} ${CLANG_LIBRARY_DIRS})
if (CLANG_${_libname_}_LIB)
   get_filename_component(ADD_LIB ${CLANG_${_libname_}_LIB} NAME_WE)
   set(CLANG_LIBS "${CLANG_LIBS} -l${_libname_}")
endif (CLANG_${_libname_}_LIB)
ENDMACRO(FIND_AND_ADD_CLANG_LIB)

# FIND_AND_ADD_CLANG_LIB(clang)
# Clang shared library provides just the limited C interface, so it
# can not be used.  We look for the static libraries.
FIND_AND_ADD_CLANG_LIB(clangAnalysis)
FIND_AND_ADD_CLANG_LIB(clangARCMigrate)
FIND_AND_ADD_CLANG_LIB(clangAST)
FIND_AND_ADD_CLANG_LIB(clangASTMatchers)
FIND_AND_ADD_CLANG_LIB(clangBasic)
FIND_AND_ADD_CLANG_LIB(clangCodegen)
FIND_AND_ADD_CLANG_LIB(clangDriver)
FIND_AND_ADD_CLANG_LIB(clangDynamicASTMatchers)
FIND_AND_ADD_CLANG_LIB(clangEdit)
FIND_AND_ADD_CLANG_LIB(clangFormat)
FIND_AND_ADD_CLANG_LIB(clangFrontend)
FIND_AND_ADD_CLANG_LIB(clangFrontendTools)
FIND_AND_ADD_CLANG_LIB(clangIndex)
FIND_AND_ADD_CLANG_LIB(clangLex)
FIND_AND_ADD_CLANG_LIB(clangParse)
FIND_AND_ADD_CLANG_LIB(clangRewrite)
FIND_AND_ADD_CLANG_LIB(clangRewriteCore)
FIND_AND_ADD_CLANG_LIB(clangRewriteFrontend)
FIND_AND_ADD_CLANG_LIB(clangSema)
FIND_AND_ADD_CLANG_LIB(clangSerialization)
FIND_AND_ADD_CLANG_LIB(clangStaticAnalyzerCheckers)
FIND_AND_ADD_CLANG_LIB(clangStaticAnalyzerCore)
FIND_AND_ADD_CLANG_LIB(clangStaticAnalyzerFrontend)
FIND_AND_ADD_CLANG_LIB(clangTooling)
FIND_AND_ADD_CLANG_LIB(clangToolingCore)

find_path(CLANG_INCLUDE_DIRS clang/Basic/Version.h HINTS ${LLVM_INCLUDE_DIRS})

if (CLANG_LIBS AND CLANG_INCLUDE_DIRS)
  set(CLANG_LIBS "-lclang ${CLANG_LIBS}")
  MESSAGE(STATUS "Clang libs: " ${CLANG_LIBS})
  set(CLANG_FOUND TRUE)
endif (CLANG_LIBS AND CLANG_INCLUDE_DIRS)

if (CLANG_FOUND)
  message(STATUS "Found Clang: ${CLANG_INCLUDE_DIRS}")
  set(CLANG_LDFLAGS -L${LLVM_LIBRARY_DIRS} ${CLANG_LIBS})
  set(CLANG_LDFLAGS "-Wl,--start-group ${CLANG_LIBS} ${LLVM_MODULE_LIBS} -Wl,--end-group ${LLVM_LDFLAGS}")
  if (MINGW)
    set(CLANG_LDFLAGS "${CLANG_LDFLAGS} ${LLVM_SYSTEM_LIBS}")
  endif()
  
else (CLANG_FOUND)
  if (CLANG_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Clang")
  endif (CLANG_FIND_REQUIRED)
endif (CLANG_FOUND)

endif (NOT LLVM_INCLUDE_DIRS OR NOT LLVM_LIBRARY_DIRS)
