#ifndef GOTOANYTHINGDLG_H
#define GOTOANYTHINGDLG_H

#include "GotoAnythingBaseUI.h"
#include "clGotoAnythingManager.h"
#include "codelite_exports.h"
#include <vector>

// class WXDLLIMPEXP_SDK GotoAnythingItemData
// {
// public:
//     wxString m_desc;
//     GotoAnythingItemData(const wxString& desc)
//         : m_desc(desc)
//     {
//     }
//     ~GotoAnythingItemData() {}
// };
//
class WXDLLIMPEXP_SDK GotoAnythingDlg : public GotoAnythingBaseDlg
{
    std::vector<clGotoEntry> m_allEntries;
    wxString m_currentFilter;

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    // GotoAnythingItemData* GetSelectedItemData();
    void DoPopulate(const std::vector<clGotoEntry>& entries, const std::vector<int>& indexes = std::vector<int>());
    void DoExecuteActionAndClose();
    void UpdateLastSearch();
    void ApplyFilter();

public:
    GotoAnythingDlg(wxWindow* parent);
    virtual ~GotoAnythingDlg();

protected:
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    void OnIdle(wxIdleEvent& e);
};

#endif // GOTOANYTHINGDLG_H
