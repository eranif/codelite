#ifndef TOOLBARITEMWRAPPER_H
#define TOOLBARITEMWRAPPER_H

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
    ~ToolBarItemSpaceWrapper() override = default;

    bool IsEventHandler() const override { return false; }

    wxcWidget* Clone() const override { return new ToolBarItemSpaceWrapper(); }

    wxString CppCtorCode() const override;
};

class ToolBarItemSeparatorWrapper : public ToolBarItemWrapper
{
public:
    ToolBarItemSeparatorWrapper();
    ~ToolBarItemSeparatorWrapper() override = default;

    bool IsEventHandler() const override { return false; }

    wxcWidget* Clone() const override { return new ToolBarItemSeparatorWrapper(); }

    wxString CppCtorCode() const override;
};

class AuiToolBarLabelWrapper : public wxcWidget
{
public:
    AuiToolBarLabelWrapper(int type = ID_WXAUITOOLBARLABEL);
    ~AuiToolBarLabelWrapper() override = default;
    bool IsEventHandler() const override { return false; }
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsToolBarTool() const override { return true; }
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
};

class AuiToolBarItemSpaceWrapper : public ToolBarItemWrapper
{
public:
    AuiToolBarItemSpaceWrapper();
    ~AuiToolBarItemSpaceWrapper() override = default;

    bool IsEventHandler() const override { return false; }
    wxcWidget* Clone() const override { return new AuiToolBarItemSpaceWrapper(); }

    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

class AuiToolBarItemNonStretchSpaceWrapper : public ToolBarItemWrapper
{
public:
    AuiToolBarItemNonStretchSpaceWrapper();
    ~AuiToolBarItemNonStretchSpaceWrapper() override = default;

    bool IsEventHandler() const override { return false; }
    wxcWidget* Clone() const override { return new AuiToolBarItemNonStretchSpaceWrapper(); }

    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // TOOLBARITEMWRAPPER_H
