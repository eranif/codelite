#pragma once

#include "GotoAnythingBaseUI.h"
#include "clGotoEntry.h"
#include "clThemedListCtrl.h"
#include "codelite_exports.h"

#include <vector>

class WXDLLIMPEXP_SDK GotoAnythingDlg : public GotoAnythingBaseDlg
{
public:
    GotoAnythingDlg(wxWindow* parent, const std::vector<clGotoEntry>& entries);
    ~GotoAnythingDlg() override;

protected:
    void OnItemActivated(wxDataViewEvent& event) override;

    void DoPopulate(const std::vector<clGotoEntry>& entries, const std::vector<int>& indexes = std::vector<int>());
    void DoExecuteActionAndClose();
    void ApplyFilter();
    void OnEnter(wxCommandEvent& event) override;
    void OnKeyDown(wxKeyEvent& event) override;
    void OnIdle(wxIdleEvent& e) override;
    void DoSelectItem(const wxDataViewItem& item);

private:
    const std::vector<clGotoEntry>& m_allEntries;
    wxString m_currentFilter;
    clThemedListCtrl::BitmapVec_t m_bitmaps;
};
