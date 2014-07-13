//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_command_event.h
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

#ifndef CLCOMMANDEVENT_H
#define CLCOMMANDEVENT_H

#include <wx/event.h>
#include <wx/sharedptr.h>
#include "codelite_exports.h"
#include "entry.h"
#include <wx/arrstr.h>
#include <vector>

// Set of flags that can be passed within the 'S{G}etInt' function of clCommandEvent
enum {
    kEventImportingFolder = 0x00000001,
};

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_CL clCommandEvent : public wxCommandEvent
{
protected:
    wxSharedPtr<wxClientData> m_ptr;
    wxArrayString m_strings;
    wxString m_fileName;
    bool m_answer;

public:
    clCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCommandEvent(const clCommandEvent& event);
    clCommandEvent& operator=(const clCommandEvent& src);
    virtual ~clCommandEvent();

    // Hides wxCommandEvent::Set{Get}ClientObject
    void SetClientObject(wxClientData* clientObject);

    wxClientData* GetClientObject() const;

    virtual wxEvent* Clone() const;

    void SetAnswer(bool answer) { this->m_answer = answer; }
    bool IsAnswer() const { return m_answer; }

    void SetFileName(const wxString& fileName) { this->m_fileName = fileName; }
    const wxString& GetFileName() const { return m_fileName; }
    void SetStrings(const wxArrayString& strings) { this->m_strings = strings; }
    const wxArrayString& GetStrings() const { return m_strings; }
    wxArrayString& GetStrings() { return m_strings; }
};

typedef void (wxEvtHandler::*clCommandEventFunction)(clCommandEvent&);
#define clCommandEventHandler(func) wxEVENT_HANDLER_CAST(clCommandEventFunction, func)

/// a clCodeCompletionEvent
class WXDLLIMPEXP_CL clCodeCompletionEvent : public clCommandEvent
{
    TagEntryPtrVector_t m_tags;
    wxObject* m_editor;
    wxString m_word;
    int m_position;
    wxString m_tooltip;
    bool m_insideCommentOrString;
    TagEntryPtr m_tagEntry;

public:
    clCodeCompletionEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCodeCompletionEvent(const clCodeCompletionEvent& event);
    clCodeCompletionEvent& operator=(const clCodeCompletionEvent& src);
    virtual ~clCodeCompletionEvent();
    virtual wxEvent* Clone() const;

    void SetTagEntry(TagEntryPtr tag) { this->m_tagEntry = tag; }

    /**
     * @brief return the tag entry associated with this event.
     * This usually makes sense for event wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED
     */
    TagEntryPtr GetTagEntry() const { return m_tagEntry; }

    void SetInsideCommentOrString(bool insideCommentOrString) { this->m_insideCommentOrString = insideCommentOrString; }

    bool IsInsideCommentOrString() const { return m_insideCommentOrString; }
    void SetTags(const TagEntryPtrVector_t& tags) { this->m_tags = tags; }
    const TagEntryPtrVector_t& GetTags() const { return m_tags; }
    void SetEditor(wxObject* editor) { this->m_editor = editor; }
    /**
     * @brief return the Editor object
     */
    wxObject* GetEditor() { return m_editor; }
    void SetWord(const wxString& word) { this->m_word = word; }

    /**
     * @brief return the user typed word up to the caret position
     */
    const wxString& GetWord() const { return m_word; }
    void SetPosition(int position) { this->m_position = position; }
    /**
     * @brief return the editor position
     */
    int GetPosition() const { return m_position; }
    void SetTooltip(const wxString& tooltip) { this->m_tooltip = tooltip; }
    const wxString& GetTooltip() const { return m_tooltip; }
};

typedef void (wxEvtHandler::*clCodeCompletionEventFunction)(clCodeCompletionEvent&);
#define clCodeCompletionEventHandler(func) wxEVENT_HANDLER_CAST(clCodeCompletionEventFunction, func)

class WXDLLIMPEXP_CL clColourEvent : public clCommandEvent
{
    wxColour m_bgColour;
    wxColour m_fgColour;
    wxColour m_borderColour;
    wxWindow* m_page;
    bool m_isActiveTab;

public:
    clColourEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clColourEvent(const clColourEvent& event);
    clColourEvent& operator=(const clColourEvent& src);
    virtual ~clColourEvent();
    virtual wxEvent* Clone() const { return new clColourEvent(*this); };

