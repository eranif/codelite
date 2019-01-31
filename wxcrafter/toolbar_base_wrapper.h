#ifndef TOOLBARBASEWRAPPER_H
#define TOOLBARBASEWRAPPER_H

#include "AuiToolbarWrapperBase.h"
#include "allocator_mgr.h"
#include "wxc_widget.h" // Base class: WrapperBase

/// Base class for wxToolBar and wxAuiToolBar
class ToolbarBaseWrapper : public wxcWidget
{
public:
    ToolbarBaseWrapper(int type);
    virtual ~ToolbarBaseWrapper();

public:
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
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
    virtual ~ToolbarWrapper() {}
    virtual wxcWidget* Clone() const { return new ToolbarWrapper(); }
};

/// wxAuiToolBar
class AuiToolbarWrapper : public AuiToolbarWrapperBase, public wxcWidget
{
public:
    AuiToolbarWrapper();
    virtual ~AuiToolbarWrapper() {}

    virtual wxcWidget* Clone() const { return new AuiToolbarWrapper(); }

    wxString DoGenerateClassMember() const;

    wxString CppCtorCode() const;
    wxString CppDtorCode() const;

    wxString GetWxClassName() const { return wxT("wxAuiToolBar"); }
    void GetIncludeFile(wxArrayString& headers) const;

    virtual void DoGenerateExtraFunctions(wxString& decl, wxString& impl) const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // TOOLBARBASEWRAPPER_H
