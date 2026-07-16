#ifndef TOP_PANELWRAPPER_H
#define TOP_PANELWRAPPER_H

#include "top_level_win_wrapper.h"
#include "wxc_widget.h" // Base class: WrapperBase

class PanelWrapperTopLevel : public TopLevelWinWrapper
{
protected:
    wxString BaseCtorDecl() const override;
    wxString BaseCtorImplPrefix() const override;

public:
    PanelWrapperTopLevel();
    ~PanelWrapperTopLevel() override = default;

public:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DesignerXRC(bool forPreviewDialog) const override;
    bool IsTopWindow() const override;
    wxcWidget* Clone() const override { return new PanelWrapperTopLevel(); }
};

#endif // TOP_PANELWRAPPER_H
