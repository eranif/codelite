#ifndef SEARCHCTRLWRAPPER_H
#define SEARCHCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class SearchCtrlWrapper : public wxcWidget
{

public:
    SearchCtrlWrapper();
    virtual ~SearchCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // SEARCHCTRLWRAPPER_H
