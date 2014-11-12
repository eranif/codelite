#ifndef PHPDEBUGSTARTDLG_H
#define PHPDEBUGSTARTDLG_H
#include "php_ui.h"
#include "php_project.h"

class PHPDebugStartDlg : public PHPDebugStartDlgBase
{
    PHPProject::Ptr_t m_project;
    IManager* m_manager;
protected:
    void SetBookSelection(int sel) { m_simpleBook->SetSelection(sel); }
    
public:
    PHPDebugStartDlg(wxWindow* parent, PHPProject::Ptr_t pProject, IManager* manager);
    virtual ~PHPDebugStartDlg();
    wxString GetPath() const;
    
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnUseActiveEditor(wxCommandEvent& event);
    virtual void OnDebugMethodChanged(wxCommandEvent& event);
    virtual void OnScriptToDebugUI(wxUpdateUIEvent& event);
};
#endif // PHPDEBUGSTARTDLG_H
