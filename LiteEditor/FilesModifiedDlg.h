#ifndef FILESMODIFIEDDLG_H
#define FILESMODIFIEDDLG_H
#include "filechecklistbase.h"

class FilesModifiedDlg : public FilesModifiedDlgBase
{
public:
    enum {
        kID_BUTTON_CHOOSE = ID_BUTTON_CHOOSE,
        kID_BUTTON_LOAD   = ID_BUTTON_LOAD,
    };
    
public:
    FilesModifiedDlg(wxWindow* parent);
    virtual ~FilesModifiedDlg();
    
    bool GetRememberMyAnswer() const {
        return m_checkBoxRemember->IsChecked();
    }
protected:
    virtual void OnChoose(wxCommandEvent& event);
    virtual void OnLoad(wxCommandEvent& event);
};
#endif // FILESMODIFIEDDLG_H
