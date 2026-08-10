# Our project is called 'plugin' this is how it will be called in visual studio,
# and in our makefiles.
project(plugin)

# It was noticed that when using MinGW gcc it is essential that 'core' is
# mentioned before 'base'.

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

wxcrafter_target(
    PluginAi
    "${CMAKE_CURRENT_SOURCE_DIR}/ai/ai_UI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "ai_UI.cpp"
    "ai_UI.hpp"
    "UI_assistance_ai_bitmaps.cpp")

wxcrafter_target(
    PluginCustomControls
    "${CMAKE_CURRENT_SOURCE_DIR}/CustomControls/CustomControls.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "CustomControls.cpp"
    "CustomControls.hpp"
    "CustomControls_customcontrols_bitmaps.cpp")

wxcrafter_target(
    PluginDebugger
    "${CMAKE_CURRENT_SOURCE_DIR}/Debugger/memoryviewbase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "memoryviewbase.cpp"
    "memoryviewbase.hpp"
    "memoryviewbase_liteeditor_bitmaps.cpp")

wxcrafter_target(
    PluginDiff
    "${CMAKE_CURRENT_SOURCE_DIR}/Diff/DiffUI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "DiffUI.cpp"
    "DiffUI.hpp"
    "DiffUI_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginFileSystemWorkspace
    "${CMAKE_CURRENT_SOURCE_DIR}/FileSystemWorkspace/clFileSystemWorkspaceDlgBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "clFileSystemWorkspaceDlgBase.cpp"
    "clFileSystemWorkspaceDlgBase.hpp"
    "clFileSystemWorkspaceDlgBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginKeyboard
    "${CMAKE_CURRENT_SOURCE_DIR}/Keyboard/NewKeyShortcutBaseDlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "NewKeyShortcutBaseDlg.cpp"
    "NewKeyShortcutBaseDlg.hpp"
    "NewKeyShortcutBaseDlg_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginLSP
    "${CMAKE_CURRENT_SOURCE_DIR}/LSP/lsp_UI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "lsp_UI.cpp"
    "lsp_UI.hpp"
    "UI_languageserver_bitmaps.cpp")

wxcrafter_target(
    PluginTerminalEmulator
    "${CMAKE_CURRENT_SOURCE_DIR}/TerminalEmulator/TerminalEmulatorUI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "TerminalEmulatorUIBase.cpp"
    "TerminalEmulatorUIBase.hpp"
    "TerminalEmulatorUI_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginWxTerminal
    "${CMAKE_CURRENT_SOURCE_DIR}/wxTerminalCtrl/wxTerminalCtrlUI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "wxTerminalCtrlUI.cpp"
    "wxTerminalCtrlUI.hpp"
    "wxTerminalCtrlUI_wxterminalctrl_bitmaps.cpp")

wxcrafter_target(
    PluginAddIncludeFile
    "${CMAKE_CURRENT_SOURCE_DIR}/AddIncludeFile.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "addincludefiledlgbase.cpp"
    "addincludefiledlgbase.hpp"
    "AddIncludeFile_liteeditor_bitmaps.cpp")

wxcrafter_target(
    PluginAboutDialog
    "${CMAKE_CURRENT_SOURCE_DIR}/clAboutDialogBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "clAboutDialogBase.cpp"
    "clAboutDialogBase.hpp"
    "clAboutDialogBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginBoostrapWizard
    "${CMAKE_CURRENT_SOURCE_DIR}/clBoostrapWizardBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "clBoostrapWizardBase.cpp"
    "clBoostrapWizardBase.hpp"
    "clBoostrapWizardBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginProgressDlg
    "${CMAKE_CURRENT_SOURCE_DIR}/clprogressdlgbase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "clprogressdlgbase.cpp"
    "clprogressdlgbase.hpp"
    "clprogressdlgbase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginResizableTooltip
    "${CMAKE_CURRENT_SOURCE_DIR}/clResizableTooltipBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "clResizableTooltipBase.cpp"
    "clResizableTooltipBase.hpp"
    "clResizableTooltipBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginSelectSymbolDialog
    "${CMAKE_CURRENT_SOURCE_DIR}/clSelectSymbolDialogBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "clSelectSymbolDialogBase.cpp"
    "clSelectSymbolDialogBase.hpp"
    "clSelectSymbolDialogBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginEnvVarTableDlg
    "${CMAKE_CURRENT_SOURCE_DIR}/envvar_table_dlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "EnvironmentVariablesDlgBase.cpp"
    "EnvironmentVariablesDlgBase.hpp"
    "EnvironmentVariablesDlgBaseBitmaps.cpp")

