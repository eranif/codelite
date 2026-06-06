#ifndef DELETECUSTOMCONTROLDLG_H
#define DELETECUSTOMCONTROLDLG_H
#include "wxcrafter.hpp"

class DeleteCustomControlDlg : public DeleteCustomControlDlgBaseClass
{
    wxArrayString m_controlsToDelete;

public:
    DeleteCustomControlDlg(wxWindow* parent);
    ~DeleteCustomControlDlg() override = default;

protected:
    void OnItemValueChanged(wxDataViewEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;
    void OnDeleteControls(wxCommandEvent& event) override;

    void DoPopulate();
};
#endif // DELETECUSTOMCONTROLDLG_H
