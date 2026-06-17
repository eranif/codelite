#ifndef WIZARDPAGEWRAPPER_H
#define WIZARDPAGEWRAPPER_H

#include "panel_wrapper.h" // Base class: PanelWrapper

#include <functional>

class WizardPageWrapper : public PanelWrapper
{
public:
    WizardPageWrapper();
    ~WizardPageWrapper() override = default;

    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;

    static std::function<bool(const wxcWidget* /*page*/)> isActiveWizardPage;
};

#endif // WIZARDPAGEWRAPPER_H
