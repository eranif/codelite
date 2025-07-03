#ifndef EDITCUSTOMCONTROLDLG_H
#define EDITCUSTOMCONTROLDLG_H
#include "wxcrafter.h"

class EditCustomControlDlg : public EditCustomControlDlgBaseClass
{
    bool m_isModified;

public:
    EditCustomControlDlg(wxWindow* parent);
    ~EditCustomControlDlg() override = default;

protected:
    void OnEventEditDone(wxDataViewEvent& event) override;
    void OnDeleteEventUI(wxUpdateUIEvent& event) override;
    void OnDeleteEvent(wxCommandEvent& event) override;
    void OnNewEvent(wxCommandEvent& event) override;
    void OnControlModified(wxCommandEvent& event) override;
    void OnSaveUI(wxUpdateUIEvent& event) override;
    void OnSave(wxCommandEvent& event) override;
    void OnSelectControl(wxCommandEvent& event) override;
};
#endif // EDITCUSTOMCONTROLDLG_H
