#ifndef BUTTONWRAPPER_H
#define BUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ButtonWrapper : public wxcWidget
{
protected:
    void GetIncludeFile(wxArrayString& headers) const override;

public:
    ButtonWrapper();
    ~ButtonWrapper() override = default;

public:
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    wxcWidget* Clone() const override { return new ButtonWrapper(); }

    void SetLabel(const wxString& label) { DoSetPropertyStringValue(PROP_LABEL, label); }

    wxString GetLabel() const { return PropertyString(PROP_LABEL); }
};

#endif // BUTTONWRAPPER_H
