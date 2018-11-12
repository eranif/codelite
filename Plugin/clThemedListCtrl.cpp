#include "ColoursAndFontsManager.h"
#include "clThemedListCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "lexer_configuration.h"
#include <wx/settings.h>
#include "cl_config.h"

#ifdef __WXMSW__
#define LIST_STYLE wxDV_ROW_LINES | wxDV_ENABLE_SEARCH | wxBORDER_SIMPLE
#else
#define LIST_STYLE wxDV_ROW_LINES | wxDV_ENABLE_SEARCH | wxBORDER_THEME
#endif

clThemedListCtrl::clThemedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clDataViewListCtrl(parent, id, pos, size, style | LIST_STYLE)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrl::OnThemeChanged, this);
    SetNativeTheme(true);
    ApplyTheme();

    // Enable keyboard search
    m_keyboard.reset(new clTreeKeyboardInput(this));
}

clThemedListCtrl::~clThemedListCtrl()
{
    m_keyboard.reset(nullptr);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrl::OnThemeChanged, this);
}

void clThemedListCtrl::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedListCtrl::ApplyTheme()
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
#if 0
    if(!colours.IsLightTheme()) { colours.SetAlternateColour(colours.GetBgColour()); }
#endif
    this->SetColours(colours);
}
