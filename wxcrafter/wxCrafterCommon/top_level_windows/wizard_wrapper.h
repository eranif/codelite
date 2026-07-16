#ifndef WIZARDWRAPPER_H
#define WIZARDWRAPPER_H

#include "top_level_win_wrapper.h"

class WizardWrapper : public TopLevelWinWrapper
{
protected:
    wxString DoGenerateClassMember() const override;

public:
    WizardWrapper();
    ~WizardWrapper() override = default;

    bool HasIcon() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString BaseCtorDecl() const override;
    wxString BaseCtorImplPrefix() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    wxString DesignerXRC(bool forPreviewDialog) const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString CppCtorCode() const override;
    wxcWidget* Clone() const override { return new WizardWrapper(); }

    wxString GetWxClassName() const override { return wxT("wxWizard"); }
};

#endif // WIZARDWRAPPER_H