wxcrafter_target(
    PluginGotoAnythingBaseUI
    "${CMAKE_CURRENT_SOURCE_DIR}/GotoAnythingBaseUI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "GotoAnythingBaseUI.cpp"
    "GotoAnythingBaseUI.hpp"
    "GotoAnythingBaseUI_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginMacrosBaseDlg
    "${CMAKE_CURRENT_SOURCE_DIR}/MacrosBaseDlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "macrosbasedlg.cpp"
    "macrosbasedlg.hpp"
    "MacrosBaseDlg_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginNameAndDescDlg
    "${CMAKE_CURRENT_SOURCE_DIR}/nameanddescdlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "nameanddescbasedlg.cpp"
    "nameanddescbasedlg.hpp"
    "nameanddescdlg_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginNewProjectDialog
    "${CMAKE_CURRENT_SOURCE_DIR}/NewProjectDialogBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "NewProjectDialogBase.cpp"
    "NewProjectDialogBase.hpp"
    "NewProjectDialogBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginOpenResourceDialog
    "${CMAKE_CURRENT_SOURCE_DIR}/openresourcedialogbase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "openresourcedialogbase.cpp"
    "openresourcedialogbase.hpp"
    "openresourcedialogbase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginGeneralWxcp
    "${CMAKE_CURRENT_SOURCE_DIR}/plugin_general_wxcp.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "plugin_general_wxcp.cpp"
    "plugin_general_wxcp.hpp"
    "plugin_general_wxcp_plugin_bitmaps.cpp")

#wxcrafter_target(
#    PluginQuickFindBar
#    "${CMAKE_CURRENT_SOURCE_DIR}/quickfindbarbase.wxcp"
#    "${CMAKE_CURRENT_BINARY_DIR}"
#    "quickfindbarbase.cpp"
#    "quickfindbarbase.hpp"
#    "quickfindbarbase_bitmaps.cpp")

wxcrafter_target(
    PluginSelectFileTypesDialog
    "${CMAKE_CURRENT_SOURCE_DIR}/SelectFileTypesDialogBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "SelectFileTypesDialogBase.cpp"
    "SelectFileTypesDialogBase.hpp"
    "SelectFileTypesDialogBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginSftpUi
    "${CMAKE_CURRENT_SOURCE_DIR}/sftp_ui.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "sftp_ui.cpp"
    "sftp_ui.hpp"
    "sftp_ui_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginSwitchToWorkspace
    "${CMAKE_CURRENT_SOURCE_DIR}/SwitchToWorkspaceBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "SwitchToWorkspaceBase.cpp"
    "SwitchToWorkspaceBase.hpp"
    "SwitchToWorkspaceBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginToolBar
    "${CMAKE_CURRENT_SOURCE_DIR}/ToolBar.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "ToolBarUI.cpp"
    "ToolBarUI.hpp"
    "ToolBar_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginVirtualDirectorySelector
    "${CMAKE_CURRENT_SOURCE_DIR}/VirtualDirectorySelectorBase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "VirtualDirectorySelectorBase.cpp"
    "VirtualDirectorySelectorBase.hpp"
    "VirtualDirectorySelectorBase_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginWSImporterDlgs
    "${CMAKE_CURRENT_SOURCE_DIR}/WSImporterDlgs.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "WSImporterDlgs.cpp"
    "WSImporterDlgs.hpp"
    "WSImporterDlgs_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginCodeCompletionBox
    "${CMAKE_CURRENT_SOURCE_DIR}/wxCodeCompletionBox.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "wxCodeCompletionBoxBase.cpp"
    "wxCodeCompletionBoxBase.hpp"
    "wxCodeCompletionBox_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginWxCrafter
    "${CMAKE_CURRENT_SOURCE_DIR}/wxcrafter.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "wxcrafter_plugin.cpp"
    "wxcrafter_plugin.hpp"
    "wxcrafter_plugin_bitmaps.cpp")

wxcrafter_target(
    PluginRenameFile
    "${CMAKE_CURRENT_SOURCE_DIR}/renamefilebasedlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "renamefilebasedlg.cpp"
    "renamefilebasedlg.hpp"
    "renamefilebasedlg_plugin_bitmaps.cpp")

#wxcrafter_target(
#    PluginTerminal
#    "${CMAKE_CURRENT_SOURCE_DIR}/wxterminalbase.wxcp"
#    "${CMAKE_CURRENT_BINARY_DIR}"
#    "wxterminalbase.cpp"
#    "wxterminalbase.hpp"
#    "wxterminalbase_plugin_bitmaps.cpp")


if(MINGW)
  file(GLOB_RECURSE SRCS "*.cpp" "*.h" "*.hpp")
  list(FILTER SRCS EXCLUDE REGEX ".*/macos/.*")
  list(FILTER SRCS EXCLUDE REGEX ".*/gtk/.*")
elseif(APPLE)
  file(GLOB_RECURSE SRCS "*.cpp" "*.h" "*.hpp")
  list(FILTER SRCS EXCLUDE REGEX ".*/msw/.*")
  list(FILTER SRCS EXCLUDE REGEX ".*/gtk/.*")
else()
  file(GLOB_RECURSE SRCS "*.cpp" "*.h" "*.hpp")
  list(FILTER SRCS EXCLUDE REGEX ".*/msw/.*")
  list(FILTER SRCS EXCLUDE REGEX ".*/macos/.*")
endif()

