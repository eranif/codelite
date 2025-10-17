#include "TextPreviewDialog.hpp"

#include "ColoursAndFontsManager.h"
#include "globals.h"

#include <wx/richtooltip.h>

TextPreviewDialog::TextPreviewDialog(wxWindow* parent)
    : TextPreviewDialogBase(parent)
{
    auto editor = clGetManager()->GetActiveEditor();
    if (editor) {
        auto lexer = ColoursAndFontsManager::Get().GetLexerForFile(editor->GetRemotePathOrLocal());
        if (lexer) {
            lexer->Apply(m_editor, true);
        }
    }
}

TextPreviewDialog::~TextPreviewDialog() {}

void TextPreviewDialog::OnCopy(wxCommandEvent& event)
{
    event.Skip();

    ::CopyToClipboard(m_editor->GetText());
    wxRichToolTip tooltip(_("Text Copied!"), _("The preview text has been copied to the clipboard"));
    tooltip.SetTimeout(1000);
    tooltip.ShowFor(m_button_copy);
}

void TextPreviewDialog::OnCopyUI(wxUpdateUIEvent& event) {}

void TextPreviewDialog::SetPreviewText(const wxString& text) { m_editor->SetText(text); }
