//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggermanager.h
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
#ifndef DEBUGGER_MANAGER_H
#define DEBUGGER_MANAGER_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "debugger.h"
#include "dynamiclibrary.h"
#include "list"
#include "map"
#include "serialized_object.h"
#include "vector"
#include "wx/arrstr.h"
#include "wx/string.h"

class EnvironmentConfig;

// sent when a "QueryLocals" command is completed (only for locals - not for function arguments)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_QUERY_LOCALS, clCommandEvent);

// sent when a variable object createion is completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_VAROBJECT_CREATED, clCommandEvent);

// sent by codelite when a pane is needed to refresh its content
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_UPDATE_VIEWS, clCommandEvent);

// sent by the debugger when a "ListChildren" command is completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_LIST_CHILDREN, clCommandEvent);

// sent by the debugger after a successfull evaluation of variable object
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_VAROBJ_EVALUATED, clCommandEvent);

// sent by the debugger when a "disasseble" command returned
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_DISASSEBLE_OUTPUT, clCommandEvent);

// sent by the debugger when a "disasseble" current line command returns
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_DISASSEBLE_CURLINE, clCommandEvent);

// sent by the debugger when a "QueryFileLine" command has completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_QUERY_FILELINE, clCommandEvent);

// sent by the debugger in case "ResolveType" command failed (i.e. gdb could not resolve its type)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, clCommandEvent);

// sent by the debugger when 'ListRegisters' function completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_LIST_REGISTERS, clCommandEvent);

// Call stack
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_LIST_FRAMES, clCommandEvent);

// frame selected (user double clicked a stack entry)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_DEBUGGER_FRAME_SELECTED, clCommandEvent);

class WXDLLIMPEXP_SDK DebuggerMgr
{
    std::map<wxString, IDebugger*> m_debuggers;
    wxArrayString m_pluginsDebuggers;
    wxString m_baseDir;
    std::vector<clDynamicLibrary*> m_dl;
    wxString m_activeDebuggerName;
    EnvironmentConfig* m_env;

private:
    DebuggerMgr();
    virtual ~DebuggerMgr();

public:
    /**
     * Set the base dir for the debugger manager. On Linux this is
     * equivalent to $(HOME)/.liteeditor/, and on windows it is set
     * to C:\Program Files\LiteEditor\
     */
    void Initialize(wxEvtHandler* parent, EnvironmentConfig* env, const wxString& dir)
    {
        m_baseDir = dir;
        m_env = env;
    }

    const wxString& GetActiveDebuggerName() const { return m_activeDebuggerName; }
    /**
     * Load all available debuggers. This functions searches for dll/so/sl
     * which are located udner $(HOME)/.liteeditor/debuggers/ on Linux, and on Windows
     * under C:\Program Files\LiteEditor\debuggers\
     */
    bool LoadDebuggers(IDebuggerObserver* observer);

    /**
     * Return list of all available debuggers which were loaded
     * successfully into the debugger manager
     */
    wxArrayString GetAvailableDebuggers();

    /**
     * Set the active debugger to be 'name'. If a debugger with name does not
     * exist, this function does nothing
     */
    void SetActiveDebugger(const wxString& name);

    /**
     * Return the currently selected debugger. The debugger is selected
     * based on previous call to SetActiveDebugger(). If no active debugger is
     * set, this function may return NULL
     */
    IDebugger* GetActiveDebugger();

    // get/set debugger information
    void SetDebuggerInformation(const wxString& name, const DebuggerInformation& info);
    bool GetDebuggerInformation(const wxString& name, DebuggerInformation& info);

    /**
     * @brief do we have an active debugger which is running?
     */
    bool IsNativeDebuggerRunning() const;
    static DebuggerMgr& Get();
    static void Free();
};
#endif // DEBUGGER_MANAGER_H
