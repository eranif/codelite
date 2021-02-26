#ifndef CLTHEMEDCTRL_HPP
#define CLTHEMEDCTRL_HPP

#include "ColoursAndFontsManager.h"
#include "clColours.h"
#include "clSystemSettings.h"
#include "lexer_configuration.h"
namespace cl
{
template <typename T> void ApplyTheme(T* ctrl)
{
    clColours colours;
    wxColour baseColour = colours.GetBgColour();
    bool useCustomColour = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustomColour) {
        baseColour = clConfig::Get().Read("BaseColour", baseColour);
    } else {
        // we use the *native* background colour (notice that we are using wxSystemSettings)
        baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    }
    colours.InitFromColour(baseColour);

    // Set the built-in search colours
    wxColour highlightColur = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    wxColour textColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    colours.SetMatchedItemBgText(highlightColur);
    colours.SetMatchedItemText(textColour);

    // When not using custom colours, use system defaults
    if(!useCustomColour) {
        wxColour selColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        colours.SetSelItemBgColour(selColour);
        colours.SetSelItemTextColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        colours.SetSelItemBgColourNoFocus(selColour.ChangeLightness(150));
        colours.SetSelItemTextColourNoFocus(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    }
    ctrl->SetColours(colours);
}

} // namespace cl
#endif // CLTHEMEDCTRL_HPP
