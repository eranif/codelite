#ifndef COMMANDLINKBUTTONWRAPPER_H
#define COMMANDLINKBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class CommandLinkButtonWrapper : public wxcWidget
{
public:
    CommandLinkButtonWrapper();
    virtual ~CommandLinkButtonWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsLicensed() const;
};

#endif // COMMANDLINKBUTTONWRAPPER_H
