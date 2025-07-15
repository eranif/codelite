#ifndef STATICBITMAPWRAPPER_H
#define STATICBITMAPWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class StaticBitmapWrapper : public wxcWidget
{

public:
    StaticBitmapWrapper();
    ~StaticBitmapWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // STATICBITMAPWRAPPER_H
