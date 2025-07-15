#ifndef PROPERTYGRIDWRAPPER_H
#define PROPERTYGRIDWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class PropertyGridWrapper : public wxcWidget
{
public:
    PropertyGridWrapper();
    ~PropertyGridWrapper() override = default;

    bool IsValidParent() const override;
    bool IsWxWindow() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString DoGenerateCppCtorCode_End() const override;
};

#endif // PROPERTYGRIDWRAPPER_H
