#ifndef RIBBONBUTTON_H
#define RIBBONBUTTON_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonButtonBase : public wxcWidget
{
    bool m_isButtonBar;

public:
    RibbonButtonBase(int type);
    ~RibbonButtonBase() override = default;

public:
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString GetCppName() const override;
    bool UseIdInConnect() const override { return true; }
};

// ------------------------------------ concrete types -------------------------
class RibbonButton : public RibbonButtonBase
{
public:
    RibbonButton();
    ~RibbonButton() override = default;
    wxcWidget* Clone() const override { return new RibbonButton(); }
};

// ------------------------------------ concrete types -------------------------
class RibbonButtonHybrid : public RibbonButtonBase
{
public:
    RibbonButtonHybrid();
    ~RibbonButtonHybrid() = default;
    wxcWidget* Clone() const override { return new RibbonButtonHybrid(); }
};

// ------------------------------------ concrete types -------------------------
class RibbonButtonDropdown : public RibbonButtonBase
{
public:
    RibbonButtonDropdown();
    ~RibbonButtonDropdown() override = default;
    wxcWidget* Clone() const override { return new RibbonButtonDropdown(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonButtonToggle : public RibbonButtonBase
{
public:
    RibbonButtonToggle();
    ~RibbonButtonToggle() override = default;
    wxcWidget* Clone() const override { return new RibbonButtonToggle(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonTool : public RibbonButtonBase
{
public:
    RibbonTool();
    ~RibbonTool() override = default;
    wxcWidget* Clone() const override { return new RibbonTool(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonToolToggle : public RibbonButtonBase
{
public:
    RibbonToolToggle();
    ~RibbonToolToggle() override = default;
    wxcWidget* Clone() const override { return new RibbonToolToggle(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonToolHybrid : public RibbonButtonBase
{
public:
    RibbonToolHybrid();
    ~RibbonToolHybrid() override = default;
    wxcWidget* Clone() const override { return new RibbonToolHybrid(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonToolDropdown : public RibbonButtonBase
{
public:
    RibbonToolDropdown();
    ~RibbonToolDropdown() override = default;
    wxcWidget* Clone() const override { return new RibbonToolDropdown(); }
};

#endif // RIBBONBUTTON_H
