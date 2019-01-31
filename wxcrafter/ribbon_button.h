#ifndef RIBBONBUTTON_H
#define RIBBONBUTTON_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonButtonBase : public wxcWidget
{
    bool m_isButtonBar;

public:
    RibbonButtonBase(int type);
    virtual ~RibbonButtonBase();

public:
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString GetCppName() const;
    virtual bool UseIdInConnect() const { return true; }
    virtual bool IsLicensed() const { return wxcSettings::Get().IsLicensed(); }
};

// ------------------------------------ concrete types -------------------------
class RibbonButton : public RibbonButtonBase
{
public:
    RibbonButton();
    virtual ~RibbonButton();
    virtual wxcWidget* Clone() const { return new RibbonButton(); }
};

// ------------------------------------ concrete types -------------------------
class RibbonButtonHybrid : public RibbonButtonBase
{
public:
    RibbonButtonHybrid();
    virtual ~RibbonButtonHybrid();
    virtual wxcWidget* Clone() const { return new RibbonButtonHybrid(); }
};

// ------------------------------------ concrete types -------------------------
class RibbonButtonDropdown : public RibbonButtonBase
{
public:
    RibbonButtonDropdown();
    virtual ~RibbonButtonDropdown();
    virtual wxcWidget* Clone() const { return new RibbonButtonDropdown(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonButtonToggle : public RibbonButtonBase
{
public:
    RibbonButtonToggle();
    virtual ~RibbonButtonToggle();
    virtual wxcWidget* Clone() const { return new RibbonButtonToggle(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonTool : public RibbonButtonBase
{
public:
    RibbonTool();
    virtual ~RibbonTool();
    virtual wxcWidget* Clone() const { return new RibbonTool(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonToolToggle : public RibbonButtonBase
{
public:
    RibbonToolToggle();
    virtual ~RibbonToolToggle();
    virtual wxcWidget* Clone() const { return new RibbonToolToggle(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonToolHybrid : public RibbonButtonBase
{
public:
    RibbonToolHybrid();
    virtual ~RibbonToolHybrid();
    virtual wxcWidget* Clone() const { return new RibbonToolHybrid(); }
};

// ------------------------------------ concrete types -------------------------

class RibbonToolDropdown : public RibbonButtonBase
{
public:
    RibbonToolDropdown();
    virtual ~RibbonToolDropdown();
    virtual wxcWidget* Clone() const { return new RibbonToolDropdown(); }
};

#endif // RIBBONBUTTON_H
