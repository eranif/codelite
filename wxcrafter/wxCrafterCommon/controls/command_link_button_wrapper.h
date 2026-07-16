#ifndef COMMANDLINKBUTTONWRAPPER_H
#define COMMANDLINKBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class CommandLinkButtonWrapper : public wxcWidget
{
public:
    CommandLinkButtonWrapper();
    ~CommandLinkButtonWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // COMMANDLINKBUTTONWRAPPER_H
