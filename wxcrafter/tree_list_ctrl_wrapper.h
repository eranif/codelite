#ifndef TREELISTCTRLWRAPPER_H
#define TREELISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TreeListCtrlWrapper : public wxcWidget
{
public:
    virtual bool IsLicensed() const;
    TreeListCtrlWrapper();
    virtual ~TreeListCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // TREELISTCTRLWRAPPER_H
