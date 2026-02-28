cmake_minimum_required(VERSION 3.18)

# ######################################################################################################################
# Check submodules are here
# ######################################################################################################################
find_path(CTAGS_SUBMODULE CMakeLists.txt PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ctags/)
if(NOT CTAGS_SUBMODULE)
  message(STATUS "Could not locate `${CMAKE_CURRENT_SOURCE_DIR}/ctags/CMakeLists.txt` file")
  message(FATAL_ERROR "Please make sure you have run `git submodule update --init`")
endif()

# ######################################################################################################################
# cc-wrapper
# ######################################################################################################################
set(WITHOUT_INSTALL ON)
add_subdirectory(cc-wrapper)

install(TARGETS cc-wrapper DESTINATION ${CL_INSTALL_BIN})

# ######################################################################################################################
# cJSON
# ######################################################################################################################
add_library(cJSON STATIC ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.c ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.h)
target_include_directories(cJSON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cJSON)

# ######################################################################################################################
# ctags
# ######################################################################################################################
if(NOT WXC_APP)
  add_subdirectory(ctags)
  install(TARGETS ctags DESTINATION ${CL_INSTALL_BIN})
  set_target_properties(ctags PROPERTIES OUTPUT_NAME "codelite-ctags")
  if(APPLE)
    install(
      TARGETS ctags
      DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS
      PERMISSIONS ${EXE_PERM})
  else()
    install(
      TARGETS ctags
      DESTINATION ${CL_PREFIX}/bin
      PERMISSIONS ${EXE_PERM})
  endif()
endif()

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
# wx-config-msys2
# ######################################################################################################################

if(NOT WXC_APP AND MINGW)
  # build wx-config for Windows
  add_subdirectory(wx-config-msys2)
  install(TARGETS wx-config DESTINATION ${CL_INSTALL_BIN})
endif()

# ######################################################################################################################
# For wxWidgets dependencies
# ######################################################################################################################
include("${wxWidgets_USE_FILE}")

add_subdirectory(lua)
add_subdirectory(LuaBridge)
