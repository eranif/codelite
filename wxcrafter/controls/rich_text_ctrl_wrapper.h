#ifndef RICHTEXTCTRLWRAPPER_H
#define RICHTEXTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class RichTextCtrlWrapper : public wxcWidget
{

public:
    RichTextCtrlWrapper();
    virtual ~RichTextCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // RICHTEXTCTRLWRAPPER_H
