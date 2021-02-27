#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "clThemedChoice.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"

clThemedChoice::clThemedChoice() {}

clThemedChoice::~clThemedChoice()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedChoice::OnThemeChanged, this);
}

bool clThemedChoice::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                            const wxArrayString& choices, long style, const wxValidator& validator,
                            const wxString& name)

{
    bool res = clChoice::Create(parent, id, pos, size, choices, 0, validator, name);
    if(res) {
        EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedChoice::OnThemeChanged, this);
        ApplyTheme();
    }
    return res;
}

clThemedChoice::clThemedChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                               const wxArrayString& choices, long style, const wxValidator& validator,
                               const wxString& name)
    : clChoice(parent, id, pos, size, choices, style, validator, name)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedChoice::OnThemeChanged, this);
    ApplyTheme();
}

void clThemedChoice::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedChoice::ApplyTheme()
{
#if !wxUSE_NATIVE_CHOICE
    clColours colours;
    colours.InitFromColour(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    SetColours(colours);
#endif
}
