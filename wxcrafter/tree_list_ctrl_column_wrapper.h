#ifndef TREELISTCTRLCOLUMNWRAPPER_H
#define TREELISTCTRLCOLUMNWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TreeListCtrlColumnWrapper : public wxcWidget
{
public:
    TreeListCtrlColumnWrapper();
    virtual ~TreeListCtrlColumnWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsLicensed() const;
};

#endif // TREELISTCTRLCOLUMNWRAPPER_H
