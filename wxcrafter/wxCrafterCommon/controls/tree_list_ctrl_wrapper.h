#ifndef TREELISTCTRLWRAPPER_H
#define TREELISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TreeListCtrlWrapper : public wxcWidget
{
public:
    TreeListCtrlWrapper();
    ~TreeListCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // TREELISTCTRLWRAPPER_H
