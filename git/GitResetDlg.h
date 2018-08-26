#ifndef GITRESETDLG_H
#define GITRESETDLG_H
#include "gitui.h"

class GitResetDlg : public GitResetDlgBase
{
public:
    GitResetDlg(wxWindow* parent, const wxArrayString& filesToRemove,  const wxArrayString& filesToRevert);
    virtual ~GitResetDlg();
    
    wxArrayString GetItemsToRevert() const;
    wxArrayString GetItemsToRemove() const;
    
protected:
    virtual void OnToggleAllRevert(wxCommandEvent& event);
    virtual void OnToggleAllRemove(wxCommandEvent& event);
    virtual void OnToggleAllRevertUI(wxUpdateUIEvent& event);
    virtual void OnToggleAllRemoveUI(wxUpdateUIEvent& event);

    bool m_toggleReverts;
    bool m_toggleRemoves;
};
#endif // GITRESETDLG_H
