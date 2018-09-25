#include "ColoursAndFontsManager.h"
#include "clThemedListCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "lexer_configuration.h"

static void ApplyTheme(clThemedListCtrl* ctrl)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    const StyleProperty& defaultStyle = lexer->GetProperty(0);
    clColours colours;
    if(lexer->IsDark()) {
        colours.InitDefaults();
        wxColour bgColour = colours.GetBgColour();
        wxColour oddLineColour = bgColour.ChangeLightness(97);
        colours.SetBgColour(bgColour);
        colours.SetAlternateColourEven(bgColour);
        colours.SetAlternateColourOdd(oddLineColour);
    } else {
        colours.InitDefaults();
        wxColour bgColour = defaultStyle.GetBgColour(); // Use the current style background colour
        wxColour oddLineColour = bgColour.ChangeLightness(97);
        colours.SetBgColour(bgColour);
        colours.SetAlternateColourEven(bgColour);
        colours.SetAlternateColourOdd(oddLineColour);
    }
    ctrl->SetColours(colours);
}

clThemedListCtrl::clThemedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clDataViewListCtrl(parent, id, pos, size, style | wxTR_ROW_LINES | wxBORDER_THEME)
{
    clColours colours;
    colours.InitDefaults();
    SetColours(colours);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrl::OnThemeChanged, this);
#ifdef __WXMSW__
    SetNativeHeader(true);
#endif
    ApplyTheme(this);
}

clThemedListCtrl::~clThemedListCtrl()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrl::OnThemeChanged, this);
}

void clThemedListCtrl::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme(this);
}
