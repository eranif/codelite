#include "clThemedChoice.h"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"

clThemedChoice::clThemedChoice() {}

clThemedChoice::~clThemedChoice()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clThemedChoice::OnThemeChanged, this);
}

bool clThemedChoice::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                            const wxArrayString& choices, long style, const wxValidator& validator,
                            const wxString& name)

{
    bool res = clChoice::Create(parent, id, pos, size, choices, 0, validator, name);
    if(res) {
        EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clThemedChoice::OnThemeChanged, this);
        ApplyTheme();
    }
    return res;
}

clThemedChoice::clThemedChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                               const wxArrayString& choices, long style, const wxValidator& validator,
                               const wxString& name)
{
    bool res = Create(parent, id, pos, size, choices, style, validator, name);
    wxUnusedVar(res);
}

void clThemedChoice::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedChoice::ApplyTheme()
{
#if !wxUSE_NATIVE_CHOICE
    clColours colours;
    colours.InitFromColour(clSystemSettings::GetDefaultPanelColour());
    SetColours(colours);
#endif
}