    void SetBorderColour(const wxColour& borderColour) { this->m_borderColour = borderColour; }
    const wxColour& GetBorderColour() const { return m_borderColour; }
    void SetPage(wxWindow* page) { this->m_page = page; }
    wxWindow* GetPage() { return m_page; }
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetFgColour(const wxColour& fgColour) { this->m_fgColour = fgColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxColour& GetFgColour() const { return m_fgColour; }
    void SetIsActiveTab(bool isActiveTab) { this->m_isActiveTab = isActiveTab; }
    bool IsActiveTab() const { return m_isActiveTab; }
};

typedef void (wxEvtHandler::*clColourEventFunction)(clColourEvent&);
#define clColourEventHandler(func) wxEVENT_HANDLER_CAST(clColourEventFunction, func)

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
class WXDLLIMPEXP_CL clBuildEvent : public clCommandEvent
{
    wxString m_projectName;
    wxString m_configurationName;
    wxString m_command;
    bool m_projectOnly;

public:
    clBuildEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clBuildEvent(const clBuildEvent& event);
    clBuildEvent& operator=(const clBuildEvent& src);
    virtual ~clBuildEvent();
    virtual wxEvent* Clone() const { return new clBuildEvent(*this); };

    void SetProjectOnly(bool projectOnly) { this->m_projectOnly = projectOnly; }
    bool IsProjectOnly() const { return m_projectOnly; }
    void SetCommand(const wxString& command) { this->m_command = command; }
    const wxString& GetCommand() const { return m_command; }
    void SetConfigurationName(const wxString& configurationName) { this->m_configurationName = configurationName; }
    void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
    const wxString& GetConfigurationName() const { return m_configurationName; }
    const wxString& GetProjectName() const { return m_projectName; }
};

typedef void (wxEvtHandler::*clBuildEventFunction)(clBuildEvent&);
#define clBuildEventHandler(func) wxEVENT_HANDLER_CAST(clBuildEventFunction, func)

// -------------------------------------------------------------------------
// clDebugEvent
// -------------------------------------------------------------------------
class WXDLLIMPEXP_CL clDebugEvent : public clCommandEvent
{
    wxString m_projectName;       // wxEVT_DBG_UI_START_OR_CONT
    wxString m_configurationName; // wxEVT_DBG_UI_START_OR_CONT
    wxString m_debuggerName; // holds the selected debugger name. wxEVT_DBG_UI_START_OR_CONT, wxEVT_DBG_UI_QUICK_DEBUG,
                             // wxEVT_DBG_UI_CORE_FILE
    wxString m_executableName;   // This will be set for wxEVT_DBG_UI_QUICK_DEBUG and wxEVT_DBG_UI_CORE_FILE
    wxString m_coreFile;         // wxEVT_DBG_UI_CORE_FILE
    wxString m_workingDirectory; // wxEVT_DBG_UI_CORE_FILE, wxEVT_DBG_UI_QUICK_DEBUG
    wxString m_arguments;        // wxEVT_DBG_UI_QUICK_DEBUG
    wxString m_startupCommands;  // wxEVT_DBG_UI_QUICK_DEBUG

public:
    clDebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clDebugEvent(const clDebugEvent& event);
    clDebugEvent& operator=(const clDebugEvent& other);

    virtual ~clDebugEvent();
    virtual wxEvent* Clone() const { return new clDebugEvent(*this); };

    void SetDebuggerName(const wxString& debuggerName) { this->m_debuggerName = debuggerName; }
    const wxString& GetDebuggerName() const { return m_debuggerName; }
    void SetArguments(const wxString& arguments) { this->m_arguments = arguments; }
    void SetCoreFile(const wxString& coreFile) { this->m_coreFile = coreFile; }
    void SetExecutableName(const wxString& executableName) { this->m_executableName = executableName; }
    void SetStartupCommands(const wxString& startupCommands) { this->m_startupCommands = startupCommands; }
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetArguments() const { return m_arguments; }
    const wxString& GetCoreFile() const { return m_coreFile; }
    const wxString& GetExecutableName() const { return m_executableName; }
    const wxString& GetStartupCommands() const { return m_startupCommands; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    void SetConfigurationName(const wxString& configurationName) { this->m_configurationName = configurationName; }
    void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
    const wxString& GetConfigurationName() const { return m_configurationName; }
    const wxString& GetProjectName() const { return m_projectName; }
};

typedef void (wxEvtHandler::*clDebugEventFunction)(clDebugEvent&);
#define clDebugEventHandler(func) wxEVENT_HANDLER_CAST(clDebugEventFunction, func)

// ------------------------------------------------------------------
// clNewProjectEvent
// ------------------------------------------------------------------
class WXDLLIMPEXP_CL clNewProjectEvent : public clCommandEvent
{
public:
    struct Template
    {
        wxString m_category;
        wxString m_categoryPng;
        wxString m_template;
        wxString m_templatePng;
        wxString m_toolchain;
        wxString m_debugger;
        typedef std::vector<clNewProjectEvent::Template> Vec_t;
    };

protected:
    clNewProjectEvent::Template::Vec_t m_templates;
    wxString m_toolchain;
    wxString m_debugger;
    wxString m_projectName;
    wxString m_projectFolder;
    wxString m_templateName;

public:
    clNewProjectEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clNewProjectEvent(const clNewProjectEvent& event);
    clNewProjectEvent& operator=(const clNewProjectEvent& src);
    virtual ~clNewProjectEvent();
    virtual wxEvent* Clone() const { return new clNewProjectEvent(*this); };

