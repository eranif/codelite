#ifndef TREELISTCTRLCOLUMNWRAPPER_H
#define TREELISTCTRLCOLUMNWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TreeListCtrlColumnWrapper : public wxcWidget
{
public:
    TreeListCtrlColumnWrapper();
    ~TreeListCtrlColumnWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // TREELISTCTRLCOLUMNWRAPPER_H
