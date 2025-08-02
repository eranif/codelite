#ifndef TOOLBARITEMWRAPPER_H
#define TOOLBARITEMWRAPPER_H

#include "allocator_mgr.h"
#include "wxc_widget.h" // Base class: WrapperBase

class ToolBarItemWrapper : public wxcWidget
{
public:
    ToolBarItemWrapper(int type = ID_WXTOOLBARITEM);
    ~ToolBarItemWrapper() override = default;

    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsToolBarTool() const override { return true; }
    void UpdateRegisteredEventsIfNeeded() override;

    void OnPropertiesUpdated(); // Called when a Propertygrid value changes
    bool HasDefaultDropdown() const;
};

class ToolBarItemSpaceWrapper : public ToolBarItemWrapper
{
public:
    ToolBarItemSpaceWrapper();
    virtual ~ToolBarItemSpaceWrapper() {}

    virtual bool IsEventHandler() const { return false; }

    virtual wxcWidget* Clone() const { return new ToolBarItemSpaceWrapper(); }

    virtual wxString CppCtorCode() const;
};

class ToolBarItemSeparatorWrapper : public ToolBarItemWrapper
{
public:
    ToolBarItemSeparatorWrapper();
    virtual ~ToolBarItemSeparatorWrapper();

    virtual bool IsEventHandler() const { return false; }

    virtual wxcWidget* Clone() const { return new ToolBarItemSeparatorWrapper(); }

    virtual wxString CppCtorCode() const;
};

class AuiToolBarLabelWrapper : public wxcWidget
{
public:
    AuiToolBarLabelWrapper(int type = ID_WXAUITOOLBARLABEL);
    virtual ~AuiToolBarLabelWrapper() {}
    virtual bool IsEventHandler() const { return false; }
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsToolBarTool() const { return true; }
    void LoadPropertiesFromXRC(const wxXmlNode* node);
};

class AuiToolBarItemSpaceWrapper : public ToolBarItemWrapper
{
public:
    AuiToolBarItemSpaceWrapper();
    virtual ~AuiToolBarItemSpaceWrapper() {}

    virtual bool IsEventHandler() const { return false; }
    virtual wxcWidget* Clone() const { return new AuiToolBarItemSpaceWrapper(); }

    virtual wxString CppCtorCode() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

class AuiToolBarItemNonStretchSpaceWrapper : public ToolBarItemWrapper
{
public:
    AuiToolBarItemNonStretchSpaceWrapper();
    virtual ~AuiToolBarItemNonStretchSpaceWrapper() {}

    virtual bool IsEventHandler() const { return false; }
    virtual wxcWidget* Clone() const { return new AuiToolBarItemNonStretchSpaceWrapper(); }

    virtual wxString CppCtorCode() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // TOOLBARITEMWRAPPER_H
