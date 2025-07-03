#ifndef TOOLBARBASEWRAPPER_H
#define TOOLBARBASEWRAPPER_H

#include "AuiToolbarWrapperBase.h"
#include "allocator_mgr.h"
#include "wxc_widget.h" // Base class: WrapperBase

/// Base class for wxToolBar and wxAuiToolBar
class ToolbarBaseWrapper : public wxcWidget
{
public:
    explicit ToolbarBaseWrapper(int type);
    ~ToolbarBaseWrapper() override = default;

public:
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

    virtual wxString DesignerXRC() const;
};

/// wxToolBar
class ToolbarWrapper : public ToolbarBaseWrapper
{
public:
    ToolbarWrapper()
        : ToolbarBaseWrapper(ID_WXTOOLBAR)
    {
    }
    ~ToolbarWrapper() override = default;
    wxcWidget* Clone() const override { return new ToolbarWrapper(); }
};

/// wxAuiToolBar
class AuiToolbarWrapper : public AuiToolbarWrapperBase, public wxcWidget
{
public:
    AuiToolbarWrapper();
    ~AuiToolbarWrapper() override = default;

    wxcWidget* Clone() const override { return new AuiToolbarWrapper(); }

    wxString DoGenerateClassMember() const override;

    wxString CppCtorCode() const override;
    wxString CppDtorCode() const override;

    wxString GetWxClassName() const override { return wxT("wxAuiToolBar"); }
    void GetIncludeFile(wxArrayString& headers) const override;

    void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // TOOLBARBASEWRAPPER_H
