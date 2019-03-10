//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggermanager.cpp
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
#include "cl_defs.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "debuggerconfigtool.h"
#include "debuggermanager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "wx/filename.h"
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

//---------------------------------------------------------
static DebuggerMgr* ms_instance = NULL;

wxDEFINE_EVENT(wxEVT_DEBUGGER_UPDATE_VIEWS, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_QUERY_LOCALS, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_LIST_CHILDREN, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_VAROBJ_EVALUATED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_VAROBJECT_CREATED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_DISASSEBLE_CURLINE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_QUERY_FILELINE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_LIST_REGISTERS, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_LIST_FRAMES, clCommandEvent);
wxDEFINE_EVENT(wxEVT_DEBUGGER_FRAME_SELECTED, clCommandEvent);

DebuggerMgr::DebuggerMgr() {}

DebuggerMgr::~DebuggerMgr()
{
    std::vector<clDynamicLibrary*>::iterator iter = m_dl.begin();
    for(; iter != m_dl.end(); iter++) {
        (*iter)->Detach();
        delete(*iter);
    }
    m_dl.clear();
    m_debuggers.clear();
}

DebuggerMgr& DebuggerMgr::Get()
{
    if(!ms_instance) { ms_instance = new DebuggerMgr(); }
    return *ms_instance;
}

void DebuggerMgr::Free()
{
    delete ms_instance;
    ms_instance = NULL;
}

bool DebuggerMgr::LoadDebuggers()
{
    wxString ext;

#if defined(__WXMSW__)
    ext = wxT("dll");

#elif defined(__WXMAC__)
    ext = wxT("dylib");

#else
    ext = wxT("so");

#endif

    wxString fileSpec(wxT("*.") + ext);

    // get list of dlls
    wxArrayString files;
#ifdef __WXGTK__
    wxString debuggersPath(PLUGINS_DIR, wxConvUTF8);
    debuggersPath += wxT("/debuggers");
#elif defined(__WXMSW__)
#ifdef USE_POSIX_LAYOUT
    wxString debuggersPath(clStandardPaths::Get().GetPluginsDirectory() + wxT("/debuggers"));
#else
    wxString debuggersPath(m_baseDir + wxT("/debuggers"));
#endif
#else
    // OSX
    wxFileName debuggersFolder(clStandardPaths::Get().GetDataDir(), "");
    debuggersFolder.AppendDir("debuggers");
    wxString debuggersPath(debuggersFolder.GetPath());
#endif

    clDEBUG() << "Loading debuggers from:" << debuggersPath;
    wxDir::GetAllFiles(debuggersPath, &files, fileSpec, wxDIR_FILES);

    for(size_t i = 0; i < files.GetCount(); i++) {
        clDynamicLibrary* dl = new clDynamicLibrary();
        wxString fileName(files.Item(i));
        
#if defined(__WXMSW__) && CL_DEBUG_BUILD
        // Under MSW loading a release plugin while in debug mode will cause a crash
        if(!fileName.EndsWith("-dbg.dll")) { continue; }
#elif defined(__WXMSW__)
        // filter debug plugins
        if(fileName.EndsWith("-dbg.dll")) { continue; }
#endif
        clDEBUG() << "Attempting to load debugger:" << fileName;
        if(!dl->Load(fileName)) {
            CL_WARNING("Failed to load debugger: %s", fileName);
            if(!dl->GetError().IsEmpty()) { CL_WARNING("%s", dl->GetError()); }
            wxDELETE(dl);
            continue;
        }

        bool success(false);
        GET_DBG_INFO_FUNC pfn = (GET_DBG_INFO_FUNC)dl->GetSymbol(wxT("GetDebuggerInfo"), &success);
        if(!success) {
            clLogMessage(wxT("Failed to find GetDebuggerInfo() in dll: ") + fileName);
            if(!dl->GetError().IsEmpty()) { clLogMessage(dl->GetError()); }
            // dl->Unload();
            delete dl;
            continue;
        }

        DebuggerInfo info = pfn();
        // Call the init method to create an instance of the debugger
        success = false;
        GET_DBG_CREATE_FUNC pfnInitDbg = (GET_DBG_CREATE_FUNC)dl->GetSymbol(info.initFuncName, &success);
        if(!success) {
            clLogMessage(wxT("Failed to find init function in dll: ") + fileName);
            if(!dl->GetError().IsEmpty()) { clLogMessage(dl->GetError()); }
            dl->Detach();
            delete dl;
            continue;
        }

        clLogMessage(wxT("Loaded debugger: ") + info.name + wxT(", Version: ") + info.version);
        IDebugger* dbg = pfnInitDbg();

        // set the environment
        dbg->SetEnvironment(m_env);

        m_debuggers[info.name] = dbg;

        // keep the dynamic load library
        m_dl.push_back(dl);
    }

    // Load all debuggers in the form of plugin (i.e. they dont implement the IDebugger interface)
    // and append them to a special list
    clDebugEvent queryPlugins(wxEVT_DBG_IS_PLUGIN_DEBUGGER);
    EventNotifier::Get()->ProcessEvent(queryPlugins);
    m_pluginsDebuggers.swap(queryPlugins.GetStrings());
    return true;
}

wxArrayString DebuggerMgr::GetAvailableDebuggers()
{
    wxArrayString dbgs;
    std::map<wxString, IDebugger*>::iterator iter = m_debuggers.begin();
    for(; iter != m_debuggers.end(); iter++) {
        dbgs.Add(iter->first);
    }

    // append all the plugins that were registered themself as debugger
    dbgs.insert(dbgs.end(), m_pluginsDebuggers.begin(), m_pluginsDebuggers.end());
    return dbgs;
}

IDebugger* DebuggerMgr::GetActiveDebugger()
{
    if(m_activeDebuggerName.IsEmpty()) {
        // no active debugger is set, use the first one
        std::map<wxString, IDebugger*>::const_iterator iter = m_debuggers.begin();
        if(iter != m_debuggers.end()) {
            SetActiveDebugger(iter->first);
            return iter->second;
        }
        return NULL;
    }

    std::map<wxString, IDebugger*>::iterator iter = m_debuggers.find(m_activeDebuggerName);
    if(iter != m_debuggers.end()) { return iter->second; }
    return NULL;
}

void DebuggerMgr::SetActiveDebugger(const wxString& name) { m_activeDebuggerName = name; }

void DebuggerMgr::SetDebuggerInformation(const wxString& name, const DebuggerInformation& info)
{
    DebuggerConfigTool::Get()->WriteObject(name, (SerializedObject*)&info);
}

bool DebuggerMgr::GetDebuggerInformation(const wxString& name, DebuggerInformation& info)
{
    return DebuggerConfigTool::Get()->ReadObject(name, &info);
}

bool DebuggerMgr::IsNativeDebuggerRunning() const
{
    std::map<wxString, IDebugger*>::const_iterator iter = m_debuggers.find(m_activeDebuggerName);
    if(iter == m_debuggers.end()) { return false; }

    IDebugger* d = iter->second;
    return d && d->IsRunning();
}
