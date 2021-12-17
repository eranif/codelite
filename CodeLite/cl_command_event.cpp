//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_command_event.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "cl_command_event.h"

clCommandEvent::clCommandEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_answer(false)
    , m_allowed(true)
    , m_lineNumber(0)
    , m_selected(false)
{
}

clCommandEvent::clCommandEvent(const clCommandEvent& event)
    : wxCommandEvent(event)
    , m_answer(false)
    , m_allowed(true)
{
    *this = event;
}

clCommandEvent& clCommandEvent::operator=(const clCommandEvent& src)
{
    m_strings.clear();
    m_ptr = src.m_ptr;
    for(size_t i = 0; i < src.m_strings.size(); ++i) {
        m_strings.Add(src.m_strings.Item(i).c_str());
    }
    m_fileName = src.m_fileName;
    m_answer = src.m_answer;
    m_allowed = src.m_allowed;
    m_oldName = src.m_oldName;
    m_lineNumber = src.m_lineNumber;
    m_selected = src.m_selected;

    // Copy wxCommandEvent members here
    m_eventType = src.m_eventType;
    m_id = src.m_id;
    m_cmdString = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong = src.m_extraLong;
    return *this;
}

clCommandEvent::~clCommandEvent() { m_ptr.reset(); }

wxEvent* clCommandEvent::Clone() const
{
    clCommandEvent* new_event = new clCommandEvent(*this);
    return new_event;
}

void clCommandEvent::SetClientObject(wxClientData* clientObject) { m_ptr = clientObject; }

wxClientData* clCommandEvent::GetClientObject() const { return m_ptr.get(); }

// ---------------------------------
// Code Completion event
// ---------------------------------
wxEvent* clCodeCompletionEvent::Clone() const
{
    clCodeCompletionEvent* new_event = new clCodeCompletionEvent(*this);
    return new_event;
}

clCodeCompletionEvent::clCodeCompletionEvent(const clCodeCompletionEvent& event)
    : clCommandEvent(event)
    , m_insideCommentOrString(false)
{
    *this = event;
    m_position = wxNOT_FOUND;
    m_entry.reset(NULL);
}

clCodeCompletionEvent::clCodeCompletionEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_insideCommentOrString(false)
{
    m_position = wxNOT_FOUND;
    m_entry.reset(NULL);
}

clCodeCompletionEvent::~clCodeCompletionEvent() {}

clCodeCompletionEvent& clCodeCompletionEvent::operator=(const clCodeCompletionEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    // Implement our copy c tor
    m_word = src.m_word;
    m_position = src.m_position;
    m_tooltip = src.m_tooltip;
    m_insideCommentOrString = src.m_insideCommentOrString;
    m_entry = src.m_entry;
    m_definitions = src.m_definitions;
    m_entries = src.m_entries;
    m_triggerKind = src.m_triggerKind;
    m_classes = src.m_classes;
    m_variables = src.m_variables;
    return *this;
}

// ------------------------------------------------
// clColourEvent
// ------------------------------------------------

clColourEvent::clColourEvent(const clColourEvent& event) { *this = event; }

clColourEvent::clColourEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
#if wxUSE_GUI
    , m_page(NULL)
#endif
    , m_isActiveTab(false)
{
}

clColourEvent::~clColourEvent() {}

clColourEvent& clColourEvent::operator=(const clColourEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
#if wxUSE_GUI
    m_bgColour = src.m_bgColour;
    m_fgColour = src.m_fgColour;
    m_page = src.m_page;
    m_borderColour = src.m_borderColour;
#endif
    m_isActiveTab = src.m_isActiveTab;
    return *this;
}

// ------------------------------------------------
// clBuildEvent
// ------------------------------------------------

clBuildEvent::clBuildEvent(const clBuildEvent& event) { *this = event; }

clBuildEvent::clBuildEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_projectOnly(false)
    , m_warningCount(0)
    , m_errorCount(0)
    , m_isRunning(false)
{
}

clBuildEvent::~clBuildEvent() {}

clBuildEvent& clBuildEvent::operator=(const clBuildEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    m_projectName = src.m_projectName;
    m_configurationName = src.m_configurationName;
    m_command = src.m_command;
    m_projectOnly = src.m_projectOnly;
    m_errorCount = src.m_errorCount;
    m_warningCount = src.m_warningCount;
    m_kind = src.m_kind;
    m_isRunning = src.m_isRunning;
    m_cleanLog = src.m_cleanLog;
    m_flags = src.m_flags;
    m_toolchain = src.m_toolchain;
    return *this;
}

// ------------------------------------------------------------------
// clDebugEvent
// ------------------------------------------------------------------
clDebugEvent::clDebugEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_features(kAllFeatures)
    , m_memoryBlockSize(32)
{
}

clDebugEvent::~clDebugEvent() {}

clDebugEvent::clDebugEvent(const clDebugEvent& event) { *this = event; }

