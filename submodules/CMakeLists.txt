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
# For wxWidgets dependencies
# ######################################################################################################################
include("${wxWidgets_USE_FILE}")

add_subdirectory(lua)
add_subdirectory(LuaBridge)
