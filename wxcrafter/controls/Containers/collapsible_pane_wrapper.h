#ifndef COLLAPSIBLEPANEWRAPPER_H
#define COLLAPSIBLEPANEWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class CollapsiblePaneWrapper : public wxcWidget
{
public:
    CollapsiblePaneWrapper();
    ~CollapsiblePaneWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // COLLAPSIBLEPANEWRAPPER_H
