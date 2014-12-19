#ifndef NEWPHPPROJECTDLG_H
#define NEWPHPPROJECTDLG_H

#include "php_ui.h"
#include "php_project.h"

class NewPHPProjectDlg : public NewPHPProjectDlgBase
{
public:
    NewPHPProjectDlg(wxWindow* parent);
    virtual ~NewPHPProjectDlg();
    PHPProject::CreateData GetCreateData();

protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnCreateUnderSeparateFolderUI(wxUpdateUIEvent& event);
    virtual void OnCreateUnderSeparateFolder(wxCommandEvent& event);
    virtual void OnNameUpdated(wxCommandEvent& event);
    virtual void OnPathUpdated(wxFileDirPickerEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    
private:
    void DoUpdateProjectFolder();
};
#endif // NEWPHPPROJECTDLG_H
