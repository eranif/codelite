#ifndef TREECTRLWRAPPER_H
#define TREECTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class TreeCtrlWrapper : public wxcWidget
{

public:
    TreeCtrlWrapper();
    virtual ~TreeCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // TREECTRLWRAPPER_H
