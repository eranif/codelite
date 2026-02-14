#include "TextViewerDlg.h"

#include "ColoursAndFontsManager.h"

TextViewerDlg::TextViewerDlg(wxWindow* parent,
                             const wxString& message,
                             const wxString& content,
                             const wxString& content_lexer,
                             const wxString& title)
    : TextViewerBaseDlg(parent)
{
    SetLabel(title);
    m_staticTextMessage->SetLabel(message);

    m_stc->SetText(content);
    m_stc->SetReadOnly(true);

    auto lexer = ColoursAndFontsManager::Get().GetLexer(content_lexer);
    if (lexer) {
        lexer->Apply(m_stc, true);
    }

    m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
}
