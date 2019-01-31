#ifndef CHECKBOXWRAPPER_H
#define CHECKBOXWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class CheckBoxWrapper : public wxcWidget
{

public:
    CheckBoxWrapper();
    virtual ~CheckBoxWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // CHECKBOXWRAPPER_H