add_library(plugin SHARED ${SRCS}
    ${WXCRAFTER_PluginAi_OUTPUTS}
    ${WXCRAFTER_PluginCustomControls_OUTPUTS}
    ${WXCRAFTER_PluginDebugger_OUTPUTS}
    ${WXCRAFTER_PluginDiff_OUTPUTS}
    ${WXCRAFTER_PluginFileSystemWorkspace_OUTPUTS}
    ${WXCRAFTER_PluginKeyboard_OUTPUTS}
    ${WXCRAFTER_PluginLSP_OUTPUTS}
    ${WXCRAFTER_PluginTerminalEmulator_OUTPUTS}
    ${WXCRAFTER_PluginWxTerminal_OUTPUTS}
    ${WXCRAFTER_PluginAddIncludeFile_OUTPUTS}
    ${WXCRAFTER_PluginAboutDialog_OUTPUTS}
    ${WXCRAFTER_PluginBoostrapWizard_OUTPUTS}
    ${WXCRAFTER_PluginProgressDlg_OUTPUTS}
    ${WXCRAFTER_PluginResizableTooltip_OUTPUTS}
    ${WXCRAFTER_PluginSelectSymbolDialog_OUTPUTS}
    ${WXCRAFTER_PluginEnvVarTableDlg_OUTPUTS}
    ${WXCRAFTER_PluginGotoAnythingBaseUI_OUTPUTS}
    ${WXCRAFTER_PluginMacrosBaseDlg_OUTPUTS}
    ${WXCRAFTER_PluginNameAndDescDlg_OUTPUTS}
    ${WXCRAFTER_PluginNewProjectDialog_OUTPUTS}
    ${WXCRAFTER_PluginOpenResourceDialog_OUTPUTS}
    ${WXCRAFTER_PluginGeneralWxcp_OUTPUTS}
#    ${WXCRAFTER_PluginQuickFindBar_OUTPUTS}
    ${WXCRAFTER_PluginSelectFileTypesDialog_OUTPUTS}
    ${WXCRAFTER_PluginSftpUi_OUTPUTS}
    ${WXCRAFTER_PluginSwitchToWorkspace_OUTPUTS}
    ${WXCRAFTER_PluginToolBar_OUTPUTS}
    ${WXCRAFTER_PluginVirtualDirectorySelector_OUTPUTS}
    ${WXCRAFTER_PluginWSImporterDlgs_OUTPUTS}
    ${WXCRAFTER_PluginCodeCompletionBox_OUTPUTS}
    ${WXCRAFTER_PluginWxCrafter_OUTPUTS}
    ${WXCRAFTER_PluginRenameFile_OUTPUTS}
#    ${WXCRAFTER_PluginTerminal_OUTPUTS}
)

# Include paths
target_include_directories(
  plugin
  PUBLIC "${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/Interfaces"
         "${CL_SRC_ROOT}/Plugin/aui"
          "${WXCRAFTER_PluginAi_OUTPUTDIR}"
          "${WXCRAFTER_PluginCustomControls_OUTPUTDIR}"
          "${WXCRAFTER_PluginDebugger_OUTPUTDIR}"
          "${WXCRAFTER_PluginDiff_OUTPUTDIR}"
          "${WXCRAFTER_PluginFileSystemWorkspace_OUTPUTDIR}"
          "${WXCRAFTER_PluginKeyboard_OUTPUTDIR}"
          "${WXCRAFTER_PluginLSP_OUTPUTDIR}"
          "${WXCRAFTER_PluginAddIncludeFile_OUTPUTDIR}"
  PRIVATE "${CL_SRC_ROOT}/submodules/yaml-cpp/include"
          "${CL_SRC_ROOT}/ThemeImporters" "${CL_SRC_ROOT}/wxTerminalCtrl")

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
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                          "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    find_package(GTK3)
    set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH}) # Reset, else the official
                                                    # path isn't used again :/
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
  PUBLIC ${LINKER_OPTIONS}
         ${ADDITIONAL_LIBRARIES}
         ${GTK3_LIBRARIES}
         ${LIBYAML_CPP}
         -L"${CL_LIBPATH}"
         lexers_extra
         libcodelite
         lualib
         LuaBridge
         wxterminal_lib
  PRIVATE dtl)
target_include_directories(plugin
                           PUBLIC ${CL_SRC_ROOT}/submodules/wxTerminalEmulator)

if(USE_PCH)
  codelite_add_pch(plugin)
endif()

if(NOT MINGW AND NOT CYGWIN)
  if(APPLE)
    install(TARGETS plugin
            DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
  else()
    install(TARGETS plugin DESTINATION ${PLUGINS_DIR})
  endif()
else()
  install(
    TARGETS plugin
    RUNTIME DESTINATION ${CL_INSTALL_BIN}
    LIBRARY DESTINATION ${CL_INSTALL_BIN}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()

codelite_install_terminal_themes()

install(
  DIRECTORY ${CL_SRC_ROOT}/submodules/agent-sop/agent-sops
  DESTINATION ${CL_RESOURCES_DIR}
  USE_SOURCE_PERMISSIONS
  PATTERN ".svn" EXCLUDE
  PATTERN ".git" EXCLUDE)
