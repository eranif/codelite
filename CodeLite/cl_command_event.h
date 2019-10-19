//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "clEditorConfig.h"
#include "clGotoEntry.h"
#include "codelite_exports.h"
#include "entry.h"
#include "wxCodeCompletionBoxEntry.hpp"
#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include "LSP/CompletionItem.h"

// Set of flags that can be passed within the 'S{G}etInt' function of clCommandEvent
enum {
    kEventImportingFolder = 0x00000001,
};

#if !wxUSE_GUI
// To allow building libcodelite without UI we need these 2 event types (wxCommandEvent and wxNotifyEvent)
class WXDLLIMPEXP_CL wxCommandEvent : public wxEvent, public wxEventBasicPayloadMixin
{
public:
    wxCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxEvent(winid, commandType)
    {
        m_clientData = NULL;
        m_clientObject = NULL;
        m_isCommandEvent = true;

        // the command events are propagated upwards by default
        m_propagationLevel = wxEVENT_PROPAGATE_MAX;
    }

    wxCommandEvent(const wxCommandEvent& event)
        : wxEvent(event)
        , wxEventBasicPayloadMixin(event)
        , m_clientData(event.m_clientData)
        , m_clientObject(event.m_clientObject)
    {
        // Because GetString() can retrieve the string text only on demand, we
        // need to copy it explicitly.
        if(m_cmdString.empty()) m_cmdString = event.GetString();
    }

    virtual ~wxCommandEvent() {}

    // Set/Get client data from controls
    void SetClientData(void* clientData) { m_clientData = clientData; }
    void* GetClientData() const { return m_clientData; }

    // Set/Get client object from controls
    void SetClientObject(wxClientData* clientObject) { m_clientObject = clientObject; }
    wxClientData* GetClientObject() const { return m_clientObject; }

    // Note: this shadows wxEventBasicPayloadMixin::GetString(), because it does some
    // GUI-specific hacks
    wxString GetString() const { return m_cmdString; }

    // Get listbox selection if single-choice
    int GetSelection() const { return m_commandInt; }

    // Get checkbox value
    bool IsChecked() const { return m_commandInt != 0; }

    // true if the listbox event was a selection.
    bool IsSelection() const { return (m_extraLong != 0); }

    virtual wxEvent* Clone() const wxOVERRIDE { return new wxCommandEvent(*this); }
    virtual wxEventCategory GetEventCategory() const wxOVERRIDE { return wxEVT_CATEGORY_USER_INPUT; }

protected:
    void* m_clientData;           // Arbitrary client data
    wxClientData* m_clientObject; // Arbitrary client object
};

// this class adds a possibility to react (from the user) code to a control
// notification: allow or veto the operation being reported.
class WXDLLIMPEXP_CL wxNotifyEvent : public wxCommandEvent
{
public:
    wxNotifyEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxCommandEvent(commandType, winid)
    {
        m_bAllow = true;
    }

    wxNotifyEvent(const wxNotifyEvent& event)
        : wxCommandEvent(event)
    {
        m_bAllow = event.m_bAllow;
    }

    // veto the operation (usually it's allowed by default)
    void Veto() { m_bAllow = false; }

    // allow the operation if it was disabled by default
    void Allow() { m_bAllow = true; }

    // for implementation code only: is the operation allowed?
    bool IsAllowed() const { return m_bAllow; }

    virtual wxEvent* Clone() const wxOVERRIDE { return new wxNotifyEvent(*this); }

private:
    bool m_bAllow;
};

