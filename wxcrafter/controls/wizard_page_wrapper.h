#ifndef WIZARDPAGEWRAPPER_H
#define WIZARDPAGEWRAPPER_H

#include "panel_wrapper.h" // Base class: PanelWrapper

class WizardPageWrapper : public PanelWrapper
{
public:
    WizardPageWrapper();
    virtual ~WizardPageWrapper();
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
};

#endif // WIZARDPAGEWRAPPER_H
