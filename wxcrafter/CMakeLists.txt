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
# We have three target flavors that differ only in two preprocessor symbols:
#
#   target            STANDALONE_BUILD   WXCGEN_BUILD
#   ----------------- ----------------   ------------
#   wxCrafterPlugin   undefined          undefined
#   wxcrafter (exe)   1                  undefined
#   wxcgen (exe)      1                  1
#
# Most TUs don't reference either macro; their object files are bit-identical
# across all flavors and can be compiled once, into a shared static library.
# Only a handful of TUs actually #if on the macros and must be compiled per
# flavor.
#
# Bucket the files into:
#   MACRO_SENSITIVE_SRCS — TUs whose object code depends on the defines above.
#                          Compiled per target with the right defines.
#   GUI_ONLY_SRCS        — TUs that pull GUI-only symbols (MainFrame, etc.).
#                          NOT linked into wxcgen.
#   COMMON_GUI_SRCS      — define-agnostic but GUI-coupled. Compiled into
#                          wxCrafterCommonGui (linked by GUI targets only).
#   COMMON_SRCS          — define-agnostic AND headless-safe. Compiled into
#                          wxCrafterCommon (linked by all three targets).

# wxc_runtime_gui.cpp / wxc_runtime_headless.cpp are picked per target — they
# don't belong in any common library.
list(FILTER ALL_SRCS EXCLUDE REGEX ".*/src/wxc_runtime_(gui|headless)\\.cpp$")

# The 11 .cpp files that #if on STANDALONE_BUILD or WXCGEN_BUILD (verified by
# `grep -l "STANDALONE_BUILD\|WXCGEN_BUILD" **/*.cpp`).
set(MACRO_SENSITIVE_FILES
    "src/main.cpp"
    "src/allocator_mgr.cpp"
    "src/wxcrafter_plugin.cpp"
    "src/wxguicraft_main_view.cpp"
    "src/wxgui_helpers.cpp"
    "UI/NewFormWizard.cpp"
    "UI/wxcSettingsDlg.cpp"
    "Importer/import_dlg.cpp"
    "Importer/import_from_wxSmith.cpp"
    "Importer/import_from_xrc.cpp")

# Of the macro-sensitive files, these are GUI-only (define MainFrame,
# GUICraftMainPanel, plugin glue, GUI dialogs) and must NOT be linked into
# the headless wxcgen executable.
set(MACRO_SENSITIVE_GUI_ONLY_FILES
    "src/main.cpp"
    "src/wxcrafter_plugin.cpp"
    "src/wxguicraft_main_view.cpp"
    "UI/NewFormWizard.cpp"
    "UI/wxcSettingsDlg.cpp"
    "Importer/import_dlg.cpp")

# The macro-sensitive files that wxcgen does compile (with WXCGEN_BUILD=1).
set(MACRO_SENSITIVE_HEADLESS_FILES ${MACRO_SENSITIVE_FILES})
foreach(f ${MACRO_SENSITIVE_GUI_ONLY_FILES})
  list(REMOVE_ITEM MACRO_SENSITIVE_HEADLESS_FILES "${f}")
endforeach()

# Build absolute-path source lists.
set(MACRO_SENSITIVE_SRCS)
foreach(f ${MACRO_SENSITIVE_FILES})
  list(APPEND MACRO_SENSITIVE_SRCS "${CMAKE_CURRENT_LIST_DIR}/${f}")
endforeach()
set(MACRO_SENSITIVE_GUI_ONLY_SRCS)
foreach(f ${MACRO_SENSITIVE_GUI_ONLY_FILES})
  list(APPEND MACRO_SENSITIVE_GUI_ONLY_SRCS "${CMAKE_CURRENT_LIST_DIR}/${f}")
endforeach()
set(MACRO_SENSITIVE_HEADLESS_SRCS)
foreach(f ${MACRO_SENSITIVE_HEADLESS_FILES})
  list(APPEND MACRO_SENSITIVE_HEADLESS_SRCS "${CMAKE_CURRENT_LIST_DIR}/${f}")
endforeach()

# Define-agnostic files split into headless-safe (COMMON_SRCS) and GUI-only
# (COMMON_GUI_SRCS). The split is based on whether the TU pulls GUI-only
# symbols (MainFrame, GUICraftMainPanel, wxDialog subclasses, etc.).
set(COMMON_GUI_ONLY_FILES
    # generated wxFormBuilder GUI base classes
    "src/gui.cpp"
    "src/gui_wxcrafter_bitmaps.cpp"
    "src/wxcrafter.cpp"
    "src/wxcrafter_gui.cpp"
    "src/wxcrafter_gui_wxcrafter_bitmaps.cpp"
    "src/wxcrafter_wxcrafter_bitmaps.cpp"
    # GUI dialogs / widgets defined in src/
    "src/AboutDlg.cpp"
    "src/BitmapSelectorDlg.cpp"
    "src/designer_panel.cpp"
    "src/duplicateTLWdlg.cpp"
    "src/enter_strings_dlg.cpp"
    "src/EventsEditorDlg.cpp"
    "src/file_picker_ctrl.cpp"
    "src/MediaCtrl.cpp"
    "src/my_dialog.cpp"
    "src/my_frame.cpp"
    "src/my_panel.cpp"
    "src/OpenGLCanvas.cpp"
    "src/output_nbook.cpp"
    "src/wxc_aui_manager.cpp"
    "src/wxc_aui_tool_stickiness.cpp"
    "src/wxc_edit_manager.cpp"
    "src/wxcLicenseGPL2.cpp"
	"Importer/importer_from_wxFB.cpp")

