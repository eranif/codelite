#ifndef BITMAPSELECTORDLG_H
#define BITMAPSELECTORDLG_H
#include "wxcrafter.h"

class BitmapSelectorDlg : public BitmapSelectorDlgBase
{
public:
    BitmapSelectorDlg(wxWindow* parent, const wxString& selection);
    ~BitmapSelectorDlg() override;
    wxString GetBitmapFile() const;

protected:
    void OnUseRelativePaths(wxCommandEvent& event) override;
    void OnUseFilePicker(wxCommandEvent& event) override;
    void OnUserArtProvider(wxCommandEvent& event) override;
    void OnBrowseFile(wxCommandEvent& event) override;
    void OnSelectBitmapUI(wxUpdateUIEvent& event) override;
    void OnUserArtProviderUI(wxUpdateUIEvent& event) override;
};
#endif // BITMAPSELECTORDLG_H
