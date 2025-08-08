#ifndef RIBBONTOOLSEPARATOR_H
#define RIBBONTOOLSEPARATOR_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonToolSeparator : public wxcWidget
{
public:
    RibbonToolSeparator();
    ~RibbonToolSeparator() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // RIBBONTOOLSEPARATOR_H