clDebugEvent& clDebugEvent::operator=(const clDebugEvent& other)
{
    clCommandEvent::operator=(other);
    m_features = other.m_features;
    m_debuggerName = other.m_debuggerName;
    m_projectName = other.m_projectName;
    m_configurationName = other.m_configurationName;
    m_executableName =
        other.m_executableName;    // This will be set for wxEVT_DBG_UI_QUICK_DEBUG and wxEVT_DBG_UI_CORE_FILE
    m_coreFile = other.m_coreFile; // wxEVT_DBG_UI_CORE_FILE
    m_workingDirectory = other.m_workingDirectory; // wxEVT_DBG_UI_CORE_FILE, wxEVT_DBG_UI_QUICK_DEBUG
    m_arguments = other.m_arguments;               // wxEVT_DBG_UI_QUICK_DEBUG
    m_startupCommands = other.m_startupCommands;   // wxEVT_DBG_UI_QUICK_DEBUG
    m_memoryBlockSize = other.m_memoryBlockSize;
    m_memoryAddress = other.m_memoryAddress;
    m_memoryBlockValue = other.m_memoryBlockValue;
    m_breakpoints = other.m_breakpoints;
    m_isSSHDebugging = other.m_isSSHDebugging;
    m_sshAccount = other.m_sshAccount;
    m_alternateDebuggerPath = other.m_alternateDebuggerPath;
    return *this;
}

//-------------------------------------------------------------------------
// clNewProjectEvent
//-------------------------------------------------------------------------

clNewProjectEvent::clNewProjectEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clNewProjectEvent::clNewProjectEvent(const clNewProjectEvent& event) { *this = event; }

clNewProjectEvent::~clNewProjectEvent() {}

clNewProjectEvent& clNewProjectEvent::operator=(const clNewProjectEvent& other)
{
    clCommandEvent::operator=(other);
    m_templates = other.m_templates;
    m_toolchain = other.m_toolchain;
    m_debugger = other.m_debugger;
    m_projectName = other.m_projectName;
    m_projectFolder = other.m_projectFolder;
    m_templateName = other.m_templateName;
    return *this;
}

// --------------------------------------------------------------
// Compiler event
// --------------------------------------------------------------
clCompilerEvent::~clCompilerEvent() {}

clCompilerEvent::clCompilerEvent(const clCompilerEvent& event) { *this = event; }

clCompilerEvent::clCompilerEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clCompilerEvent& clCompilerEvent::operator=(const clCompilerEvent& src)
{
    clCommandEvent::operator=(src);
    return *this;
}

// --------------------------------------------------------------
// Source formatting event
// --------------------------------------------------------------

clSourceFormatEvent::clSourceFormatEvent(const clSourceFormatEvent& event) { *this = event; }

clSourceFormatEvent::clSourceFormatEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clSourceFormatEvent::~clSourceFormatEvent() {}

clSourceFormatEvent& clSourceFormatEvent::operator=(const clSourceFormatEvent& src)
{
    clCommandEvent::operator=(src);
    m_formattedString = src.m_formattedString;
    m_inputString = src.m_inputString;
    return *this;
}

//-------------------------------------------------------------------------
// clContextMenuEvent
//-------------------------------------------------------------------------

clContextMenuEvent::clContextMenuEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
#if wxUSE_GUI
    , m_menu(NULL)
#endif
    , m_editor(NULL)
{
}

clContextMenuEvent::clContextMenuEvent(const clContextMenuEvent& event) { *this = event; }
clContextMenuEvent& clContextMenuEvent::operator=(const clContextMenuEvent& src)
{
    clCommandEvent::operator=(src);
    m_editor = src.m_editor;
#if wxUSE_GUI
    m_menu = src.m_menu;
#endif
    m_path = src.m_path;
    return *this;
}

clContextMenuEvent::~clContextMenuEvent() {}

//-------------------------------------------------------------------------
// clExecuteEvent
//-------------------------------------------------------------------------

clExecuteEvent::clExecuteEvent(const clExecuteEvent& event) { *this = event; }
clExecuteEvent::~clExecuteEvent() {}
clExecuteEvent& clExecuteEvent::operator=(const clExecuteEvent& src)
{
    clCommandEvent::operator=(src);
    m_targetName = src.m_targetName;
    return *this;
}

clExecuteEvent::clExecuteEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//------------------------------------------------------------------------
// clProjectSettingsEvent
//------------------------------------------------------------------------
clProjectSettingsEvent& clProjectSettingsEvent::operator=(const clProjectSettingsEvent& src)
{
    clCommandEvent::operator=(src);
    m_configName = src.m_configName;
    m_projectName = src.m_projectName;
    return *this;
}

clProjectSettingsEvent::clProjectSettingsEvent(const clProjectSettingsEvent& event) { *this = event; }

clProjectSettingsEvent::clProjectSettingsEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clProjectSettingsEvent::~clProjectSettingsEvent() {}

