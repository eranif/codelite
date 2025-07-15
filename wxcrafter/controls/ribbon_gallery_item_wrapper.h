#ifndef RIBBONGALLERYITEMWRAPPER_H
#define RIBBONGALLERYITEMWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonGalleryItemWrapper : public wxcWidget
{
public:
    RibbonGalleryItemWrapper();
    ~RibbonGalleryItemWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // RIBBONGALLERYITEMWRAPPER_H