#endif // !wxUSE_GUI

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_CL clCommandEvent : public wxCommandEvent
{
protected:
    wxSharedPtr<wxClientData> m_ptr;
    wxArrayString m_strings;
    wxString m_fileName;
    wxString m_oldName;
    bool m_answer;
    bool m_allowed;
    int m_lineNumber;
    bool m_selected;

public:
    clCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCommandEvent(const clCommandEvent& event);
    clCommandEvent& operator=(const clCommandEvent& src);
    virtual ~clCommandEvent();

    clCommandEvent& SetSelected(bool selected)
    {
        this->m_selected = selected;
        return *this;
    }
    bool IsSelected() const { return m_selected; }

    // Veto management
    void Veto() { this->m_allowed = false; }
    void Allow() { this->m_allowed = true; }

    // Hides wxCommandEvent::Set{Get}ClientObject
    void SetClientObject(wxClientData* clientObject);

    wxClientData* GetClientObject() const;
    virtual wxEvent* Clone() const;

    clCommandEvent& SetLineNumber(int lineNumber)
    {
        this->m_lineNumber = lineNumber;
        return *this;
    }
    int GetLineNumber() const { return m_lineNumber; }
    clCommandEvent& SetAllowed(bool allowed)
    {
        this->m_allowed = allowed;
        return *this;
    }
    clCommandEvent& SetAnswer(bool answer)
    {
        this->m_answer = answer;
        return *this;
    }
    clCommandEvent& SetFileName(const wxString& fileName)
    {
        this->m_fileName = fileName;
        return *this;
    }
    clCommandEvent& SetOldName(const wxString& oldName)
    {
        this->m_oldName = oldName;
        return *this;
    }
    clCommandEvent& SetPtr(const wxSharedPtr<wxClientData>& ptr)
    {
        this->m_ptr = ptr;
        return *this;
    }
    clCommandEvent& SetStrings(const wxArrayString& strings)
    {
        this->m_strings = strings;
        return *this;
    }
    bool IsAllowed() const { return m_allowed; }
    bool IsAnswer() const { return m_answer; }
    const wxString& GetFileName() const { return m_fileName; }
    const wxString& GetOldName() const { return m_oldName; }
    const wxSharedPtr<wxClientData>& GetPtr() const { return m_ptr; }
    const wxArrayString& GetStrings() const { return m_strings; }
    wxArrayString& GetStrings() { return m_strings; }
};

typedef void (wxEvtHandler::*clCommandEventFunction)(clCommandEvent&);
#define clCommandEventHandler(func) wxEVENT_HANDLER_CAST(clCommandEventFunction, func)

/// Source control event
class WXDLLIMPEXP_CL clSourceControlEvent : public clCommandEvent
{
protected:
    wxString m_sourceControlName;

public:
    clSourceControlEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clSourceControlEvent(const clSourceControlEvent& event);
    clSourceControlEvent& operator=(const clSourceControlEvent& src);
    virtual ~clSourceControlEvent();
    virtual wxEvent* Clone() const;
    void SetSourceControlName(const wxString& sourceControlName) { this->m_sourceControlName = sourceControlName; }
    const wxString& GetSourceControlName() const { return m_sourceControlName; }
};

typedef void (wxEvtHandler::*clSourceControlEventFunction)(clSourceControlEvent&);
#define clSourceControlEventHandler(func) wxEVENT_HANDLER_CAST(clSourceControlEventFunction, func)

/// a clCodeCompletionEvent
class WXDLLIMPEXP_CL clCodeCompletionEvent : public clCommandEvent
{
    wxObject* m_editor;
    wxString m_word;
    int m_position;
    wxString m_tooltip;
    bool m_insideCommentOrString;
    wxCodeCompletionBoxEntry::Ptr_t m_entry;
    wxArrayString m_definitions;
    wxCodeCompletionBoxEntry::Vec_t m_entries;
    LSP::CompletionItem::eTriggerKind m_triggerKind = LSP::CompletionItem::kTriggerUnknown;

public:
    clCodeCompletionEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCodeCompletionEvent(const clCodeCompletionEvent& event);
    clCodeCompletionEvent& operator=(const clCodeCompletionEvent& src);
    virtual ~clCodeCompletionEvent();
    virtual wxEvent* Clone() const;

    void SetDefinitions(const wxArrayString& definitions) { this->m_definitions = definitions; }
    const wxArrayString& GetDefinitions() const { return m_definitions; }
    void SetEntry(wxCodeCompletionBoxEntry::Ptr_t entry) { this->m_entry = entry; }
    wxCodeCompletionBoxEntry::Ptr_t GetEntry() { return m_entry; }

    void SetInsideCommentOrString(bool insideCommentOrString) { this->m_insideCommentOrString = insideCommentOrString; }

    bool IsInsideCommentOrString() const { return m_insideCommentOrString; }
    void SetEditor(wxObject* editor) { this->m_editor = editor; }
    void SetEntries(const wxCodeCompletionBoxEntry::Vec_t& entries) { this->m_entries = entries; }
    const wxCodeCompletionBoxEntry::Vec_t& GetEntries() const { return m_entries; }
    wxCodeCompletionBoxEntry::Vec_t& GetEntries() { return m_entries; }

