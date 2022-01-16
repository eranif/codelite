#ifndef RIBBONBARWRAPPER_H
#define RIBBONBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget
class RibbonPageWrapper;

class RibbonBarWrapper : public wxcWidget
{
public:
    RibbonBarWrapper();
    virtual ~RibbonBarWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DoGenerateCppCtorCode_End() const;
    void SetSelection(RibbonPageWrapper* page);
    RibbonPageWrapper* GetSelection() const;
};

#endif // RIBBONBARWRAPPER_H
