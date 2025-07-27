#ifndef RIBBONPAGEWRAPPER_H
#define RIBBONPAGEWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonPageWrapper : public wxcWidget
{
    bool m_selected;

public:
    RibbonPageWrapper();
    ~RibbonPageWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DoGenerateCppCtorCode_End() const override;
    void Select(bool b);
    bool IsSelected() const;
};

#endif // RIBBONPAGEWRAPPER_H
