#ifndef ABOUTDLG_H
#define ABOUTDLG_H
#include "wxcrafter.h"

class wxcAboutDlg : public wxcAboutDlgBaseClass
{
    bool m_modified;

public:
    wxcAboutDlg(wxWindow* parent);
    virtual ~wxcAboutDlg();

protected:
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
    virtual void OnUpdateLicenseUI(wxUpdateUIEvent& event);
    virtual void OnUpdateLicense(wxCommandEvent& event);

    void DoRefresh();
    bool IsValidUser(const wxString& username, const wxString& serialNumber) const;
};
#endif // ABOUTDLG_H
