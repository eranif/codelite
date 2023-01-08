#include "clColours.h"

#include "clSystemSettings.h"
#include "drawingutils.h"

#include <wx/gdicmn.h>
#include <wx/settings.h>

#ifdef __WXMAC__
#define FACTOR_ALTROW 103
#define FACTOR_SEL_BG_COLOUR_NOFOCUS 110
#define FACTOR_ALTROW_LIGHT 98
#define FACTOR_ALTROW_DARK 102
#else
#define FACTOR_ALTROW_LIGHT 98
#define FACTOR_ALTROW_DARK 102
#define FACTOR_SEL_BG_COLOUR_NOFOCUS 145
#endif

namespace
{
void init_from_colour(clColours* colours, const wxColour& baseColour)
{
    if(!baseColour.IsOk()) {
        colours->InitDefaults();
        return;
    }
    bool is_dark = DrawingUtils::IsDark(baseColour);
    bool is_light = !is_dark;
    colours->bgColour = is_dark ? baseColour.ChangeLightness(107) : baseColour;

    // determine the text colour
    colours->itemTextColour =
        is_light ? wxColour(*wxBLACK).ChangeLightness(110) : wxColour(*wxWHITE).ChangeLightness(90);

    if(is_light) {
        colours->alternateColour = colours->bgColour.ChangeLightness(FACTOR_ALTROW_LIGHT);
        colours->hoverBgColour = colours->bgColour.ChangeLightness(110);
        colours->headerBgColour = colours->bgColour.ChangeLightness(96);
        colours->headerHBorderColour = colours->headerBgColour.ChangeLightness(90);
        colours->headerVBorderColour = colours->headerBgColour.ChangeLightness(90);

        colours->selItemTextColour = colours->itemTextColour;
        colours->selItemBgColour = wxColour("#BDD8F2").ChangeLightness(130);

        colours->selItemTextColourNoFocus = colours->selItemTextColour;
        colours->selItemBgColourNoFocus = colours->selItemBgColour.ChangeLightness(130);

        colours->selbuttonColour = colours->selItemTextColour.ChangeLightness(120);
        colours->buttonColour = colours->itemTextColour.ChangeLightness(120);
        colours->grayText = colours->itemTextColour.ChangeLightness(150);
    } else {
        colours->alternateColour = colours->bgColour.ChangeLightness(FACTOR_ALTROW_DARK);
        colours->hoverBgColour = colours->bgColour.ChangeLightness(90);
        colours->headerBgColour = colours->bgColour.ChangeLightness(105);
        colours->headerHBorderColour = colours->headerBgColour.ChangeLightness(112);
        colours->headerVBorderColour = colours->headerBgColour.ChangeLightness(112);
        colours->selItemTextColour = colours->itemTextColour;
        colours->selbuttonColour = colours->selItemTextColour.ChangeLightness(80);
        colours->buttonColour = colours->itemTextColour.ChangeLightness(80);
        colours->grayText = colours->itemTextColour.ChangeLightness(50);

        colours->selItemBgColour = colours->bgColour.ChangeLightness(120);
        colours->selItemTextColourNoFocus = colours->itemTextColour;
        colours->selItemBgColourNoFocus = colours->bgColour.ChangeLightness(110);
    }
    colours->itemBgColour = colours->bgColour;
    colours->matchedItemBgText = wxColour("#8BC34A");
    colours->matchedItemText = wxColour("#FDFEFE");
    colours->fillColour = colours->bgColour;
    colours->borderColour = is_light ? colours->bgColour.ChangeLightness(70) : colours->bgColour.ChangeLightness(110);
    colours->darkBorderColour =
        is_light ? colours->bgColour.ChangeLightness(30) : colours->bgColour.ChangeLightness(150);
}
} // namespace
clColours::clColours() {}

void clColours::InitDefaults()
{
    wxColour base_colour = clSystemSettings::Get().IsDark() ? wxColour(*wxBLACK).ChangeLightness(105) : *wxWHITE;
    InitFromColour(base_colour);

    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
    selItemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
}

void clColours::InitDarkDefaults() { InitFromColour(wxColour("#5F6A6A")); }

bool clColours::IsLightTheme() const
{
    if(GetBgColour().IsOk()) {
        return !DrawingUtils::IsDark(GetBgColour());
    } else {
        return !DrawingUtils::IsDark(clSystemSettings::GetDefaultPanelColour());
    }
}

void clColours::FromLexer(LexerConf::Ptr_t lexer)
{
    // sanity
    if(!lexer) {
        InitDefaults();
        return;
    }

    // initialised the colours based on the lexer default bg colour
    auto prop = lexer->GetProperty(0);
    init_from_colour(this, prop.GetBgColour());
    auto prop_selected_text = lexer->GetProperty(SEL_TEXT_ATTR_ID);
    this->selItemBgColour = prop_selected_text.GetBgColour();
    this->selItemTextColour = prop_selected_text.GetFgColour();
}

void clColours::InitFromColour(const wxColour& baseColour) { init_from_colour(this, baseColour); }
