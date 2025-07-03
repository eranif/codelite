#ifndef DIALOGWRAPPER_H
#define DIALOGWRAPPER_H

#include "top_level_win_wrapper.h" // Base class: TopLevelWinWrapper

class DialogWrapper : public TopLevelWinWrapper
{

public:
    DialogWrapper();
    ~DialogWrapper() override = default;

public:
    bool HasIcon() const override;
    wxString DesignerXRC(bool forPreviewDialog) const override;
    wxString BaseCtorDecl() const override;
    wxString BaseCtorImplPrefix() const override;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // DIALOGWRAPPER_H
