#ifndef SCROLLBARWRAPPER_H
#define SCROLLBARWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ScrollBarWrapper : public wxcWidget
{

public:
    ScrollBarWrapper();
    ~ScrollBarWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // SCROLLBARWRAPPER_H
