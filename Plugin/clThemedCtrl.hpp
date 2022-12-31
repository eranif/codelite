#ifndef CLTHEMEDCTRL_HPP
#define CLTHEMEDCTRL_HPP

#include "ColoursAndFontsManager.h"
#include "clColours.h"
#include "clSystemSettings.h"
#include "clTreeCtrl.h"
#include "drawingutils.h"
#include "lexer_configuration.h"

#include <wx/settings.h>

namespace cl
{
template <typename T> void ApplyTheme(T* ctrl)
{
    // When not using custom colours, use system defaults
    clColours colours;
    colours.InitDefaults();

    colours.SetSelItemBgColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    colours.SetSelItemTextColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

    wxColour selBgColour = colours.GetSelItemBgColour();
    colours.SetSelItemBgColourNoFocus(colours.IsLightTheme() ? selBgColour.ChangeLightness(120)
                                                             : selBgColour.ChangeLightness(80));
    colours.SetSelItemTextColourNoFocus(colours.GetSelItemTextColour());
    ctrl->SetColours(colours);
}

} // namespace cl
#endif // CLTHEMEDCTRL_HPP
