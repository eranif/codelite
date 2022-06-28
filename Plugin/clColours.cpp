#include "clColours.h"

#include "clSystemSettings.h"
#include "drawingutils.h"

#include <wx/gdicmn.h>
#include <wx/settings.h>

#ifdef __WXOSX__
#define FACTOR_ALTROW 103
#define FACTOR_SEL_BG_COLOUR_NOFOCUS 110
#else
#define FACTOR_ALTROW_LIGHT 98
#define FACTOR_ALTROW_DARK 102
#define FACTOR_SEL_BG_COLOUR_NOFOCUS 145
#endif

clColours::clColours() {}

void clColours::InitDefaults()
{
    InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
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

void clColours::InitFromColour(const wxColour& baseColour)
{
#ifdef __WXMAC__
    bool is_dark = DrawingUtils::IsDark(clSystemSettings::GetDefaultPanelColour());
    bgColour = clSystemSettings::GetDefaultPanelColour().ChangeLightness(
        is_dark ? 110 : 90);                                          // background colour for the control
    itemBgColour = clSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX); // item bg colour
    itemTextColour = clSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
    hoverBgColour = itemBgColour.ChangeLightness(90);
    selItemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    selItemTextColourNoFocus = selItemTextColour; // item bg colour
    selItemBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    selItemBgColourNoFocus = selItemBgColour;
    buttonColour = itemTextColour;    // expand/collapse button colour
    selbuttonColour = itemTextColour; // The colour of the button ("Expand") when on a selected row
    alternateColour = bgColour.ChangeLightness(105);
    headerBgColour = bgColour;
    darkBorderColour = borderColour = headerVBorderColour = headerHBorderColour =
        bgColour.ChangeLightness(80); // Header background colour
    matchedItemText = *wxBLACK;       // Text colour for matched item (need the style wxTR_ENABLE_SEARCH)
    matchedItemBgText = *wxYELLOW;
    fillColour = bgColour;                                         // fill colour
    grayText = clSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT); // Gray text
#else
    if(!baseColour.IsOk()) {
        InitDefaults();
        return;
    }
    bool is_dark = DrawingUtils::IsDark(baseColour);
    bool is_light = !is_dark;
    bgColour = baseColour;
    itemTextColour = is_light ? wxColour("#212121") : wxColour("#FDFEFE");
    if(is_light) {
        alternateColour = bgColour.ChangeLightness(FACTOR_ALTROW_LIGHT);
        hoverBgColour = bgColour.ChangeLightness(110);
        headerBgColour = bgColour.ChangeLightness(96);
        headerHBorderColour = headerBgColour.ChangeLightness(90);
        headerVBorderColour = headerBgColour.ChangeLightness(90);

        selItemTextColour = itemTextColour;
        selItemBgColour = wxColour("#BDD8F2").ChangeLightness(130);

        selItemTextColourNoFocus = selItemTextColour;
        selItemBgColourNoFocus = selItemBgColour.ChangeLightness(130);

        selbuttonColour = selItemTextColour.ChangeLightness(120);
        buttonColour = itemTextColour.ChangeLightness(120);
        grayText = itemTextColour.ChangeLightness(150);
    } else {
        alternateColour = bgColour.ChangeLightness(FACTOR_ALTROW_DARK);
        hoverBgColour = bgColour.ChangeLightness(90);
        headerBgColour = bgColour.ChangeLightness(105);
        headerHBorderColour = headerBgColour.ChangeLightness(112);
        headerVBorderColour = headerBgColour.ChangeLightness(112);
        selItemTextColour = itemTextColour;
        selbuttonColour = selItemTextColour.ChangeLightness(80);
        buttonColour = itemTextColour.ChangeLightness(80);
        grayText = itemTextColour.ChangeLightness(50);

        selItemBgColour = bgColour.ChangeLightness(120);
        selItemTextColourNoFocus = itemTextColour;
        selItemBgColourNoFocus = bgColour.ChangeLightness(110);
    }
    itemBgColour = bgColour;
    matchedItemBgText = wxColour("#8BC34A");
    matchedItemText = wxColour("#FDFEFE");
    fillColour = bgColour;
    borderColour = is_light ? bgColour.ChangeLightness(70) : bgColour.ChangeLightness(110);
    darkBorderColour = is_light ? bgColour.ChangeLightness(30) : bgColour.ChangeLightness(150);
#endif
}
