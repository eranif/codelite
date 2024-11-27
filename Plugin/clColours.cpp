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
    if (!baseColour.IsOk()) {
        colours->InitDefaults();
        return;
    }
    bool is_dark = DrawingUtils::IsDark(baseColour);
    bool is_light = !is_dark;
    colours->bgColour = is_dark ? baseColour : *wxWHITE;

    // determine the text colour
    colours->itemTextColour =
        is_light ? wxColour(*wxBLACK).ChangeLightness(110) : wxColour(*wxWHITE).ChangeLightness(90);

    if (is_light) {
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
    InitFromColour(clSystemSettings::GetDefaultPanelColour());
    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
    selItemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
}

bool clColours::IsLightTheme() const
{
    if (GetBgColour().IsOk()) {
        return !DrawingUtils::IsDark(GetBgColour());
    } else {
        return !DrawingUtils::IsDark(clSystemSettings::GetDefaultPanelColour());
    }
}

void clColours::FromLexer(LexerConf::Ptr_t lexer)
{
    // sanity
    if (!lexer) {
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

wxColour dark_black;
wxColour dark_red;
wxColour dark_green;
wxColour dark_yellow;
wxColour dark_blue;
wxColour dark_magenta;
wxColour dark_cyan;
wxColour dark_grey;
wxColour dark_white;

wxColour black;
wxColour red;
wxColour green;
wxColour yellow;
wxColour blue;
wxColour magenta;
wxColour cyan;
wxColour grey;
wxColour white;

namespace
{
void initialize_base_colours()
{
    static bool initialise = true;

    if (initialise) {
        // dark theme colours
        dark_black = wxColour("#0E1415");
        dark_red = wxColour("#e25d56");
        dark_green = wxColour("#73ca50");
        dark_yellow = wxColour("#e9bf57");
        dark_blue = wxColour("#4a88e4");
        dark_magenta = wxColour("#915caf");
        dark_cyan = wxColour("#23acdd");
        dark_white = wxColour("#f0f0f0");
        dark_grey = "LIGHT GREY";

        // normal colours
        black = wxColour("#000000");
        red = wxColour("#AA3731");
        green = wxColour("#448C27");
        yellow = wxColour("#CB9000");
        blue = wxColour("#325CC0");
        magenta = wxColour("#7A3E9D");
        cyan = wxColour("#0083B2");
        white = wxColour("#BBBBBB");
        grey = "GREY";

        initialise = false;
    }
}
} // namespace

#define RETURN_COLOUR(name)                   \
    initialize_base_colours();                \
    wxColour colour = name;                   \
    if (dark_theme) {                         \
        colour = dark_##name;                 \
    }                                         \
    if (bright) {                             \
        colour = colour.ChangeLightness(110); \
    }                                         \
    return colour

wxColour clColours::Black(bool dark_theme, bool bright) { RETURN_COLOUR(black); }
wxColour clColours::Red(bool dark_theme, bool bright) { RETURN_COLOUR(red); }
wxColour clColours::Green(bool dark_theme, bool bright) { RETURN_COLOUR(green); }
wxColour clColours::Yellow(bool dark_theme, bool bright) { RETURN_COLOUR(yellow); }
wxColour clColours::Blue(bool dark_theme, bool bright) { RETURN_COLOUR(blue); }
wxColour clColours::Magenta(bool dark_theme, bool bright) { RETURN_COLOUR(magenta); }
wxColour clColours::Cyan(bool dark_theme, bool bright) { RETURN_COLOUR(cyan); }
wxColour clColours::Grey(bool dark_theme, bool bright) { RETURN_COLOUR(grey); }
wxColour clColours::White(bool dark_theme, bool bright) { RETURN_COLOUR(white); }