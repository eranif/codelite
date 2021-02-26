#ifndef CLTHEMEDCTRL_HPP
#define CLTHEMEDCTRL_HPP

#include "ColoursAndFontsManager.h"
#include "clColours.h"
#include "clSystemSettings.h"
#include "drawingutils.h"
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

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }

    // When not using custom colours, use system defaults
    auto& prop = lexer->GetProperty(SEL_TEXT_ATTR_ID);
    wxColour selBgColour = prop.GetBgColour();
    wxColour selTextColour = prop.GetFgColour();
    colours.SetSelItemBgColour(selBgColour);
    colours.SetSelItemTextColour(selTextColour);
    colours.SetSelItemBgColourNoFocus(colours.IsLightTheme() ? selBgColour.ChangeLightness(120)
                                                             : selBgColour.ChangeLightness(80));
    colours.SetSelItemTextColourNoFocus(selTextColour);
    ctrl->SetColours(colours);
}

} // namespace cl
#endif // CLTHEMEDCTRL_HPP
