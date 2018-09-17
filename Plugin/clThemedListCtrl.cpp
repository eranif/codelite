#include "ColoursAndFontsManager.h"
#include "clThemedListCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "lexer_configuration.h"

clThemedListCtrl::clThemedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clDataViewListCtrl(parent, id, pos, size, style | wxTR_ROW_LINES)
{
    clColours colours;
    colours.InitDefaults();
    SetColours(colours);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrl::OnThemeChanged, this);
}

clThemedListCtrl::~clThemedListCtrl()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrl::OnThemeChanged, this);
}

void clThemedListCtrl::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
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
    SetColours(colours);
}
