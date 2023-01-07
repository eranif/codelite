#ifndef MEDIACTRLWRAPPER_H
#define MEDIACTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class MediaCtrlWrapper : public wxcWidget
{
public:
    MediaCtrlWrapper();
    virtual ~MediaCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // MEDIACTRLWRAPPER_H
