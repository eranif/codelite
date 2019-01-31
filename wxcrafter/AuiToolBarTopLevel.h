#ifndef AUITOOLBARTOPLEVEL_H
#define AUITOOLBARTOPLEVEL_H

#include "AuiToolbarWrapperBase.h"
#include "top_level_win_wrapper.h"

class AuiToolBarTopLevelWrapper : public AuiToolbarWrapperBase, public TopLevelWinWrapper
{
public:
    AuiToolBarTopLevelWrapper();
    virtual ~AuiToolBarTopLevelWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString BaseCtorDecl() const;
    virtual wxString BaseCtorImplPrefix() const;
    virtual wxString DesignerXRC(bool forPreviewDialog) const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DoGenerateClassMember() const;
    void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const;
    virtual wxString CreateBaseclassName() const { return GetName(); }
    // not a real window
    virtual bool IsWindow() const { return true; }
    // no base class
    virtual bool WantsSubclass() const { return true; }
    virtual bool IsLicensed() const;
};

#endif // AUITOOLBARTOPLEVEL_H
