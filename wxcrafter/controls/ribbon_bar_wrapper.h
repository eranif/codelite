#ifndef RIBBONBARWRAPPER_H
#define RIBBONBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget
class RibbonPageWrapper;

class RibbonBarWrapper : public wxcWidget
{
public:
    RibbonBarWrapper();
    ~RibbonBarWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DoGenerateCppCtorCode_End() const override;

    void SetSelection(RibbonPageWrapper* page);
    RibbonPageWrapper* GetSelection() const;
};

#endif // RIBBONBARWRAPPER_H
