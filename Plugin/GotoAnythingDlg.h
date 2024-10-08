#ifndef GOTOANYTHINGDLG_H
#define GOTOANYTHINGDLG_H

#include "GotoAnythingBaseUI.h"
#include "bitmap_loader.h"
#include "clGotoAnythingManager.h"
#include "clThemedListCtrl.h"
#include "codelite_exports.h"

#include <vector>

class WXDLLIMPEXP_SDK GotoAnythingDlg : public GotoAnythingBaseDlg
{
    const std::vector<clGotoEntry>& m_allEntries;
    wxString m_currentFilter;
    clThemedListCtrl::BitmapVec_t m_bitmaps;

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);

    void DoPopulate(const std::vector<clGotoEntry>& entries, const std::vector<int>& indexes = std::vector<int>());
    void DoExecuteActionAndClose();
    void ApplyFilter();

public:
    GotoAnythingDlg(wxWindow* parent, const std::vector<clGotoEntry>& entries);
    virtual ~GotoAnythingDlg();

protected:
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    void OnIdle(wxIdleEvent& e);
    void DoSelectItem(const wxDataViewItem& item);
};

#endif // GOTOANYTHINGDLG_H
