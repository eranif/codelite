#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "clThemedTreeCtrl.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include <clColours.h>
#include <wx/settings.h>

#ifdef __WXMSW__
#define TREE_STYLE wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE | wxTR_ROW_LINES
#else
#define TREE_STYLE wxTR_ENABLE_SEARCH | wxBORDER_DEFAULT | wxTR_ROW_LINES
#endif

clThemedTreeCtrl::clThemedTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clTreeCtrl(parent, id, pos, size, style | TREE_STYLE)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
    ApplyTheme();
    m_keyboard.reset(new clTreeKeyboardInput(this));
}

bool clThemedTreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!clTreeCtrl::Create(parent, id, pos, size, style | TREE_STYLE)) {
        return false;
    }
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
    ApplyTheme();
    m_keyboard.reset(new clTreeKeyboardInput(this));
    return true;
}

clThemedTreeCtrl::clThemedTreeCtrl() {}

clThemedTreeCtrl::~clThemedTreeCtrl()
{
    m_keyboard.reset(nullptr);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
}

void clThemedTreeCtrl::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedTreeCtrl::ApplyTheme()
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

    // Set the built-in search colours
    wxColour highlightColur = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    wxColour textColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    colours.SetMatchedItemBgText(highlightColur);
    colours.SetMatchedItemText(textColour);

    // When not using custom colours, use system defaults
    if(!useCustomColour) {
        wxColour selColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        colours.SetSelItemBgColour(selColour);
        colours.SetSelItemTextColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        colours.SetSelItemBgColourNoFocus(selColour.ChangeLightness(150));
        colours.SetSelItemTextColourNoFocus(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    }
    // When using custom bg colour, don't use native drawings
    this->SetColours(colours);
}