    const LSP::CompletionItem::eTriggerKind& GetTriggerKind() const { return m_triggerKind; }
    void SetTriggerKind(const LSP::CompletionItem::eTriggerKind& triggerKind) { this->m_triggerKind = triggerKind; }

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
#if wxUSE_GUI
    wxColour m_bgColour;
    wxColour m_fgColour;
    wxColour m_borderColour;
    wxWindow* m_page;
#endif
    bool m_isActiveTab;

public:
    clColourEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clColourEvent(const clColourEvent& event);
    clColourEvent& operator=(const clColourEvent& src);
    virtual ~clColourEvent();
    virtual wxEvent* Clone() const { return new clColourEvent(*this); };

#if wxUSE_GUI
    void SetBorderColour(const wxColour& borderColour) { this->m_borderColour = borderColour; }
    const wxColour& GetBorderColour() const { return m_borderColour; }
    void SetPage(wxWindow* page) { this->m_page = page; }
    wxWindow* GetPage() { return m_page; }
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetFgColour(const wxColour& fgColour) { this->m_fgColour = fgColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxColour& GetFgColour() const { return m_fgColour; }
#endif
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
    size_t m_warningCount;
    size_t m_errorCount;
    wxString m_kind;
    bool m_isRunning;

public:
    clBuildEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clBuildEvent(const clBuildEvent& event);
    clBuildEvent& operator=(const clBuildEvent& src);
    virtual ~clBuildEvent();
    virtual wxEvent* Clone() const { return new clBuildEvent(*this); };

    void SetIsRunning(bool isRunning) { this->m_isRunning = isRunning; }
    bool IsRunning() const { return m_isRunning; }

    void SetKind(const wxString& kind) { this->m_kind = kind; }
    const wxString& GetKind() const { return this->m_kind; }

    void SetProjectOnly(bool projectOnly) { this->m_projectOnly = projectOnly; }
    bool IsProjectOnly() const { return m_projectOnly; }
    void SetCommand(const wxString& command) { this->m_command = command; }
    const wxString& GetCommand() const { return m_command; }
    void SetConfigurationName(const wxString& configurationName) { this->m_configurationName = configurationName; }
    void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
    const wxString& GetConfigurationName() const { return m_configurationName; }
    const wxString& GetProjectName() const { return m_projectName; }

    void SetErrorCount(size_t errorCount) { this->m_errorCount = errorCount; }
    void SetWarningCount(size_t warningCount) { this->m_warningCount = warningCount; }
    size_t GetErrorCount() const { return m_errorCount; }
    size_t GetWarningCount() const { return m_warningCount; }
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
    size_t m_features;
    wxString m_memoryAddress;    // wxEVT_DEBUGGER_SET_MEMORY
    wxString m_memoryBlockValue; // wxEVT_DEBUGGER_SET_MEMORY
    size_t m_memoryBlockSize;    // wxEVT_DEBUGGER_SET_MEMORY

public:
    // Special features not available by all the debuggers
    enum eFeatures {
        kStepInst = (1 << 0),         // single step instruction
        kInterrupt = (1 << 1),        // interrup the running process
        kShowCursor = (1 << 2),       // show the current active line
        kJumpToCursor = (1 << 3),     // Jump to the caret line, without executing the code in between
        kRunToCursor = (1 << 4),      // execute all the code until reaching the caret
        kReverseDebugging = (1 << 5), // execute all the code until reaching the caret
        kAllFeatures = kStepInst | kInterrupt | kShowCursor | kJumpToCursor | kRunToCursor | kReverseDebugging,
    };

public:
    clDebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clDebugEvent(const clDebugEvent& event);
    clDebugEvent& operator=(const clDebugEvent& other);

    virtual ~clDebugEvent();
    virtual wxEvent* Clone() const { return new clDebugEvent(*this); };

    void SetFeatures(size_t features) { m_features = features; }
    size_t GetFeatures() const { return m_features; }
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
    void SetMemoryAddress(const wxString& memoryAddress) { this->m_memoryAddress = memoryAddress; }
    void SetMemoryBlockSize(size_t memoryBlockSize) { this->m_memoryBlockSize = memoryBlockSize; }
    void SetMemoryBlockValue(const wxString& memoryBlockValue) { this->m_memoryBlockValue = memoryBlockValue; }
    const wxString& GetMemoryAddress() const { return m_memoryAddress; }
    size_t GetMemoryBlockSize() const { return m_memoryBlockSize; }
    const wxString& GetMemoryBlockValue() const { return m_memoryBlockValue; }
};

typedef void (wxEvtHandler::*clDebugEventFunction)(clDebugEvent&);
#define clDebugEventHandler(func) wxEVENT_HANDLER_CAST(clDebugEventFunction, func)

// ------------------------------------------------------------------
// clNewProjectEvent
// ------------------------------------------------------------------
class WXDLLIMPEXP_CL clNewProjectEvent : public clCommandEvent
{
public:
    struct Template {
        wxString m_category;
        wxString m_categoryPng;
        wxString m_template;
        wxString m_templatePng;
        wxString m_toolchain;
        wxString m_debugger;
        bool m_allowSeparateFolder;
        Template()
            : m_allowSeparateFolder(true)
        {
        }
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

// --------------------------------------------------------------
// GotoAnything event
// --------------------------------------------------------------
class WXDLLIMPEXP_CL clGotoEvent : public clCommandEvent
{
    clGotoEntry::Vec_t m_entries;
    clGotoEntry m_entry;

public:
    clGotoEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clGotoEvent(const clGotoEvent& src);
    clGotoEvent& operator=(const clGotoEvent& src);
    virtual ~clGotoEvent();
    virtual wxEvent* Clone() const { return new clGotoEvent(*this); }
    const clGotoEntry::Vec_t& GetEntries() const { return m_entries; }
    clGotoEntry::Vec_t& GetEntries() { return m_entries; }
    void SetEntries(const clGotoEntry::Vec_t& entries) { m_entries = entries; }
    void SetEntry(const clGotoEntry& entry) { this->m_entry = entry; }
    const clGotoEntry& GetEntry() const { return m_entry; }
};

typedef void (wxEvtHandler::*clGotoEventFunction)(clGotoEvent&);
#define clGotoEventHandler(func) wxEVENT_HANDLER_CAST(clGotoEventFunction, func)

// --------------------------------------------------------------
// Processs event
// --------------------------------------------------------------
class IProcess;
class WXDLLIMPEXP_CL clProcessEvent : public clCommandEvent
{
    wxString m_output;
    IProcess* m_process;

public:
    clProcessEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clProcessEvent(const clProcessEvent& event);
    clProcessEvent& operator=(const clProcessEvent& src);
    virtual ~clProcessEvent();
    virtual wxEvent* Clone() const { return new clProcessEvent(*this); }

