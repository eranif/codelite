#include "clThemedButton.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "cl_config.h"

clThemedButton::clThemedButton() {}

bool clThemedButton::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
                            const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    bool res = clButton::Create(parent, id, label, pos, size, style, validator, name);
    if(res) {
        EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedButton::OnThemeChanged, this);
        ApplyTheme();
    }

    return res;
}

clThemedButton::clThemedButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
                               const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    : clButton(parent, id, label, pos, size, style, validator, name)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedButton::OnThemeChanged, this);
    ApplyTheme();
}

clThemedButton::~clThemedButton()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedButton::OnThemeChanged, this);
}

void clThemedButton::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedButton::ApplyTheme()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    clColours colours;
    if(lexer->IsDark()) {
        colours.InitFromColour(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    } else {
        colours.InitDefaults();
    }
    wxColour baseColour = colours.GetBgColour();
    bool useCustomColour = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustomColour) {
        baseColour = clConfig::Get().Read("BaseColour", baseColour);
        colours.InitFromColour(baseColour);
    }
    SetColours(colours);
}
