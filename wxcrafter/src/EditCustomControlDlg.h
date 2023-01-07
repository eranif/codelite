#ifndef EDITCUSTOMCONTROLDLG_H
#define EDITCUSTOMCONTROLDLG_H
#include "wxcrafter.h"

class EditCustomControlDlg : public EditCustomControlDlgBaseClass
{
    bool m_isModified;

public:
    EditCustomControlDlg(wxWindow* parent);
    virtual ~EditCustomControlDlg();

protected:
    virtual void OnEventEditDone(wxDataViewEvent& event);
    virtual void OnDeleteEventUI(wxUpdateUIEvent& event);
    virtual void OnDeleteEvent(wxCommandEvent& event);
    virtual void OnNewEvent(wxCommandEvent& event);
    virtual void OnControlModified(wxCommandEvent& event);
    virtual void OnSaveUI(wxUpdateUIEvent& event);
    virtual void OnSave(wxCommandEvent& event);
    virtual void OnSelectControl(wxCommandEvent& event);
};
#endif // EDITCUSTOMCONTROLDLG_H
