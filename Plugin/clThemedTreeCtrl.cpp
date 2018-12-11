#include "ColoursAndFontsManager.h"
#include "clThemedTreeCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include <clColours.h>
#include <wx/settings.h>
#include "cl_config.h"

#ifdef __WXMSW__
#define TREE_STYLE wxTR_ROW_LINES | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE
#else
#define TREE_STYLE wxTR_ROW_LINES | wxTR_ENABLE_SEARCH | wxBORDER_THEME
#endif

clThemedTreeCtrl::clThemedTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clTreeCtrl(parent, id, pos, size, style | TREE_STYLE)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
    ApplyTheme();
    SetNativeTheme(true);
    m_keyboard.reset(new clTreeKeyboardInput(this));
}

bool clThemedTreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!clTreeCtrl::Create(parent, id, pos, size, style | TREE_STYLE)) { return false; }
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedTreeCtrl::OnThemeChanged, this);
    ApplyTheme();
    SetNativeTheme(true);
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
        colours.InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    } else {
        colours.InitDefaults();
    }
    wxColour baseColour = colours.GetBgColour();
    bool useCustomColour = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustomColour) {
        baseColour = clConfig::Get().Read("BaseColour", baseColour);
        colours.InitFromColour(baseColour);
    }
    wxColour highlightColur = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    wxColour textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    colours.SetMatchedItemBgText(highlightColur);
    colours.SetMatchedItemText(textColour);
    colours.SetSelItemBgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
#ifdef __WXOSX__
    colours.SetSelItemBgColourNoFocus(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
#else
    colours.SetSelItemBgColourNoFocus(colours.GetSelItemBgColour().ChangeLightness(110));
#endif

#ifdef __WXGTK__
    if(!colours.IsLightTheme()) { colours.SetAlternateColour(colours.GetBgColour()); }
#endif
    SetColours(colours);
}
