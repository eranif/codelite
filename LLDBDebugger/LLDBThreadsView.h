//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBThreadsView.h
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

#ifndef LLDBTHREADSVIEW_H
#define LLDBTHREADSVIEW_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBThread.h"

class LLDBPlugin;

//-------------------------------------------------------------
//-------------------------------------------------------------

class LLDBThreadViewClientData : public wxClientData
{
    LLDBThread m_thread;
public:
    LLDBThreadViewClientData(const LLDBThread& thread) {
        m_thread = thread;
    }
    const LLDBThread& GetThread() const {
        return m_thread;
    }

};

//-------------------------------------------------------------
//-------------------------------------------------------------

class ThreadsModel : public wxDataViewListStore
{
    wxDataViewListCtrl* m_view;
public:
    ThreadsModel(wxDataViewListCtrl* view) : m_view(view){}
    virtual ~ThreadsModel() {}

    bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
        bool res = false;
        LLDBThreadViewClientData *cd = reinterpret_cast<LLDBThreadViewClientData*>(m_view->GetItemData(item));
        if (cd) {
            if(cd->GetThread().IsActive()) {
                attr.SetBold(true);
                res = true;
            }

            if(cd->GetThread().IsSuspended()) {
                attr.SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
                res = true;
            }
        }
        return res;
    }
};

//-------------------------------------------------------------
//-------------------------------------------------------------

class LLDBThreadsView : public LLDBThreadsViewBase
{
    LLDBPlugin *m_plugin;
    int m_selectedThread;
    wxObjectDataPtr<ThreadsModel> m_model;

public:
    LLDBThreadsView(wxWindow* parent, LLDBPlugin* plugin);
    virtual ~LLDBThreadsView();
private:
    void DoCleanup();

    void OnContextMenu(wxDataViewEvent& event);

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);

    void OnLLDBRunning(LLDBEvent &event);
    void OnLLDBStopped(LLDBEvent &event);
    void OnLLDBExited(LLDBEvent &event);
    void OnLLDBStarted(LLDBEvent &event);

};
#endif // LLDBTHREADSVIEW_H
