#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "clThemedComboBox.hpp"
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
    //GetButton()->SetColours(colours);
}
