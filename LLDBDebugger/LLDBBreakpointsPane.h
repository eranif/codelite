//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBBreakpointsPane.h
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

#ifndef LLDBBREAKPOINTSPANE_H
#define LLDBBREAKPOINTSPANE_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "LLDBProtocol/LLDBConnector.h"

class LLDBDebugger;
class LLDBPlugin;
class LLDBBreakpointsPane : public LLDBBreakpointsPaneBase
{
    LLDBPlugin *m_plugin;
    LLDBConnector* m_connector;
    
public:
    LLDBBreakpointsPane(wxWindow* parent, LLDBPlugin *plugin);
    virtual ~LLDBBreakpointsPane();
    
    void Initialize();
    void Clear();
    
    // Event handlers
    void OnBreakpointsUpdated(LLDBEvent &event);
    LLDBBreakpoint::Ptr_t GetBreakpoint(const wxDataViewItem& item);
protected:
    void GotoBreakpoint(LLDBBreakpoint::Ptr_t bp);
    
protected:
    virtual void OnDeleteAll(wxCommandEvent& event);
    virtual void OnDeleteAllUI(wxUpdateUIEvent& event);
    virtual void OnDeleteBreakpoint(wxCommandEvent& event);
    virtual void OnDeleteBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnNewBreakpoint(wxCommandEvent& event);
    virtual void OnNewBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnBreakpointActivated(wxDataViewEvent& event);
};

#endif // LLDBBREAKPOINTSPANE_H
