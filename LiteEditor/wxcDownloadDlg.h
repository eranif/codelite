#ifndef WXCDOWNLOADDLG_H
#define WXCDOWNLOADDLG_H
#include "wxcrafter.h"

class wxcDownloadDlg : public wxcDownloadDlgBaseClass
{
public:
    wxcDownloadDlg(wxWindow* parent);
    virtual ~wxcDownloadDlg();
protected:
    virtual void OnDownloadWxCrafterPlugin(wxCommandEvent& event);
    virtual void OnIgnoreTheError(wxCommandEvent& event);
};
#endif // WXCDOWNLOADDLG_H
