#ifndef MEDIACTRLWRAPPER_H
#define MEDIACTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class MediaCtrlWrapper : public wxcWidget
{
public:
    MediaCtrlWrapper();
    ~MediaCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // MEDIACTRLWRAPPER_H
