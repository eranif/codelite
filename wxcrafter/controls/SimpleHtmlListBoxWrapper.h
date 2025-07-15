#ifndef SIMPLEHTMLLISTBOXWRAPPER_H
#define SIMPLEHTMLLISTBOXWRAPPER_H

#include "wxc_widget.h"

class SimpleHtmlListBoxWrapper : public wxcWidget
{
public:
    SimpleHtmlListBoxWrapper();
    ~SimpleHtmlListBoxWrapper() override = default;

    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override { return new SimpleHtmlListBoxWrapper(); }

};

#endif // SIMPLEHTMLLISTBOXWRAPPER_H
