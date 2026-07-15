if(NOT CL_SRC_ROOT)
  message(FATAL_ERROR "Missing CL_SRC_ROOT")
endif()

if(WXC_APP)
  message("-- Building wxCrafter as a standalone executable")
else()
  message("-- Building wxCrafter as codelite plugin")
endif(WXC_APP)

set(APP_NAME "wxcrafter")
set(PLUGIN_NAME "wxCrafter")

set(WXC_DIR "${CL_SRC_ROOT}/wxcrafter")

message("-- Generating makefile for wxCrafter")

# Our project is called 'plugin' this is how it will be called in visual studio,
# and in our makefiles.
project(wxCrafter)

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
  add_definitions(-include "${CL_PCH_FILE}")
  add_definitions(-Winvalid-pch)
endif()

# Add RPATH When building an application, set the install library to the proper
# location which is now under /usr/lib/x86_64-linux-gnu/wxcrafter
if(NOT MINGW)
  if(WXC_APP)
    set(WXC_INSTALL_LIBDIR "lib")
    if(NOT PREFIX)
      # If the caller hasn't set his own destination, install to a multi-arch
      # lib dir if applicable
      if(CMAKE_VERSION VERSION_GREATER 2.8.7 AND (UNIX AND NOT APPLE))
        include(GNUInstallDirs)
        # defines CMAKE_INSTALL_LIBDIR to lib or lib64 or whatever. Available
        # since cmake 2.8.8
        set(WXC_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
      endif()
    endif(NOT PREFIX)
    set(WXC_LIBS_DIR
        "${CMAKE_INSTALL_PREFIX}/${WXC_INSTALL_LIBDIR}/${APP_NAME}")
    set(LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}")
  else(WXC_APP)
    if(NOT OFFICIAL_CL_BUILD)
      set(LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
    endif(NOT OFFICIAL_CL_BUILD)
  endif(WXC_APP)
endif()

# configure version file, this needs to be done before we set the SRCS variable
configure_file(${CL_SRC_ROOT}/wxcrafter/wxcversion.cpp.in
               ${CMAKE_BINARY_DIR}/generated/wxcversion.cpp)

file(
  GLOB
  ALL_SRCS
  "Importer/*.cpp"
  "Preview/*.cpp"
  "Properties/*.cpp"
  "UI/PropertiesView/*.cpp"
  "UI/*.cpp"
  "aui/*.cpp"
  "controls/*.cpp"
  "controls/Containers/*.cpp"
  "controls/Grid/*.cpp"
  "controls/PropertyGrid/*.cpp"
  "controls/RibbonBar/*.cpp"
  "controls/menu_toolbar/*.cpp"
  "sizers/*.cpp"
  "src/*.cpp"
  "top_level_windows/*.cpp"
  "xrc_handlers/*.cpp"
  "${CMAKE_BINARY_DIR}/generated/wxcversion.cpp")

# ----------------------------------------------------------------------------
# Source bucketing
# ----------------------------------------------------------------------------
# We have three target flavors that differ in one preprocessor symbol:
#
# target            STANDALONE_BUILD
# ----------------- ----------------
# wxCrafterPlugin   undefined wxcrafter (exe)   1 wxcgen (exe)      unused
#
# Most TUs don't reference the macro; their object files are bit-identical
# across all flavors and can be compiled once, into a shared static library.
# Only a handful of TUs actually #if on the macro and must be compiled per
# flavor.
#
# Bucket the files into: MACRO_SENSITIVE_GUI_ONLY_SRCS — TUs whose object code
# depends on the define above. Compiled per target with the right define.
# GUI_ONLY_SRCS        — TUs that pull GUI-only symbols (MainFrame, etc.). NOT
# linked into wxcgen. COMMON_GUI_SRCS      — define-agnostic but GUI-coupled.
# Compiled into wxCrafterCommonGui (linked by GUI targets only). COMMON_SRCS —
# define-agnostic AND headless-safe. Compiled into wxCrafterCommon (linked by
# all three targets).

# The 7 .cpp files that #if on STANDALONE_BUILD (verified by `grep -l
# "STANDALONE_BUILD" **/*.cpp`). these are GUI-only (define MainFrame,
# GUICraftMainPanel, plugin glue, GUI dialogs) and must NOT be linked into the
# headless wxcgen executable.
set(MACRO_SENSITIVE_GUI_ONLY_FILES
    "UI/main.cpp" "src/allocator_mgr.cpp" "src/wxcrafter_plugin.cpp"
    "UI/wxguicraft_main_view.cpp" "UI/NewFormWizard.cpp"
    "UI/wxcSettingsDlg.cpp")

# Build absolute-path source lists.
set(MACRO_SENSITIVE_GUI_ONLY_SRCS)
foreach(f ${MACRO_SENSITIVE_GUI_ONLY_FILES})
  list(APPEND MACRO_SENSITIVE_GUI_ONLY_SRCS "${CMAKE_CURRENT_LIST_DIR}/${f}")
endforeach()

# Define-agnostic files split into headless-safe (COMMON_SRCS) and GUI-only
# (COMMON_GUI_SRCS). The split is based on whether the TU pulls GUI-only symbols
# (MainFrame, GUICraftMainPanel, wxDialog subclasses, etc.).
set(COMMON_GUI_ONLY_FILES
    "UI/AboutDlg.cpp"
    "UI/BitmapSelectorDlg.cpp"
    "UI/duplicateTLWdlg.cpp"
    "UI/EventsEditorDlg.cpp"
    "UI/import_dlg.cpp"
    "UI/MediaCtrl.cpp"
    "UI/OpenGLCanvas.cpp"
    "src/designer_panel.cpp"
    "src/enter_strings_dlg.cpp"
    "src/file_picker_ctrl.cpp"
    "src/my_dialog.cpp"
    "src/my_frame.cpp"
    "src/my_panel.cpp"
    "src/output_nbook.cpp"
    "src/wxc_aui_manager.cpp"
    "src/wxc_aui_tool_stickiness.cpp"
    "src/wxc_edit_manager.cpp"
    "src/wxcLicenseGPL2.cpp"
    "src/wxgui_bitmaploader.cpp"
    "Importer/importer_from_wxFB.cpp"
    "Importer/importer_from_wxSmith.cpp"
    "Importer/importer_from_xrc.cpp")

# Start with everything except the macro-sensitive files (they're handled
# separately per target).
set(COMMON_SRCS ${ALL_SRCS})
list(REMOVE_ITEM COMMON_SRCS ${MACRO_SENSITIVE_GUI_ONLY_SRCS})

# COMMON_GUI_SRCS: take from COMMON_SRCS the explicitly-listed src/ GUI files
# plus the wholesale GUI-only directories (UI/, UI/PropertiesView/, Preview/,
# xrc_handlers/).
set(COMMON_GUI_SRCS)
foreach(f ${COMMON_GUI_ONLY_FILES})
  set(absf "${CMAKE_CURRENT_LIST_DIR}/${f}")
  list(REMOVE_ITEM COMMON_SRCS "${absf}")
  list(APPEND COMMON_GUI_SRCS "${absf}")
endforeach()
foreach(prefix "/UI/" "/Preview/" "/xrc_handlers/")
  set(matched ${COMMON_SRCS})
  list(FILTER matched INCLUDE REGEX "${prefix}.*\\.cpp$")
  list(REMOVE_ITEM COMMON_SRCS ${matched})
  list(APPEND COMMON_GUI_SRCS ${matched})
endforeach()

list(LENGTH COMMON_SRCS _n_common)
list(LENGTH COMMON_GUI_SRCS _n_common_gui)
list(LENGTH MACRO_SENSITIVE_GUI_ONLY_SRCS _n_sens)
message(STATUS "wxCrafter source bucketing:")
message(
  STATUS
    "  wxCrafterCommon    (compiled once, all targets):    ${_n_common} files")
message(
  STATUS
    "  wxCrafterCommonGui (compiled once, GUI targets):    ${_n_common_gui} files"
)
message(
  STATUS
    "  macro-sensitive    (compiled per-target, all):      ${_n_sens} files")

include_directories(
  "${CMAKE_CURRENT_LIST_DIR}"
  "${CMAKE_CURRENT_LIST_DIR}/aui"
  "${CMAKE_CURRENT_LIST_DIR}/controls"
  "${CMAKE_CURRENT_LIST_DIR}/sizers"
  "${CMAKE_CURRENT_LIST_DIR}/src"
  "${CMAKE_CURRENT_LIST_DIR}/top_level_windows"
  "${CMAKE_CURRENT_LIST_DIR}/xrc_handlers")

set(RES_FILE "")
# Define the output
if(MINGW)
  set(RES_FILES "resources/wxcapp_resources.rc")
endif()

set(WXCGEN_RES_FILES "")
if(MINGW)
  set(WXCGEN_RES_FILES "resources/wxcgen_resources.rc")
endif()

# ----------------------------------------------------------------------------
# Non GUI targets
# ----------------------------------------------------------------------------
# wxCrafterCommon: define-agnostic, headless-safe TUs. Compiled once, linked
# into all three targets (wxcrafter, wxCrafterPlugin, wxcgen).
add_library(wxCrafterCommon STATIC ${COMMON_SRCS})
set_target_properties(wxCrafterCommon PROPERTIES POSITION_INDEPENDENT_CODE ON)
# Link the CodeLite internal libs so the static lib picks up their INTERFACE
# include directories (JSON.h, etc.). Static libs don't actually link against
# other libs at archive time, but CMake will propagate include dirs and compile
# features.
target_link_libraries(wxCrafterCommon PUBLIC libcodelite)
if(USE_PCH)
  target_precompile_headers(wxCrafterCommon REUSE_FROM PCH)
endif()

# Headless CLI code-generator (no DISPLAY required).
add_executable(wxcgen ${MACRO_SENSITIVE_HEADLESS_SRCS} "${WXCGEN_RES_FILES}"
                      "${CMAKE_CURRENT_LIST_DIR}/wxcgen/main.cpp")
target_compile_definitions(wxcgen
                           PRIVATE INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
target_link_libraries(wxcgen PRIVATE wxCrafterCommon)

macro(
  wxcrafter_target
  Name
  Input
  OutputDir
  OutputCpp
  OutputHpp
  OutputBitmaps
  # Extra output for models
)

  set(WXCRAFTER_${Name}_OUTPUTS
      "${OutputDir}/${OutputCpp}" "${OutputDir}/${OutputHpp}"
      "${OutputDir}/${OutputBitmaps}")

  set(ExtraMacroArgs ${ARGN}) # ARGN is not a variable
  foreach(ExtraArgs ${ExtraMacroArgs})
    list(APPEND WXCRAFTER_${Name}_OUTPUTS "${OutputDir}/${ExtraArgs}")
  endforeach()
  unset(ExtraMacroArgs)

  add_custom_command(
    OUTPUT ${WXCRAFTER_${Name}_OUTPUTS}
    COMMAND $<TARGET_FILE:wxcgen> -o "${OutputDir}" "${Input}"
    VERBATIM
    DEPENDS ${Input} wxcgen
    COMMENT "[WxCrafter][${Name}] Building code (wxcgen)")

  set(WXCRAFTER_${Name}_OUTPUTDIR "${OutputDir}")
endmacro()

# ----------------------------------------------------------------------------
# GUI targets
# ----------------------------------------------------------------------------

# wxCrafterCommonGui: define-agnostic but GUI-coupled TUs. Compiled once, linked
# into wxcrafter and wxCrafterPlugin only — NOT into wxcgen.

wxcrafter_target(
  WxCrafterGui "${CMAKE_CURRENT_SOURCE_DIR}/UI/gui.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "gui.cpp" "gui.hpp" "gui_wxcrafter_bitmaps.cpp")

wxcrafter_target(
  WxCrafterWxCrafter "${CMAKE_CURRENT_SOURCE_DIR}/UI/wxcrafter.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "wxcrafter.cpp" "wxcrafter.hpp"
  "wxcrafter_wxcrafter_bitmaps.cpp")

wxcrafter_target(
  WxCrafterWxCrafterGui "${CMAKE_CURRENT_SOURCE_DIR}/UI/wxcrafter_gui.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "wxcrafter_gui.cpp" "wxcrafter_gui.hpp"
  "wxcrafter_gui_wxcrafter_bitmaps.cpp")

add_library(
  wxCrafterCommonGui STATIC
  ${COMMON_GUI_SRCS} ${WXCRAFTER_WxCrafterGui_OUTPUTS}
  ${WXCRAFTER_WxCrafterWxCrafter_OUTPUTS}
  ${WXCRAFTER_WxCrafterWxCrafterGui_OUTPUTS})
set_target_properties(wxCrafterCommonGui PROPERTIES POSITION_INDEPENDENT_CODE
                                                    ON)
target_include_directories(wxCrafterCommonGui
                           PUBLIC "${WXCRAFTER_WxCrafterGui_OUTPUTDIR}")
target_link_libraries(wxCrafterCommonGui PUBLIC libcodelite plugin)
if(USE_PCH)
  target_precompile_headers(wxCrafterCommonGui REUSE_FROM PCH)
endif()

# ----------------------------------------------------------------------------
# Per-flavor targets
# ----------------------------------------------------------------------------
# Each target compiles the macro-sensitive sources itself (with the right
# defines) and links the common static libs above.

# wxCrafter as a CodeLite plugin (shared library)
add_library(wxCrafterPlugin SHARED ${MACRO_SENSITIVE_GUI_ONLY_SRCS})
target_link_libraries(wxCrafterPlugin PRIVATE wxCrafterCommonGui
                                              wxCrafterCommon)

# wxCrafter as a standalone GUI application
add_executable(wxcrafter ${MACRO_SENSITIVE_GUI_ONLY_SRCS} ${RES_FILES})
target_compile_definitions(
  wxcrafter PRIVATE STANDALONE_BUILD=1
                    INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
target_link_libraries(wxcrafter PRIVATE wxCrafterCommonGui wxCrafterCommon)

if(WXC_APP)
  set_target_properties(wxCrafterPlugin PROPERTIES EXCLUDE_FROM_ALL ON)
else()
  set_target_properties(wxcrafter PROPERTIES EXCLUDE_FROM_ALL ON)
endif()

if(UNIX OR APPLE)
  set_target_properties(wxCrafterPlugin PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set_target_properties(wxcrafter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
  target_precompile_headers(wxCrafterPlugin REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(wxCrafterPlugin PROPERTIES OUTPUT_NAME "wxCrafter" PREFIX
                                                                         "")

if(NOT OFFICIAL_CL_BUILD)
  target_link_libraries(
    wxCrafterPlugin PRIVATE ${LINKER_OPTIONS} ${SQLite3_LIBRARIES} libcodelite
                            plugin)
else(OFFICIAL_CL_BUILD)
  target_link_libraries(wxCrafterPlugin PRIVATE ${LINKER_OPTIONS})
endif(NOT OFFICIAL_CL_BUILD)

target_link_libraries(wxcrafter PRIVATE ${LINKER_OPTIONS}
                                        $<IF:$<BOOL:${MINGW}>,-mwindows,-lutil>)

target_link_libraries(wxcgen PRIVATE ${LINKER_OPTIONS}
                                     $<IF:$<BOOL:${MINGW}>,-mconsole,-lutil>)

if(UNIX AND NOT APPLE)
  set_target_properties(wxcrafter PROPERTIES SKIP_BUILD_RPATH FALSE
                                             BUILD_WITH_INSTALL_RPATH FALSE)
  set_target_properties(wxcgen PROPERTIES SKIP_BUILD_RPATH FALSE
                                          BUILD_WITH_INSTALL_RPATH FALSE)
endif(UNIX AND NOT APPLE)

if(MINGW)
  execute_process(
    COMMAND sh -c
            'ls ${WXWIN}/lib/clang_x64_dll/wxmsw*u_clang*custom.dll|tail -1'
    OUTPUT_VARIABLE WXDLL
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(NOT WXDLL)
    message(FATAL_ERROR "Could not locate wxWidgets DLL")
  endif()

  # Copy dlls near the executable
  add_custom_command(
    TARGET wxcrafter
    POST_BUILD
    COMMAND cmake -E copy_if_different ${WXDLL} $<TARGET_FILE_DIR:wxcrafter>
    COMMAND_EXPAND_LISTS)
  add_custom_command(
    TARGET wxcrafter
    POST_BUILD
    COMMAND cmake -E copy_if_different $<TARGET_RUNTIME_DLLS:wxcrafter>
            $<TARGET_FILE_DIR:wxcrafter>
    COMMAND_EXPAND_LISTS)

  # Get DLL dependencies for additional libraries needed by wxcgen
  msys_list_deps(${MSYS2_BASE}/${MSYSTEM}/bin/libssh.dll LIBSSH_DLLS)
  msys_list_deps(${MSYS2_BASE}/${MSYSTEM}/bin/libc++.dll LIBCXX_DLLS)
  msys_list_deps(${MSYS2_BASE}/${MSYSTEM}/bin/openssl.exe OPENSSL_DLLS)

  # Combine all additional DLLs
  list(APPEND WXCGEN_EXTRA_DLLS ${LIBSSH_DLLS} ${LIBCXX_DLLS} ${OPENSSL_DLLS})
  list(APPEND WXCGEN_EXTRA_DLLS "${MSYS2_BASE}/${MSYSTEM}/bin/libsqlite3-0.dll")
  list(REMOVE_DUPLICATES WXCGEN_EXTRA_DLLS)

  # Copy dlls near the wxcgen executable
  add_custom_command(
    TARGET wxcgen
    POST_BUILD
    COMMAND cmake -E copy_if_different ${WXDLL} $<TARGET_FILE_DIR:wxcgen>
    COMMAND_EXPAND_LISTS)
  add_custom_command(
    TARGET wxcgen
    POST_BUILD
    COMMAND cmake -E copy_if_different $<TARGET_RUNTIME_DLLS:wxcgen>
            $<TARGET_FILE_DIR:wxcgen>
    COMMAND_EXPAND_LISTS)

  # Copy additional dependencies for wxcgen
  foreach(DLL ${WXCGEN_EXTRA_DLLS})
    add_custom_command(
      TARGET wxcgen
      POST_BUILD
      COMMAND cmake -E copy_if_different "${DLL}" $<TARGET_FILE_DIR:wxcgen>
      COMMAND_EXPAND_LISTS)
  endforeach()
endif(MINGW)

if(NOT WXC_APP)
  # wxCrafter as CodeLite's plugin
  cl_install_plugin(wxCrafterPlugin)
  cl_install_file_shared(${WXC_DIR}/wxgui.zip)

elseif(NOT MINGW)
  set(CPACK_DEB_COMPONENT_INSTALL 1)

  # Installing the applications
  install(
    TARGETS wxcrafter wxcgen
    DESTINATION ${CL_INSTALL_BIN}
    PERMISSIONS ${EXE_PERM}
    COMPONENT standalone)

  install(
    FILES "${WXC_DIR}/wxgui.zip"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}
    COMPONENT standalone)

  install(
    FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/liblibcodelite.so"
    DESTINATION ${WXC_LIBS_DIR}
    COMPONENT standalone)
  install(
    FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libplugin.so"
    DESTINATION ${WXC_LIBS_DIR}
    COMPONENT standalone)
  install(
    FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libwxsqlite3.so"
    DESTINATION ${WXC_LIBS_DIR}
    COMPONENT standalone)
  install(
    FILES "${CL_SRC_ROOT}/wxcrafter/resources/wxc-logo-64.png"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}
    COMPONENT standalone)
  install(
    FILES "${CL_SRC_ROOT}/Runtime/config/codelite.xml.default.gtk"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}/config
    RENAME codelite.xml.default
    COMPONENT standalone)

  if(NOT DEBUG_BUILD)
    message(
      "-- Will install ${CL_SRC_ROOT}/wxcrafter/wxCrafter.desktop to /usr/share/applications"
    )
    install(
      FILES "${CL_SRC_ROOT}/wxcrafter/wxCrafter.desktop"
      DESTINATION /usr/share/applications
      COMPONENT standalone)
  endif(NOT DEBUG_BUILD)

  # Install codelite lexer files
  install(
    DIRECTORY ${CL_SRC_ROOT}/Runtime/lexers
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}
    COMPONENT standalone
    USE_SOURCE_PERMISSIONS
    PATTERN ".svn" EXCLUDE
    PATTERN ".git" EXCLUDE)

  # ############################################################################
  # CPack
  # ############################################################################
  message("-- Generating deb target")
  message("-- CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

  # ############################################################################
  # We need to deduce the location of the wx libs to be installed
  # ############################################################################
  execute_process(
    COMMAND ${WX_TOOL} --libs
    OUTPUT_VARIABLE WX_LIBSOUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(${WX_LIBSOUTPUT} MATCHES "^-L.+")
    # In recent, multi-architecture, distro versions it'll start with -L/foo/bar
    string(REGEX REPLACE "^-L([^ ;]+).*" "\\1" WX_LIBS_DIR ${WX_LIBSOUTPUT})
  else()
    set(WX_LIBS_DIR "/usr/lib")
    # ${WX_LIBS_DIR} will be empty for older, non multi-architecture, distro
    # versions that still use /usr/lib
  endif()
  message("-- wxlibsdir is: ${WXC_LIBS_DIR}")

  # Grab the necessary libs
  execute_process(
    COMMAND ${WX_TOOL} --libs all
    OUTPUT_VARIABLE WX_LIBS_ALL_IN
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REPLACE " " ";" WX_LIBS_ALL_LIST ${WX_LIBS_ALL_IN})
  # the semicolons make the variable into a list

  foreach(ITEM ${WX_LIBS_ALL_LIST})
    # We want to replace -l with lib. We also want to exclude imaginary libs
    # called _all.so, which buggy wx-config scripts can invent
    if(${ITEM} MATCHES "-lwx_.+" AND NOT ${ITEM} MATCHES "-l.+_all.+")
      string(REGEX REPLACE "^-l" "lib" LIBITEM ${ITEM})
      list(APPEND WX_LIBS_ALL ${WX_LIBS_DIR}/${LIBITEM}.so*)
      # we need the terminal [.]so because the rpms contain both dynamic and
      # static libs
    endif()
  endforeach()

  file(GLOB wxfilepaths ${WX_LIBS_ALL})
  install(
    FILES ${wxfilepaths}
    DESTINATION ${WXC_LIBS_DIR}
    COMPONENT standalone)

  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "wxCrafter${GTKNAME}")
  set(CPACK_PACKAGE_VERSION "${CODELITE_VERSION}")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Eran Ifrah <eran@codelite.org>")
  # required
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUI Rad Tool for wxWidgets")
  set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "build-essential")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libssh-4, libsqlite3-dev")
  include(CPack)
endif()

if(WXC_APP AND MINGW)
  install(TARGETS wxcrafter wxcgen DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/")
  # Install codelite lexer files
  install(
    DIRECTORY ${CL_SRC_ROOT}/Runtime/lexers
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}
    COMPONENT standalone
    USE_SOURCE_PERMISSIONS
    PATTERN ".svn" EXCLUDE
    PATTERN ".git" EXCLUDE)
endif()
message(STATUS "wxCrafter - Done")
