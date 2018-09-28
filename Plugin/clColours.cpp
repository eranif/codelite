#include "clColours.h"
#include "drawingutils.h"
#include <wx/gdicmn.h>
#include <wx/settings.h>

clColours::clColours() {}

void clColours::InitDefaults()
{
    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
    selItemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    selItemBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    if(IsLightTheme()) {
        alternateColour = bgColour.ChangeLightness(95);
        hoverBgColour = bgColour.ChangeLightness(110);
        headerBgColour = bgColour.ChangeLightness(113);
        headerHBorderColour = alternateColour;
        headerVBorderColour = alternateColour;
    } else {
        alternateColour = bgColour.ChangeLightness(95);
        hoverBgColour = bgColour.ChangeLightness(90);
        headerBgColour = bgColour.ChangeLightness(113);
        headerHBorderColour = wxColour("#D0D3D4");
        headerVBorderColour = wxColour("#D0D3D4");
    }

    selbuttonColour = selItemTextColour;
    buttonColour = itemTextColour;
    itemBgColour = bgColour;
    selItemBgColourNoFocus = wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION);
    matchedItemBgText = wxColour("#FF6F00");
    matchedItemText = wxColour("#17202A");
    fillColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    borderColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    darkBorderColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
}

void clColours::InitDarkDefaults()
{
    bgColour = wxColour("#5F6A6A");
    itemTextColour = wxColour("#F4F6F6");
    selItemTextColour = wxColour("#FDFEFE");
    selItemBgColour = wxColour("#6D7B8D");
    selbuttonColour = selItemTextColour;
    buttonColour = itemTextColour;
    hoverBgColour = wxColour("#717D7E");
    itemBgColour = bgColour;
    alternateColour = bgColour.ChangeLightness(95);
    headerBgColour = wxColour("#4D5656");
    headerHBorderColour = wxColour("#839192");
    headerVBorderColour = wxColour("#BFC9CA");
    selItemBgColourNoFocus = wxColour("#707B7C");
    matchedItemBgText = wxColour("#8BC34A");
    matchedItemText = wxColour("#FDFEFE");
    borderColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    fillColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    darkBorderColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
}

bool clColours::IsLightTheme() const { return !DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)); }
