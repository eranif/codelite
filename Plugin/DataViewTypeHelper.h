#pragma once

#include "codelite_exports.h"

#include <wx/dataview.h>
#include <wx/event.h>
#include <wx/timer.h>

/// Adds type-ahead (incremental search) support to a wxDataViewCtrl: typing
/// while the control has focus accumulates a search buffer and selects the
/// next row (starting just after the current selection, wrapping around)
/// whose visible text starts with it. The buffer resets after a short period
/// of inactivity, or immediately on a non-matching keystroke.
class WXDLLIMPEXP_SDK DataViewTypeHelper : public wxEvtHandler
{
public:
    explicit DataViewTypeHelper(wxDataViewCtrl* ctrl);
    ~DataViewTypeHelper() override;

    DataViewTypeHelper(const DataViewTypeHelper&) = delete;
    DataViewTypeHelper& operator=(const DataViewTypeHelper&) = delete;

private:
    void OnKeyDown(wxKeyEvent& event);
    void OnTimer(wxTimerEvent& event);
    wxString GetRowText(const wxDataViewItem& item) const;
    void SelectMatch(const wxString& searchText);
    void CollectVisibleItems(const wxDataViewItem& parent, wxDataViewItemArray& items) const;

    wxDataViewCtrl* m_ctrl = nullptr;
    wxTimer m_timer;
    wxString m_searchBuffer;
};
