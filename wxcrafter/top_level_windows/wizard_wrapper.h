#ifndef WIZARDWRAPPER_H
#define WIZARDWRAPPER_H

#include "top_level_win_wrapper.h"

class WizardWrapper : public TopLevelWinWrapper
{
protected:
    virtual wxString DoGenerateClassMember() const;

public:
    virtual bool HasIcon() const;
    WizardWrapper();
    virtual ~WizardWrapper();

    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    wxString BaseCtorDecl() const;
    wxString BaseCtorImplPrefix() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    wxString DesignerXRC(bool forPreviewDialog) const;
    void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString CppCtorCode() const;
    wxcWidget* Clone() const { return new WizardWrapper(); }

    virtual wxString GetWxClassName() const { return wxT("wxWizard"); }
};

#endif // WIZARDWRAPPER_H
