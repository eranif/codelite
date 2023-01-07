#ifndef DELETECUSTOMCONTROLDLG_H
#define DELETECUSTOMCONTROLDLG_H
#include "wxcrafter.h"

class DeleteCustomControlDlg : public DeleteCustomControlDlgBaseClass
{
    wxArrayString m_controlsToDelete;

public:
    DeleteCustomControlDlg(wxWindow* parent);
    virtual ~DeleteCustomControlDlg();

protected:
    virtual void OnItemValueChanged(wxDataViewEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
    virtual void OnDeleteControls(wxCommandEvent& event);

    void DoPopulate();
};
#endif // DELETECUSTOMCONTROLDLG_H
