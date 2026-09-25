#include "SystemPromptDialog.hpp"

#include "ColoursAndFontsManager.h"
#include "globals.h"

SystemPromptDialog::SystemPromptDialog(wxWindow* parent, const wxString& content)
    : SystemPromptDialogBase(parent)
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("markdown");
    if (lexer) {
        lexer->Apply(m_stcPrompt);
    }
    m_stcPrompt->SetValue(content);
    ::clSetDialogBestSizeAndPosition(*this);
    m_stcPrompt->CallAfter(&wxStyledTextCtrl::SetFocus);
}

SystemPromptDialog::~SystemPromptDialog() = default;

void SystemPromptDialog::OnClose(wxCommandEvent& event) { EndModal(wxID_OK); }