    void SetOutput(const wxString& output) { this->m_output = output; }
    void SetProcess(IProcess* process) { this->m_process = process; }
    const wxString& GetOutput() const { return m_output; }
    IProcess* GetProcess() { return m_process; }
};

typedef void (wxEvtHandler::*clProcessEventFunction)(clProcessEvent&);
#define clProcessEventHandler(func) wxEVENT_HANDLER_CAST(clProcessEventFunction, func)

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
#if wxUSE_GUI
    wxMenu* m_menu;
#endif
    wxObject* m_editor;
    wxString m_path;

public:
    clContextMenuEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clContextMenuEvent(const clContextMenuEvent& event);
    clContextMenuEvent& operator=(const clContextMenuEvent& src);
    virtual ~clContextMenuEvent();
    virtual wxEvent* Clone() const { return new clContextMenuEvent(*this); }
#if wxUSE_GUI
    void SetMenu(wxMenu* menu) { this->m_menu = menu; }
    wxMenu* GetMenu() { return m_menu; }
#endif
    void SetEditor(wxObject* editor) { this->m_editor = editor; }
    wxObject* GetEditor() { return m_editor; }
    const wxString& GetPath() const { return m_path; }
    void SetPath(const wxString& path) { m_path = path; }
};

typedef void (wxEvtHandler::*clContextMenuEventFunction)(clContextMenuEvent&);
#define clContextMenuEventHandler(func) wxEVENT_HANDLER_CAST(clContextMenuEventFunction, func)

//---------------------------------------------------------------
// Execute event
//---------------------------------------------------------------
class WXDLLIMPEXP_CL clExecuteEvent : public clCommandEvent
{
    wxString m_targetName;

public:
    clExecuteEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clExecuteEvent(const clExecuteEvent& event);
    clExecuteEvent& operator=(const clExecuteEvent& src);
    virtual ~clExecuteEvent();
    virtual wxEvent* Clone() const { return new clExecuteEvent(*this); }

    void SetTargetName(const wxString& targetName) { this->m_targetName = targetName; }
    const wxString& GetTargetName() const { return m_targetName; }
};

typedef void (wxEvtHandler::*clExecuteEventFunction)(clExecuteEvent&);
#define clExecuteEventHandler(func) wxEVENT_HANDLER_CAST(clExecuteEventFunction, func)

//---------------------------------------------------------------
// Execute event
//---------------------------------------------------------------
class WXDLLIMPEXP_CL clProjectSettingsEvent : public clCommandEvent
{
    wxString m_configName;
    wxString m_projectName;

public:
    clProjectSettingsEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clProjectSettingsEvent(const clProjectSettingsEvent& event);
    clProjectSettingsEvent& operator=(const clProjectSettingsEvent& src);
    virtual ~clProjectSettingsEvent();
    virtual wxEvent* Clone() const { return new clProjectSettingsEvent(*this); }
    void SetConfigName(const wxString& configName) { this->m_configName = configName; }
    void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
    const wxString& GetConfigName() const { return m_configName; }
    const wxString& GetProjectName() const { return m_projectName; }
};

typedef void (wxEvtHandler::*clProjectSettingsEventFunction)(clProjectSettingsEvent&);
#define clProjectSettingsEventHandler(func) wxEVENT_HANDLER_CAST(clProjectSettingsEventFunction, func)

//---------------------------------------------------------------
// Find event
//---------------------------------------------------------------
class WXDLLIMPEXP_CL clFindEvent : public clCommandEvent
{
    wxStyledTextCtrl* m_ctrl;

public:
    clFindEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clFindEvent(const clFindEvent& event);
    clFindEvent& operator=(const clFindEvent& src);
    virtual ~clFindEvent();
    virtual wxEvent* Clone() const { return new clFindEvent(*this); }

