#include "ColoursAndFontsManager.h"
#include "clThemedTreeCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include <clColours.h>
#include <wx/settings.h>

static void ApplyTheme(clThemedTreeCtrl* tree)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    clColours colours;
    if(lexer->IsDark()) {
        colours.InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    } else {
        colours.InitDefaults();
    }
    tree->SetColours(colours);
}

clThemedTreeCtrl::clThemedTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clTreeCtrl(parent, id, pos, size, style | wxTR_ROW_LINES)
{
    clColours colours;
    colours.InitDefaults();
    SetColours(colours);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
    ApplyTheme(this);
}

bool clThemedTreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!clTreeCtrl::Create(parent, id, pos, size, style)) { return false; }
    clColours colours;
    colours.InitDefaults();
    SetColours(colours);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
    ApplyTheme(this);
    return true;
}

clThemedTreeCtrl::clThemedTreeCtrl() {}

clThemedTreeCtrl::~clThemedTreeCtrl()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
}

void clThemedTreeCtrl::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme(this);
}
