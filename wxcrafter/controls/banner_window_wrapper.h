#ifndef BANNERWINDOWWRAPPER_H
#define BANNERWINDOWWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class BannerWindowWrapper : public wxcWidget
{
public:
    BannerWindowWrapper();
    ~BannerWindowWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // BANNERWINDOWWRAPPER_H
