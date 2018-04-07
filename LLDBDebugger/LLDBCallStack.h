//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBCallStack.h
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

#ifndef LLDBCALLSTACK_H
#define LLDBCALLSTACK_H

#include "UI.h"
#include "LLDBProtocol/LLDBBacktrace.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBCallStackPane;
class LLDBConnector;
class LLDBPlugin;

class CallstackModel : public wxDataViewListStore
{
    LLDBCallStackPane* m_ctrl;
    wxDataViewListCtrl* m_view;
public:
    CallstackModel(LLDBCallStackPane* ctrl, wxDataViewListCtrl* view) : m_ctrl(ctrl), m_view(view) {}
    virtual ~CallstackModel() {}
    
    bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const ;
};

class LLDBCallStackPane : public LLDBCallStackBase
{
    LLDBPlugin& m_plugin;
    LLDBConnector* m_connector;
    int m_selectedFrame;
    wxObjectDataPtr<CallstackModel> m_model;
private:
    void DoCopyBacktraceToClipboard();
    
protected:
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    void OnBacktrace(LLDBEvent &event);
    void OnRunning(LLDBEvent &event);

public:
    LLDBCallStackPane(wxWindow* parent, LLDBPlugin& plugin);
    virtual ~LLDBCallStackPane();
    
    void SetSelectedFrame(int selectedFrame) {
        this->m_selectedFrame = selectedFrame;
    }
    int GetSelectedFrame() const {
        return m_selectedFrame;
    }
};

#endif // LLDBCALLSTACK_H
