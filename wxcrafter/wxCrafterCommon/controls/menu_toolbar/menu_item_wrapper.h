#ifndef MENUITEMWRAPPER_H
#define MENUITEMWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class MenuItemWrapper : public wxcWidget
{
protected:
    wxString XRCKind() const;

public:
    MenuItemWrapper();
    ~MenuItemWrapper() override = default;
    void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

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

#endif // MENUITEMWRAPPER_H
