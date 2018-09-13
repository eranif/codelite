#include "clColours.h"
#include <wx/gdicmn.h>
#include <wx/settings.h>

void clColours::InitDefaults()
{
    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    selItemTextColour = wxColour("#FDFEFE");
#ifdef __WXOSX__
    selItemBgColour = wxColour("rgb(149,169,176)");
    bgColour = wxColour("rgb(215,223,225)");
    alternateColourEven = bgColour;
    alternateColourOdd = bgColour.ChangeLightness(120);
#else
    selItemBgColour = wxColour("#79BAEC");
    bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    alternateColourEven = bgColour;
    alternateColourOdd = bgColour.ChangeLightness(95);
#endif
    selbuttonColour = selItemTextColour;
    buttonColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    hoverBgColour = wxColour("rgb(219,221,224)");
    scrolBarButton = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    itemBgColour = bgColour;
    headerBgColour = wxColour("#FBFCFC");
    headerHBorderColour = wxColour("#D0D3D4");
    headerVBorderColour = wxColour("#D0D3D4");
    selItemBgColourNoFocus = wxColour("#BDC3C7");
}

void clColours::InitDarkDefaults()
{
    bgColour = wxColour("#5F6A6A");
    itemTextColour = wxColour("#F4F6F6");
    selItemTextColour = *wxWHITE;
    selItemBgColour = wxColour("#6D7B8D");
    selbuttonColour = selItemTextColour;
    buttonColour = itemTextColour;
    hoverBgColour = wxColour("#717D7E");
    itemBgColour = bgColour;
    scrolBarButton = selItemBgColour;
    alternateColourEven = bgColour;
#ifdef __WXOSX__
    alternateColourOdd = bgColour.ChangeLightness(90);
#else
    alternateColourOdd = bgColour.ChangeLightness(95);
#endif
    headerBgColour = wxColour("#4D5656");
    headerHBorderColour = wxColour("#839192");
    headerVBorderColour = wxColour("#BFC9CA");
    selItemBgColourNoFocus = wxColour("#707B7C");
}
