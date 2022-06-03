#include "clThemedComboBox.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"

clThemedComboBox::clThemedComboBox() {}

clThemedComboBox::~clThemedComboBox()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedComboBox::OnThemeChanged, this);
}

bool clThemedComboBox::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                              const wxSize& size, const wxArrayString& choices, long style,
                              const wxValidator& validator, const wxString& name)

{
    bool res = clComboBox::Create(parent, id, value, pos, size, choices, style, validator, name);
    if(res) {
        EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedComboBox::OnThemeChanged, this);
        ApplyTheme();
    }
    return res;
}

clThemedComboBox::clThemedComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                                   const wxSize& size, const wxArrayString& choices, long style,
                                   const wxValidator& validator, const wxString& name)
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

void clThemedComboBox::ApplyTheme()
{
    auto parent = wxGetTopLevelParent(this);
    if(!parent) {
        return;
    }

    auto top_frame = dynamic_cast<wxFrame*>(parent);
    if(top_frame && top_frame == EventNotifier::Get()->TopFrame()) {
        // we are placed inside our main frame
        SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
#ifdef __WXMSW__
        GetButton()->SetForegroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
        GetButton()->SetBackgroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
#endif
    }
}
