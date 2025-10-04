#include "clThemedComboBox.hpp"

#include "codelite_events.h"
#include "event_notifier.h"

clThemedComboBox::~clThemedComboBox()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedComboBox::OnThemeChanged, this);
}

bool clThemedComboBox::Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& value,
                              const wxPoint& pos,
                              const wxSize& size,
                              const wxArrayString& choices,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)

{
    bool res = clComboBox::Create(parent, id, value, pos, size, choices, style, validator, name);
    if (res) {
        EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedComboBox::OnThemeChanged, this);
        ApplyTheme();
    }
    return res;
}

clThemedComboBox::clThemedComboBox(wxWindow* parent,
                                   wxWindowID id,
                                   const wxString& value,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   const wxArrayString& choices,
                                   long style,
                                   const wxValidator& validator,
                                   const wxString& name)
    : clComboBox(parent, id, value, pos, size, choices, style, validator, name)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedComboBox::OnThemeChanged, this);
    ApplyTheme();
}

void clThemedComboBox::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedComboBox::ApplyTheme() {}