    void SetTemplates(const clNewProjectEvent::Template::Vec_t& templates) { this->m_templates = templates; }
    const clNewProjectEvent::Template::Vec_t& GetTemplates() const { return m_templates; }
    clNewProjectEvent::Template::Vec_t& GetTemplates() { return m_templates; }

    void SetDebugger(const wxString& debugger) { this->m_debugger = debugger; }
    void SetProjectFolder(const wxString& projectFolder) { this->m_projectFolder = projectFolder; }
    void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
    void SetToolchain(const wxString& toolchain) { this->m_toolchain = toolchain; }
    const wxString& GetDebugger() const { return m_debugger; }
    const wxString& GetProjectFolder() const { return m_projectFolder; }
    const wxString& GetProjectName() const { return m_projectName; }
    const wxString& GetToolchain() const { return m_toolchain; }
    void SetTemplateName(const wxString& templateName) { this->m_templateName = templateName; }
    const wxString& GetTemplateName() const { return m_templateName; }
};

typedef void (wxEvtHandler::*clNewProjectEventFunction)(clNewProjectEvent&);
#define clNewProjectEventHandler(func) wxEVENT_HANDLER_CAST(clNewProjectEventFunction, func)

// --------------------------------------------------------------
// Compiler event
// --------------------------------------------------------------
class WXDLLIMPEXP_CL clCompilerEvent : public clCommandEvent
{
public:
    clCompilerEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCompilerEvent(const clCompilerEvent& event);
    clCompilerEvent& operator=(const clCompilerEvent& src);
    virtual ~clCompilerEvent();
    virtual wxEvent* Clone() const { return new clCompilerEvent(*this); }
};

typedef void (wxEvtHandler::*clCompilerEventFunction)(clCompilerEvent&);
#define clCompilerEventHandler(func) wxEVENT_HANDLER_CAST(clCompilerEventFunction, func)

//---------------------------------------------------------------
// Source formatting event
//---------------------------------------------------------------
class WXDLLIMPEXP_CL clSourceFormatEvent : public clCommandEvent
{
    wxString m_inputString;
    wxString m_formattedString;

public:
    clSourceFormatEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clSourceFormatEvent(const clSourceFormatEvent& event);
    clSourceFormatEvent& operator=(const clSourceFormatEvent& src);
    virtual ~clSourceFormatEvent();
    virtual wxEvent* Clone() const { return new clSourceFormatEvent(*this); }
    void SetFormattedString(const wxString& formattedString) { this->m_formattedString = formattedString; }
    void SetInputString(const wxString& inputString) { this->m_inputString = inputString; }
    const wxString& GetFormattedString() const { return m_formattedString; }
    const wxString& GetInputString() const { return m_inputString; }
};

typedef void (wxEvtHandler::*clSourceFormatEventFunction)(clSourceFormatEvent&);
#define clSourceFormatEventHandler(func) wxEVENT_HANDLER_CAST(clSourceFormatEventFunction, func)

//---------------------------------------------------------------
// Context menu event
//---------------------------------------------------------------
class WXDLLIMPEXP_CL clContextMenuEvent : public clCommandEvent
{
    wxMenu* m_menu;
    wxObject* m_editor;

public:
    clContextMenuEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clContextMenuEvent(const clContextMenuEvent& event);
    clContextMenuEvent& operator=(const clContextMenuEvent& src);
    virtual ~clContextMenuEvent();
    virtual wxEvent* Clone() const { return new clContextMenuEvent(*this); }
    void SetMenu(wxMenu* menu) { this->m_menu = menu; }
    wxMenu* GetMenu() { return m_menu; }
    void SetEditor(wxObject* editor) { this->m_editor = editor; }
    wxObject* GetEditor() { return m_editor; }
};

typedef void (wxEvtHandler::*clContextMenuEventFunction)(clContextMenuEvent&);
#define clContextMenuEventHandler(func) wxEVENT_HANDLER_CAST(clContextMenuEventFunction, func)

#endif // CLCOMMANDEVENT_H
