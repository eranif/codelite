#include "clComboBox.hpp"

#if wxUSE_NATIVE_COMBOBOX
#include "clThemedTextCtrl.hpp"

#include <wx/button.h>
#include <wx/settings.h>
#include <wx/sizer.h>

clComboBox::clComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                       size_t n, const wxString choices[], long style, const wxValidator& validator,
                       const wxString& name)
    : wxComboBox(parent, id, value, pos, size, n, choices, style | wxTE_RICH2, validator)
{
}

clComboBox::clComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                       const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
    : wxComboBox(parent, id, value, pos, size, choices, style, validator)
{
}

clComboBox::clComboBox() {}

clComboBox::~clComboBox() {}

bool clComboBox::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                        size_t n, const wxString choices[], long style, const wxValidator& validator,
                        const wxString& name)
{
    wxUnusedVar(name);
    return wxComboBox::Create(parent, id, value, pos, size, n, choices, style, validator);
}

bool clComboBox::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                        const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(name);
    return wxComboBox::Create(parent, id, value, pos, size, choices, style, validator);
}
#endif // wxUSE_NATIVE_COMBOBOX
