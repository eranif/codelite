#ifndef FINDINFILESLOCATIONSDLG_H
#define FINDINFILESLOCATIONSDLG_H
#include "findinfiles_dlg.h"

class FindInFilesLocationsDlg : public FindInFilesLocationsDlgBase
{
    wxArrayString m_initialLocations;

protected:
    virtual void OnAddPath(wxCommandEvent& event);
    virtual void OnDeletePath(wxCommandEvent& event);
    virtual void OnDeletePathUI(wxUpdateUIEvent& event);
    void DoAppendItem(const wxString& str);
    void DoAppendItem(const wxString& str, bool check);

public:
    FindInFilesLocationsDlg(wxWindow* parent, const wxArrayString& locations);
    virtual ~FindInFilesLocationsDlg();

    wxArrayString GetLocations() const;
};
#endif // FINDINFILESLOCATIONSDLG_H
