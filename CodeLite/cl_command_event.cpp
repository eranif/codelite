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
    m_strings.insert(m_strings.end(), src.m_strings.begin(), src.m_strings.end());
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
    , m_editor(NULL)
    , m_insideCommentOrString(false)
{
    *this = event;
    m_position = wxNOT_FOUND;
    m_entry.reset(NULL);
}

clCodeCompletionEvent::clCodeCompletionEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_editor(NULL)
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
    m_tags = src.m_tags;
    m_editor = src.m_editor;
    m_word = src.m_word;
    m_position = src.m_position;
    m_tooltip = src.m_tooltip;
    m_insideCommentOrString = src.m_insideCommentOrString;
    m_entry = src.m_entry;
    m_definitions = src.m_definitions;
    return *this;
}

// ------------------------------------------------
// clColourEvent
// ------------------------------------------------

clColourEvent::clColourEvent(const clColourEvent& event) { *this = event; }

clColourEvent::clColourEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_page(NULL)
    , m_isActiveTab(false)
{
}

clColourEvent::~clColourEvent() {}

clColourEvent& clColourEvent::operator=(const clColourEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    m_bgColour = src.m_bgColour;
    m_fgColour = src.m_fgColour;
    m_page = src.m_page;
    m_isActiveTab = src.m_isActiveTab;
    m_borderColour = src.m_borderColour;
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
    return *this;
}

// ------------------------------------------------------------------
// clDebugEvent
// ------------------------------------------------------------------
clDebugEvent::clDebugEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clDebugEvent::~clDebugEvent() {}

clDebugEvent::clDebugEvent(const clDebugEvent& event) { *this = event; }

clDebugEvent& clDebugEvent::operator=(const clDebugEvent& other)
{
    clCommandEvent::operator=(other);
    m_debuggerName = other.m_debuggerName;
    m_projectName = other.m_projectName;
    m_configurationName = other.m_configurationName;
    m_executableName =
        other.m_executableName;    // This will be set for wxEVT_DBG_UI_QUICK_DEBUG and wxEVT_DBG_UI_CORE_FILE
    m_coreFile = other.m_coreFile; // wxEVT_DBG_UI_CORE_FILE
    m_workingDirectory = other.m_workingDirectory; // wxEVT_DBG_UI_CORE_FILE, wxEVT_DBG_UI_QUICK_DEBUG
    m_arguments = other.m_arguments;               // wxEVT_DBG_UI_QUICK_DEBUG
    m_startupCommands = other.m_startupCommands;   // wxEVT_DBG_UI_QUICK_DEBUG
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
    , m_editor(NULL)
{
}
clContextMenuEvent::clContextMenuEvent(const clContextMenuEvent& event) { *this = event; }
clContextMenuEvent& clContextMenuEvent::operator=(const clContextMenuEvent& src)
{
    clCommandEvent::operator=(src);
    m_editor = src.m_editor;
    m_menu = src.m_menu;
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
