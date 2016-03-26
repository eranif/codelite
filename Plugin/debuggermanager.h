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

#include "map"
#include "list"
#include "vector"
#include "codelite_exports.h"
#include "wx/string.h"
#include "debugger.h"
#include "wx/arrstr.h"
#include "serialized_object.h"
#include "dynamiclibrary.h"

class EnvironmentConfig;
class BreakptMgr;

// sent when a "QueryLocals" command is completed (only for locals - not for function arguments)
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_QUERY_LOCALS;

// sent when a variable object createion is completed
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_VAROBJECT_CREATED;

// sent by codelite when a pane is needed to refresh its content
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_UPDATE_VIEWS;

// sent by the debugger when a "ListChildren" command is completed
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_LIST_CHILDREN;

// sent by the debugger after a successfull evaluation of variable object
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_VAROBJ_EVALUATED;

// sent by the debugger when a "disasseble" command returned
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_DISASSEBLE_OUTPUT;

// sent by the debugger when a "disasseble" current line command returns
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_DISASSEBLE_CURLINE;

// sent by the debugger when a "QueryFileLine" command has completed
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_QUERY_FILELINE;

// sent by the debugger in case "ResolveType" command failed (i.e. gdb could not resolve its type)
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR;

// sent by the debugger when 'ListRegisters' function completed
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_LIST_REGISTERS;

// Call stack
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_LIST_FRAMES;

// frame selected (user double clicked a stack entry)
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_DEBUGGER_FRAME_SELECTED;

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
    bool LoadDebuggers();

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

    static DebuggerMgr& Get();
    static void Free();
};
#endif // DEBUGGER_MANAGER_H
