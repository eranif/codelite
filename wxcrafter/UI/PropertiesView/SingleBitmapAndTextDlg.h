#ifndef SINGLEBITMAPANDTEXTDLG_H
#define SINGLEBITMAPANDTEXTDLG_H
#include "wxcrafter.h"

class SingleBitmapAndTextDlg : public SingleBitmapAndTextDlgBase
{
public:
    SingleBitmapAndTextDlg(wxWindow* parent, const wxString& bmp, const wxString& text);
    ~SingleBitmapAndTextDlg() override = default;

protected:
    void OnOKUI(wxUpdateUIEvent& event) override;
    void OnSelectBitmap(wxCommandEvent& event) override;
};
#endif // SINGLEBITMAPANDTEXTDLG_H
