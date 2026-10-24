cmake_minimum_required(VERSION 3.18)

# ######################################################################################################################
# cc-wrapper
# ######################################################################################################################
set(WITHOUT_INSTALL ON)
add_subdirectory(cc-wrapper)

install(TARGETS cc-wrapper DESTINATION ${CL_INSTALL_BIN})

# ######################################################################################################################
# doctest
# a library header for test framework
# ######################################################################################################################
include(CTest)
if(BUILD_TESTING)
  add_library(doctest INTERFACE)
  target_include_directories(
    doctest SYSTEM
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/doctest/doctest")
  set(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH}
      "${CMAKE_CURRENT_SOURCE_DIR}/doctest/scripts/cmake")
  include(doctest)
endif()

# ######################################################################################################################
# dtl
# ######################################################################################################################
add_library(dtl INTERFACE)
target_include_directories(dtl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/dtl)

# ######################################################################################################################
# For wxWidgets dependencies
# ######################################################################################################################
include("${wxWidgets_USE_FILE}")

add_subdirectory(lua)
add_subdirectory(LuaBridge)

# ######################################################################################################################
# wxTerminalEmulator
# ######################################################################################################################
# On Linux we do not ship the GlyphT application (we only need the terminal library). Installing it conflicts with the
# standalone "glypht" package
if(UNIX AND NOT APPLE)
  set(BUILD_GLYPHT OFF CACHE BOOL "Build the GlyphT Terminal Application" FORCE)
endif()
add_subdirectory(wxTerminalEmulator)

# ######################################################################################################################
# md4c: a Markdown parser (C, MIT license).
# We do not add the md4c CMake project: it also builds md4c-html and the md2html tool, and it installs files. We build
# only the parser (md4c.c) and the table of HTML entities (entity.c, to decode the entities that md4c reports) as a
# static library.
# ######################################################################################################################
add_library(md4c STATIC "md4c/src/md4c.c" "md4c/src/entity.c")
target_include_directories(md4c SYSTEM PUBLIC "md4c/src")
# It is linked into the `libcodelite` shared library (see clMarkdownParser)
set_target_properties(md4c PROPERTIES POSITION_INDEPENDENT_CODE ON C_STANDARD 99)
if(NOT MSVC)
  # Third party code: no warnings
  target_compile_options(md4c PRIVATE -w)
endif()
