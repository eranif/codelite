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

# ######################################################################################################################
# wxTerminalEmulator
# ######################################################################################################################
set(BUILD_WXTERMINAL_DEMO OFF)
add_subdirectory(wxTerminalEmulator)
