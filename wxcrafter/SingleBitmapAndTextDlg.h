#ifndef SINGLEBITMAPANDTEXTDLG_H
#define SINGLEBITMAPANDTEXTDLG_H
#include "wxcrafter.h"

class SingleBitmapAndTextDlg : public SingleBitmapAndTextDlgBase
{
public:
    SingleBitmapAndTextDlg(wxWindow* parent, const wxString& bmp, const wxString& text);
    virtual ~SingleBitmapAndTextDlg();

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnSelectBitmap(wxCommandEvent& event);
};
#endif // SINGLEBITMAPANDTEXTDLG_H
