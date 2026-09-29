#ifndef FINDINFILESLOCATIONSDLG_H
#define FINDINFILESLOCATIONSDLG_H
#include "findinfiles_dlg.hpp"

class FindInFilesLocationsDlg : public FindInFilesLocationsDlgBase
{
    wxArrayString m_initialLocations;

protected:
    void OnAddPath(wxCommandEvent& event) override;
    void OnDeletePath(wxCommandEvent& event) override;
    void OnDeletePathUI(wxUpdateUIEvent& event) override;
    void DoAppendItem(const wxString& str);
    void DoAppendItem(const wxString& str, bool check);

public:
    FindInFilesLocationsDlg(wxWindow* parent, const wxArrayString& locations);
    ~FindInFilesLocationsDlg() override = default;

    wxArrayString GetLocations() const;
};
#endif // FINDINFILESLOCATIONSDLG_H