    void SetCtrl(wxStyledTextCtrl* ctrl) { this->m_ctrl = ctrl; }
    wxStyledTextCtrl* GetCtrl() { return m_ctrl; }
};

typedef void (wxEvtHandler::*clFindEventFunction)(clFindEvent&);
#define clFindEventHandler(func) wxEVENT_HANDLER_CAST(clFindEventFunction, func)

//---------------------------------------------------------------
// Find in files event
//---------------------------------------------------------------
class WXDLLIMPEXP_CL clFindInFilesEvent : public clCommandEvent
{
    wxString m_paths;
    wxString m_fileMask;
    size_t m_options = 0;
    wxString m_transientPaths;

public:
    clFindInFilesEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clFindInFilesEvent(const clFindInFilesEvent& event);
    clFindInFilesEvent& operator=(const clFindInFilesEvent& src);
    virtual ~clFindInFilesEvent();
    virtual wxEvent* Clone() const { return new clFindInFilesEvent(*this); }
    void SetFileMask(const wxString& fileMask) { this->m_fileMask = fileMask; }
    void SetOptions(size_t options) { this->m_options = options; }
    const wxString& GetFileMask() const { return m_fileMask; }
    size_t GetOptions() const { return m_options; }
    void SetPaths(const wxString& paths) { this->m_paths = paths; }
    const wxString& GetPaths() const { return m_paths; }
    void SetTransientPaths(const wxString& transientPaths) { this->m_transientPaths = transientPaths; }
    const wxString& GetTransientPaths() const { return m_transientPaths; }
};

typedef void (wxEvtHandler::*clFindInFilesEventFunction)(clFindInFilesEvent&);
#define clFindInFilesEventHandler(func) wxEVENT_HANDLER_CAST(clFindInFilesEventFunction, func)

// --------------------------------------------------------------
// Parsing event
// --------------------------------------------------------------
class WXDLLIMPEXP_CL clParseEvent : public clCommandEvent
{
    size_t m_curfileIndex;
    size_t m_totalFiles;

public:
    clParseEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clParseEvent(const clParseEvent& event);
    clParseEvent& operator=(const clParseEvent& src);
    virtual ~clParseEvent();
    virtual wxEvent* Clone() const { return new clParseEvent(*this); }
    void SetCurfileIndex(size_t curfileIndex) { this->m_curfileIndex = curfileIndex; }
    void SetTotalFiles(size_t totalFiles) { this->m_totalFiles = totalFiles; }
    size_t GetCurfileIndex() const { return m_curfileIndex; }
    size_t GetTotalFiles() const { return m_totalFiles; }
};

typedef void (wxEvtHandler::*clParseEventFunction)(clParseEvent&);
#define clParseEventHandler(func) wxEVENT_HANDLER_CAST(clParseEventFunction, func)

// --------------------------------------------------------------
// clEditorConfigEvent event
// --------------------------------------------------------------
class WXDLLIMPEXP_CL clEditorConfigEvent : public clCommandEvent
{
    clEditorConfigSection m_editorConfigSection;

public:
    clEditorConfigEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clEditorConfigEvent(const clEditorConfigEvent& event);
    clEditorConfigEvent& operator=(const clEditorConfigEvent& src);
    virtual ~clEditorConfigEvent();
    virtual wxEvent* Clone() const { return new clEditorConfigEvent(*this); }
    void SetEditorConfig(const clEditorConfigSection& editorConfig) { this->m_editorConfigSection = editorConfig; }
    const clEditorConfigSection& GetEditorConfig() const { return m_editorConfigSection; }
};

typedef void (wxEvtHandler::*clEditorConfigEventFunction)(clEditorConfigEvent&);
#define clEditorConfigEventHandler(func) wxEVENT_HANDLER_CAST(clEditorConfigEventFunction, func)

#endif // CLCOMMANDEVENT_H
