#ifndef CLTHEMEDCOMBOBOX_HPP
#define CLTHEMEDCOMBOBOX_HPP

#include "clComboBox.hpp"

class WXDLLIMPEXP_SDK clThemedComboBox : public clComboBox
{
protected:
    void OnThemeChanged(wxCommandEvent& event);
    void ApplyTheme();

public:
    clThemedComboBox();
    clThemedComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                     const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxChoiceNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);
    virtual ~clThemedComboBox();
};

#endif // CLTHEMEDCOMBOBOX_HPP
