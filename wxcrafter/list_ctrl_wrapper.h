#ifndef LISTCTRLWRAPPER_H
#define LISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ListCtrlWrapper : public wxcWidget
{

public:
    ListCtrlWrapper();
    virtual ~ListCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // LISTCTRLWRAPPER_H
