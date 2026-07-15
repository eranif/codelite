#ifndef AUITOOLBARWRAPPERBASE_H
#define AUITOOLBARWRAPPERBASE_H

#include <wx/arrstr.h>
#include <wx/string.h>

class wxcWidget;

class AuiToolbarWrapperBase
{
public:
    AuiToolbarWrapperBase() = default;
    ~AuiToolbarWrapperBase() = default;
    void GenerateExtraFunctions(const wxcWidget* widget, wxString& decl, wxString& impl) const;
    bool HasDropdownWithMenu(const wxcWidget* widget) const;
    wxString GenerateClassMembers(const wxcWidget* widget) const;
    wxString CppDtorCode(const wxcWidget* widget) const;
    void BaseGetIncludeFile(wxArrayString& headers) const;
};

#endif // AUITOOLBARWRAPPERBASE_H
