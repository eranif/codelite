#include "clThemedChoice.h"

#include "clSystemSettings.h"
#include "event_notifier.h"

#if !wxUSE_NATIVE_CHOICE
#include "clColours.h"
#endif

clThemedChoice::~clThemedChoice()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clThemedChoice::OnThemeChanged, this);
}

bool clThemedChoice::Create(wxWindow* parent,
                            wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            const wxArrayString& choices,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)

{
    bool res = clChoice::Create(parent, id, pos, size, choices, 0, validator, name);
    if (res) {
        EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clThemedChoice::OnThemeChanged, this);
        ApplyTheme();
    }
    return res;
}

clThemedChoice::clThemedChoice(wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               const wxArrayString& choices,
                               long style,
                               const wxValidator& validator,
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
    colours.InitDefaults();
    SetColours(colours);
#endif
}
