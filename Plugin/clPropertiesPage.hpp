#ifndef CLPROPERTIESPAGE_HPP
#define CLPROPERTIESPAGE_HPP

#include <codelite_exports.h>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/panel.h>

class clThemedListCtrl;
class WXDLLIMPEXP_SDK clPropertiesPage : public wxPanel
{
    clThemedListCtrl* m_view = nullptr;

public:
    clPropertiesPage(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~clPropertiesPage();

    void AddProperty(const wxString& label, const wxArrayString& choices, size_t sel = 0);
    void AddProperty(const wxString& label, bool checked);
    void AddProperty(const wxString& label, const wxString& value);
    void AddProperty(const wxString& label, const wxColour& value);
};

#endif // CLPROPERTIESPAGE_HPP
