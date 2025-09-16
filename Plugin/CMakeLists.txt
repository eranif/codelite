# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(plugin)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

file(GLOB_RECURSE SRCS "*.cpp" "*.h" "*.hpp")

add_library(plugin SHARED ${SRCS})

# Include paths
target_include_directories(
  plugin
  PUBLIC "${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/Interfaces" "${CL_SRC_ROOT}/Plugin/aui"
  PRIVATE "${CL_SRC_ROOT}/submodules/yaml-cpp/include" "${CL_SRC_ROOT}/ThemeImporters" "${CL_SRC_ROOT}/wxTerminalCtrl")

# Macros
if(WIN32)
  target_compile_definitions(
    plugin
    PRIVATE WXMAKINGDLL_SDK
    INTERFACE WXUSINGDLL_SDK)
endif(WIN32)

if(USE_PCH AND NOT MINGW)
  add_definitions(-include "${CL_PCH_FILE}")
  add_definitions(-Winvalid-pch)
endif()

# On UNIX we require GTK
if(UNIX AND NOT APPLE)
  if(GTK_VERSION EQUAL 3)
    set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}) # Cache the current value
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    find_package(GTK3)
    set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH}) # Reset, else the official path isn't used again :/
    if(GTK3_FOUND)
      include_directories(${GTK3_INCLUDE_DIRS})
    else(GTK3_FOUND)
      message(FATAL_ERROR "Could not locate GTK.")
    endif(GTK3_FOUND)
  else()
    find_package(GTK2)
    if(GTK2_FOUND)
      include_directories(${GTK2_INCLUDE_DIRS})
    else(GTK2_FOUND)
      message(FATAL_ERROR "Could not locate GTK.")
    endif(GTK2_FOUND)
  endif()

endif(UNIX AND NOT APPLE)

# Additional libraries for UNIX-like systems
set(ADDITIONAL_LIBRARIES "")

if(UNIX)
  if(IS_FREEBSD)
    set(ADDITIONAL_LIBRARIES "-lkvm -lutil")
  elseif(IS_NETBSD)
    set(ADDITIONAL_LIBRARIES "-lutil")
  elseif(UNIX AND NOT APPLE)
    set(ADDITIONAL_LIBRARIES "-ldl -lutil -lgobject-2.0")
  else()
    set(ADDITIONAL_LIBRARIES "-ldl")
  endif()
elseif(WIN32)
  set(ADDITIONAL_LIBRARIES "-luxtheme")
endif()

# Add RPATH
if(NOT MINGW AND WXC_APP)
  string(REPLACE "codelite" "wxcrafter" WXC_LIBS_DIR ${PLUGINS_DIR})
  set(LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}")
  message("-- libplugin.so is using RPATH set to ${WXC_LIBS_DIR}")
endif()

if(UNIX OR APPLE)
  set_target_properties(plugin PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(
  plugin
  ${LINKER_OPTIONS}
  ${ADDITIONAL_LIBRARIES}
  ${GTK3_LIBRARIES}
  ${LIBYAML_CPP}
  -L"${CL_LIBPATH}"
  lexers_extra
  libcodelite)

if(USE_PCH)
  codelite_add_pch(plugin)
endif()

if(NOT MINGW)
  if(APPLE)
    install(TARGETS plugin DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
  else()
    install(TARGETS plugin DESTINATION ${PLUGINS_DIR})
  endif()
else()
  install(
    TARGETS plugin
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()
