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
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }

    // When not using custom colours, use system defaults
    clColours colours;
    wxColour baseColour;
    clTreeCtrl* tree = dynamic_cast<clTreeCtrl*>(ctrl);
    if(tree) {
        baseColour = clSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    } else {
        baseColour = clSystemSettings::GetDefaultPanelColour();
    }

    colours.InitFromColour(baseColour);

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
