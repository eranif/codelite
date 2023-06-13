#include "clBuiltinTerminalPane.hpp"

#include "ColoursAndFontsManager.h"
#include "FontUtils.hpp"
#include "TextView.h"
#include "event_notifier.h"

#include <wx/sizer.h>

clBuiltinTerminalPane::clBuiltinTerminalPane(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_terminal = new wxTerminalCtrl(this);
    GetSizer()->Add(m_terminal, wxSizerFlags().Expand().Proportion(1));
    GetSizer()->Fit(this);
    UpdateTextAttributes();
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

clBuiltinTerminalPane::~clBuiltinTerminalPane()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBuiltinTerminalPane::OnWorkspaceLoaded, this);
}

void clBuiltinTerminalPane::UpdateTextAttributes()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    auto default_style = lexer->GetProperty(0);
    wxColour bg_colour = default_style.GetBgColour();
    wxColour fg_colour = default_style.GetFgColour();

    wxFont text_font;
    if(default_style.GetFontInfoDesc().empty()) {
        text_font = FontUtils::GetDefaultMonospacedFont();
    } else {
        text_font.SetNativeFontInfo(default_style.GetFontInfoDesc());
    }
    m_terminal->GetView()->SetAttributes(bg_colour, fg_colour, text_font);
    m_terminal->GetView()->ReloadSettings();
}

void clBuiltinTerminalPane::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

void clBuiltinTerminalPane::Focus() { m_terminal->GetView()->GetCtrl()->SetFocus(); }
