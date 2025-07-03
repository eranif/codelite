#ifndef AUIMANAGERWRAPPER_H
#define AUIMANAGERWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class AuiManagerWrapper : public wxcWidget
{
public:
    AuiManagerWrapper();
    ~AuiManagerWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString CppDtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsValidParent() const override { return false; }
    bool IsWxWindow() const override { return false; }
};

#endif // AUIMANAGERWRAPPER_H
