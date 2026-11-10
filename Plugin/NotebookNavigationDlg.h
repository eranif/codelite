//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NotebookNavigationDlg.h
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

#pragma once

#include "clTabHistory.h"

#include <memory>
#include <wx/bitmap.h>
#include <wx/bookctrl.h>
#include <wx/dataview.h>
#include <wx/eventfilter.h>
#include <wx/filename.h>
#include <wx/popupwin.h>
#include <wx/timer.h>

struct TabData {
    wxString label;
    wxBitmap bmp;
    wxFileName filename;
    int index;
    bool isFile;

    TabData()
        : index(wxNOT_FOUND)
        , isFile(false)
    {
    }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOKNAV_SELECTED, wxBookCtrlEvent);

// Implementation detail (defined in the .cpp file)
class TabSwitcherEventFilter;

/// A popup window (it does not take the activation from the main frame). Create it with `new` and call ShowPopup().
/// When the user picks a tab, it notifies the frame (wxEVT_BOOKNAV_SELECTED) and destroys itself. It also destroys
/// itself when it is cancelled (Escape, or a click outside of it)
class WXDLLIMPEXP_SDK NotebookNavigationDlg : public wxPopupTransientWindow
{
public:
    NotebookNavigationDlg(wxWindow* parent, clTabHistory::Ptr_t history);
    ~NotebookNavigationDlg() override;

    /// Centre the popup on its parent and show it
    void ShowPopup();

protected:
    void OnDismiss() override;

    void OnItemActivated(wxDataViewEvent& event);
    void OnTimer(wxTimerEvent& event);

    void SelectNext();
    void SelectPrev();
    /// Cancel the selection (Escape): dismiss the popup without switching the tab
    void Cancel();
    void FinalizeCtor();
    /// Notify the frame about the chosen tab and dismiss the popup. Only the first call does something
    void NotifyAndClose();
    void Initialise();

    wxDataViewListCtrl* m_dvListCtrl{nullptr};
    wxTimer m_ctrlKeyTimer;
    bool m_dismissed{false};
    clTabHistory::Ptr_t m_history{nullptr};
    std::unique_ptr<TabSwitcherEventFilter> m_eventFilter{nullptr};
    friend class TabSwitcherEventFilter;
};
