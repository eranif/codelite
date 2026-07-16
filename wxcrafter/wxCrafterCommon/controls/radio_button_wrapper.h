#ifndef RADIOBUTTONWRAPPER_H
#define RADIOBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class RadioButtonWrapper : public wxcWidget
{

public:
    RadioButtonWrapper();
    ~RadioButtonWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // RADIOBUTTONWRAPPER_H
