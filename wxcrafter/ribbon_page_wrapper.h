#ifndef RIBBONPAGEWRAPPER_H
#define RIBBONPAGEWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonPageWrapper : public wxcWidget
{
    bool m_selected;

public:
    RibbonPageWrapper();
    virtual ~RibbonPageWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DoGenerateCppCtorCode_End() const;
    void Select(bool b);
    bool IsSelected() const;
    virtual bool IsLicensed() const { return wxcSettings::Get().IsLicensed(); }
};

#endif // RIBBONPAGEWRAPPER_H