# Start with everything except the macro-sensitive files (they're handled
# separately per target).
set(COMMON_SRCS ${ALL_SRCS})
list(REMOVE_ITEM COMMON_SRCS ${MACRO_SENSITIVE_SRCS})

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
list(LENGTH MACRO_SENSITIVE_SRCS _n_sens)
list(LENGTH MACRO_SENSITIVE_HEADLESS_SRCS _n_sens_headless)
message(STATUS "wxCrafter source bucketing:")
message(STATUS "  wxCrafterCommon    (compiled once, all targets):    ${_n_common} files")
message(STATUS "  wxCrafterCommonGui (compiled once, GUI targets):    ${_n_common_gui} files")
message(STATUS "  macro-sensitive    (compiled per-target, all):      ${_n_sens} files")
message(STATUS "  macro-sensitive    (compiled per-target, headless): ${_n_sens_headless} files")

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
if(WIN32)
  set(WXCGEN_RES_FILES "resources/wxcgen_resources.rc")
endif()

# ----------------------------------------------------------------------------
# Shared static libraries
# ----------------------------------------------------------------------------
# wxCrafterCommon: define-agnostic, headless-safe TUs. Compiled once, linked
# into all three targets (wxcrafter, wxCrafterPlugin, wxcgen).
add_library(wxCrafterCommon STATIC ${COMMON_SRCS})
set_target_properties(wxCrafterCommon PROPERTIES POSITION_INDEPENDENT_CODE ON)
# Link the CodeLite internal libs so the static lib picks up their INTERFACE
# include directories (JSON.h, plugin.h, wxsqlite3, etc.). Static libs don't
# actually link against other libs at archive time, but CMake will propagate
# include dirs and compile features.
target_link_libraries(wxCrafterCommon PUBLIC libcodelite plugin wxsqlite3)
if(USE_PCH)
  target_precompile_headers(wxCrafterCommon REUSE_FROM PCH)
endif()

# wxCrafterCommonGui: define-agnostic but GUI-coupled TUs. Compiled once,
# linked into wxcrafter and wxCrafterPlugin only — NOT into wxcgen.
add_library(wxCrafterCommonGui STATIC ${COMMON_GUI_SRCS})
set_target_properties(wxCrafterCommonGui PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(wxCrafterCommonGui PUBLIC libcodelite plugin wxsqlite3)
if(USE_PCH)
  target_precompile_headers(wxCrafterCommonGui REUSE_FROM PCH)
endif()

# ----------------------------------------------------------------------------
# Per-flavor targets
# ----------------------------------------------------------------------------
# Each target compiles the macro-sensitive sources itself (with the right
# defines) and links the common static libs above.

# wxCrafter as a CodeLite plugin (shared library) — uses GUI runtime shim
add_library(wxCrafterPlugin SHARED ${MACRO_SENSITIVE_SRCS}
                                   "${CMAKE_CURRENT_LIST_DIR}/src/wxc_runtime_gui.cpp")
target_link_libraries(wxCrafterPlugin PRIVATE wxCrafterCommonGui wxCrafterCommon)

# wxCrafter as a standalone GUI application — uses GUI runtime shim
add_executable(wxcrafter ${MACRO_SENSITIVE_SRCS}
                         "${CMAKE_CURRENT_LIST_DIR}/src/wxc_runtime_gui.cpp"
                         ${RES_FILES})
target_compile_definitions(
  wxcrafter PRIVATE STANDALONE_BUILD=1
                    INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
target_link_libraries(wxcrafter PRIVATE wxCrafterCommonGui wxCrafterCommon)

# Headless CLI code-generator (no DISPLAY required) — uses headless runtime
# shim and a subset of the macro-sensitive sources (the GUI-only ones are
# excluded; what remains is compiled with WXCGEN_BUILD=1).
add_executable(wxcgen ${MACRO_SENSITIVE_HEADLESS_SRCS}
                      "${CMAKE_CURRENT_LIST_DIR}/wxcgen/main.cpp"
                      "${CMAKE_CURRENT_LIST_DIR}/src/wxc_runtime_headless.cpp"
                      ${WXCGEN_RES_FILES})
target_compile_definitions(
  wxcgen PRIVATE STANDALONE_BUILD=1
                 WXCGEN_BUILD=1
                 INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
target_link_libraries(wxcgen PRIVATE wxCrafterCommon)

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
set_target_properties(wxCrafterPlugin PROPERTIES OUTPUT_NAME "wxCrafter" PREFIX "")

if(NOT OFFICIAL_CL_BUILD)
  target_link_libraries(wxCrafterPlugin PRIVATE ${LINKER_OPTIONS} wxsqlite3
                        ${SQLite3_LIBRARIES} libcodelite plugin)
else(OFFICIAL_CL_BUILD)
  target_link_libraries(wxCrafterPlugin PRIVATE ${LINKER_OPTIONS})
endif(NOT OFFICIAL_CL_BUILD)

target_link_libraries(wxcrafter PRIVATE ${LINKER_OPTIONS} wxsqlite3 libcodelite plugin
                      $<IF:$<BOOL:${MINGW}>,-mwindows,-lutil>)

target_link_libraries(wxcgen PRIVATE ${LINKER_OPTIONS} wxsqlite3 libcodelite plugin
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
    "${OutputDir}/${OutputCpp}"
    "${OutputDir}/${OutputHpp}"
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

message(STATUS "wxCrafter - Done")