//------------------------------------------------------------------------
// clFindEvent
//------------------------------------------------------------------------
clFindEvent& clFindEvent::operator=(const clFindEvent& src)
{
    clCommandEvent::operator=(src);
    m_ctrl = src.m_ctrl;
    return *this;
}

clFindEvent::clFindEvent(const clFindEvent& event) { *this = event; }

clFindEvent::clFindEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_ctrl(NULL)
{
}

clFindEvent::~clFindEvent() {}

//------------------------------------------------------------------------
// clFindInFilesEvent
//------------------------------------------------------------------------
clFindInFilesEvent& clFindInFilesEvent::operator=(const clFindInFilesEvent& src)
{
    clCommandEvent::operator=(src);
    m_paths = src.m_paths;
    m_fileMask = src.m_fileMask;
    m_options = src.m_options;
    m_transientPaths = src.m_transientPaths;
    m_matches = src.m_matches;
    return *this;
}

clFindInFilesEvent::clFindInFilesEvent(const clFindInFilesEvent& event) { *this = event; }

clFindInFilesEvent::clFindInFilesEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clFindInFilesEvent::~clFindInFilesEvent() {}

//------------------------------------------------------------------------
// clParseEvent
//------------------------------------------------------------------------

clParseEvent::clParseEvent(const clParseEvent& event) { *this = event; }

clParseEvent::clParseEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_curfileIndex(0)
    , m_totalFiles(0)
{
}

clParseEvent::~clParseEvent() {}

clParseEvent& clParseEvent::operator=(const clParseEvent& src)
{
    clCommandEvent::operator=(src);
    m_curfileIndex = src.m_curfileIndex;
    m_totalFiles = src.m_totalFiles;
    return *this;
}

//-------------------------------------------------------------------
// clProcessEvent
//-------------------------------------------------------------------

clProcessEvent::clProcessEvent(const clProcessEvent& event) { *this = event; }

clProcessEvent::clProcessEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_process(NULL)
{
}

clProcessEvent::~clProcessEvent() {}

clProcessEvent& clProcessEvent::operator=(const clProcessEvent& src)
{
    clCommandEvent::operator=(src);
    m_process = src.m_process;
    m_output = src.m_output;
    return *this;
}

// --------------------------------------------------------------
// Compiler event
// --------------------------------------------------------------
clEditorConfigEvent::~clEditorConfigEvent() {}

clEditorConfigEvent::clEditorConfigEvent(const clEditorConfigEvent& event) { *this = event; }

clEditorConfigEvent::clEditorConfigEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clEditorConfigEvent& clEditorConfigEvent::operator=(const clEditorConfigEvent& src)
{
    clCommandEvent::operator=(src);
    m_editorConfigSection = src.m_editorConfigSection;
    return *this;
}

// --------------------------------------------------------------
// Goto Event
// --------------------------------------------------------------
clGotoEvent::clGotoEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clGotoEvent::clGotoEvent(const clGotoEvent& src) { *this = src; }

clGotoEvent::~clGotoEvent() {}

clGotoEvent& clGotoEvent::operator=(const clGotoEvent& src)
{
    if(this == &src) {
        return *this;
    }
    clCommandEvent::operator=(src);
    m_entries = src.m_entries;
    m_entry = src.m_entry;
    return *this;
}

// --------------------------------------------------------------
// Source control event
// --------------------------------------------------------------
clSourceControlEvent::clSourceControlEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clSourceControlEvent::clSourceControlEvent(const clSourceControlEvent& event) { *this = event; }

clSourceControlEvent::~clSourceControlEvent() {}

wxEvent* clSourceControlEvent::Clone() const { return new clSourceControlEvent(*this); }

clSourceControlEvent& clSourceControlEvent::operator=(const clSourceControlEvent& src)
{
    if(this == &src) {
        return *this;
    }
    clCommandEvent::operator=(src);
    m_sourceControlName = src.m_sourceControlName;
    return *this;
}

///----------------------------------------------------------------------------------
/// clLanguageServerEvent
///----------------------------------------------------------------------------------

clLanguageServerEvent::clLanguageServerEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clLanguageServerEvent::clLanguageServerEvent(const clLanguageServerEvent& event) {}

clLanguageServerEvent::~clLanguageServerEvent() {}
clLanguageServerEvent& clLanguageServerEvent::operator=(const clLanguageServerEvent& src)
{
    if(this == &src) {
        return *this;
    }
    clCommandEvent::operator=(src);
    m_lspName = src.m_lspName;
    m_lspCommand = src.m_lspCommand;
    m_flags = src.m_flags;
    m_sshAccount = src.m_sshAccount;
    m_priority = src.m_priority;
    m_connectionString = src.m_connectionString;
    m_enviroment = src.m_enviroment;
    m_initOptions = src.m_initOptions;
    m_languages = src.m_languages;
    m_action = src.m_action;
    m_rootUri = src.m_rootUri;
    return *this;
}

wxEvent* clLanguageServerEvent::Clone() const { return new clLanguageServerEvent(*this); }
