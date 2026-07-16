#ifndef AUITOOLBARTOPLEVEL_H
#define AUITOOLBARTOPLEVEL_H

#include "AuiToolbarWrapperBase.h"
#include "top_level_win_wrapper.h"

class AuiToolBarTopLevelWrapper : public AuiToolbarWrapperBase, public TopLevelWinWrapper
{
public:
    AuiToolBarTopLevelWrapper();
    ~AuiToolBarTopLevelWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString BaseCtorDecl() const override;
    wxString BaseCtorImplPrefix() const override;
    wxString DesignerXRC(bool forPreviewDialog) const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DoGenerateClassMember() const override;
    void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const override;
    wxString CreateBaseclassName() const override { return GetName(); }
    // not a real window
    bool IsWindow() const override { return true; }
    // no base class
    bool WantsSubclass() const override { return true; }
};

#endif // AUITOOLBARTOPLEVEL_H
