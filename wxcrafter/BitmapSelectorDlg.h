#ifndef BITMAPSELECTORDLG_H
#define BITMAPSELECTORDLG_H
#include "wxcrafter.h"

class BitmapSelectorDlg : public BitmapSelectorDlgBase
{
public:
    BitmapSelectorDlg(wxWindow* parent, const wxString& selection);
    virtual ~BitmapSelectorDlg();
    wxString GetBitmapFile() const;

protected:
    virtual void OnUseRelativePaths(wxCommandEvent& event);
    virtual void OnUseFilePicker(wxCommandEvent& event);
    virtual void OnUserArtProvider(wxCommandEvent& event);
    virtual void OnBrowseFile(wxCommandEvent& event);
    virtual void OnSelectBitmapUI(wxUpdateUIEvent& event);
    virtual void OnUserArtPorviderUI(wxUpdateUIEvent& event);
};
#endif // BITMAPSELECTORDLG_H
