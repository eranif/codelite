//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSDebuggerPane.h
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

#ifndef NODEJSDEBUGGERPANE_H
#define NODEJSDEBUGGERPANE_H

#include "NodeJSEvents.h"
#include "NodeJSOuptutParser.h"
#include "WebToolsBase.h"
#include "cl_command_event.h"
#include "json_node.h"
#include <map>

class wxTerminal;
class NodeJSDebuggerPane : public NodeJSDebuggerPaneBase
{
    struct FrameData {
        int index;
        int line;
        wxString file;
        wxString function;
        FrameData()
            : index(wxNOT_FOUND)
            , line(wxNOT_FOUND)
        {
        }
    };

public:
    std::map<int, NodeJSHandle> m_handles;
    std::vector<PendingLookupDV> m_pendingLookupRefs;
    wxTerminal* m_terminal;

protected:
    virtual void OnLocalExpanding(wxDataViewEvent& event);
    virtual void OnEvaluateExpression(clCommandEvent& event);
    virtual void OnBreakpointSelected(wxDataViewEvent& event);
    void ClearCallstack();
    void BuildLocals(const JSONElement& json);
    void BuildArguments(const JSONElement& json);
    wxDataViewItem AddLocal(const wxDataViewItem& parent, const wxString& name, int refId);
    void ParseRefsArray(const JSONElement& refs);
    NodeJSHandle ParseRef(const JSONElement& ref);
    void DoOpenFile(const wxString& filename, int line);
    void DoDeleteLocalItemAfter(const wxDataViewItem& item);
    void DoAddKnownRefs(const std::vector<std::pair<int, wxString> >& refs, const wxDataViewItem& parent);
    void DoAddUnKnownRefs(const std::vector<std::pair<int, wxString> >& refs, const wxDataViewItem& parent);
    void Clear();

protected:
    void OnItemActivated(wxDataViewEvent& event);
    void OnUpdateCallstack(clDebugEvent& event);
    void OnExpressionEvaluated(clDebugEvent& event);
    void OnLostControl(clDebugEvent& event);
    void OnLookup(clDebugEvent& event);
    void OnConsoleLog(clDebugEvent& event);
    void OnSessionStarted(clDebugEvent& event);
    void OnSessionStopped(clDebugEvent& event);
    void OnExceptionThrown(clDebugEvent& event);
    void OnUpdateDebuggerView(clDebugEvent& event);
    void OnFrameSelected(clDebugEvent& event);
    void OnSettingsChanged(wxCommandEvent& event);

public:
    NodeJSDebuggerPane(wxWindow* parent);
    virtual ~NodeJSDebuggerPane();
};
#endif // NODEJSDEBUGGERPANE_H
