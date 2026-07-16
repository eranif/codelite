# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

# configure version file, this needs to be done before we set the SRCS variable
configure_file(wxcversion.cpp.in
               ${CMAKE_BINARY_DIR}/generated/wxcversion.cpp)

file(GLOB_RECURSE SRCS "*.cpp")
list(APPEND SRCS "${CMAKE_BINARY_DIR}/generated/wxcversion.cpp")

# wxCrafterCommon: linked into all three targets (wxcrafter, wxCrafterPlugin, wxcgen).
add_library(wxCrafterCommon STATIC ${SRCS})
set_target_properties(wxCrafterCommon PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(wxCrafterCommon PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}"
  "${CMAKE_CURRENT_LIST_DIR}/Importer"
  "${CMAKE_CURRENT_LIST_DIR}/aui"
  "${CMAKE_CURRENT_LIST_DIR}/controls"
  "${CMAKE_CURRENT_LIST_DIR}/sizers"
  "${CMAKE_CURRENT_LIST_DIR}/src"
  "${CMAKE_CURRENT_LIST_DIR}/top_level_windows")

# Link the CodeLite internal libs so the static lib picks up their INTERFACE
# include directories (JSON.h, etc.). Static libs don't actually link against
# other libs at archive time, but CMake will propagate include dirs and compile
# features.
target_link_libraries(wxCrafterCommon PUBLIC libcodelite)
if(USE_PCH)
  if(NOT MINGW)
    target_compile_options(wxCrafterCommon PRIVATE -include "${CL_PCH_FILE}")
    target_compile_options(wxCrafterCommon PRIVATE -Winvalid-pch)
  endif()
  target_precompile_headers(wxCrafterCommon REUSE_FROM PCH)
endif()
