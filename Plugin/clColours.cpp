#include "clColours.h"
#include <wx/gdicmn.h>
#include <wx/settings.h>

void clColours::InitDefaults()
{
    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    selItemTextColour = itemTextColour;
    selItemBgColour = wxColour("rgb(199,203,209)");
    buttonColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    hoverBgColour = wxColour("rgb(219,221,224)");
    bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    scrolBarButton = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    scrollBarBgColour = bgColour.ChangeLightness(95); // A bit darker
    itemBgColour = bgColour;
    alternateColourEven = bgColour.ChangeLightness(105);
    alternateColourOdd = bgColour.ChangeLightness(95);
}

void clColours::InitDarkDefaults()
{
    bgColour = wxColour("#1c2833");
    itemTextColour = wxColour("#eaecee");
    selItemTextColour = *wxWHITE;
    selItemBgColour = wxColour("#566573");
    buttonColour = itemTextColour;
    hoverBgColour = wxColour("#2c3e50");
    itemBgColour = bgColour;
    scrolBarButton = selItemBgColour;
    scrollBarBgColour = wxColour("#212f3d"); // A bit darker
    alternateColourEven = bgColour.ChangeLightness(105);
    alternateColourOdd = bgColour.ChangeLightness(95);
}
