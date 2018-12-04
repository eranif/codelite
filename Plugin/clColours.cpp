#include "clColours.h"
#include "drawingutils.h"
#include <wx/gdicmn.h>
#include <wx/settings.h>

#ifdef __WXOSX__
#define FACTOR_ALTROW_LIGHT 95
#define FACTOR_ALTROW_DARK 105
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

void clColours::InitDarkDefaults()
{
    InitFromColour(wxColour("#5F6A6A"));
}

bool clColours::IsLightTheme() const { return !DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)); }

void clColours::InitFromColour(const wxColour& baseColour)
{
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
        selItemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        selItemBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        selbuttonColour = selItemTextColour;
        selItemBgColourNoFocus = selItemBgColour.ChangeLightness(FACTOR_SEL_BG_COLOUR_NOFOCUS);
    } else {
        alternateColour = bgColour.ChangeLightness(FACTOR_ALTROW_DARK);
        hoverBgColour = bgColour.ChangeLightness(90);
        headerBgColour = bgColour.ChangeLightness(105);
        headerHBorderColour = headerBgColour.ChangeLightness(112);
        headerVBorderColour = headerBgColour.ChangeLightness(112);
        selItemTextColour = itemTextColour;
        selItemBgColour = bgColour.ChangeLightness(130);
        selbuttonColour = selItemTextColour;
        selItemBgColourNoFocus = bgColour.ChangeLightness(115);
    }
    itemBgColour = bgColour;

    buttonColour = itemTextColour;
    matchedItemBgText = wxColour("#8BC34A");
    matchedItemText = wxColour("#FDFEFE");
    fillColour = bgColour;
    borderColour = is_light ? bgColour.ChangeLightness(70) : bgColour.ChangeLightness(125);
    darkBorderColour = is_light ? bgColour.ChangeLightness(30) : bgColour.ChangeLightness(170);
}
