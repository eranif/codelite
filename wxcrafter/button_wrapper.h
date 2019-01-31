#ifndef BUTTONWRAPPER_H
#define BUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ButtonWrapper : public wxcWidget
{
protected:
    virtual void GetIncludeFile(wxArrayString& headers) const;

public:
    ButtonWrapper();
    virtual ~ButtonWrapper();

public:
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual wxcWidget* Clone() const { return new ButtonWrapper(); }

    void SetLabel(const wxString& label) { DoSetPropertyStringValue(PROP_LABEL, label); }

    wxString GetLabel() const { return PropertyString(PROP_LABEL); }
};

#endif // BUTTONWRAPPER_H
