#ifndef MENUITEMWRAPPER_H
#define MENUITEMWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class MenuItemWrapper : public wxcWidget
{
protected:
    wxString XRCKind() const;

public:
    MenuItemWrapper();
    virtual ~MenuItemWrapper();
    virtual void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // MENUITEMWRAPPER_H
