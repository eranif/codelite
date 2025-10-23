#include "TextGenerationPreviewFrame.hpp"

#include "ColoursAndFontsManager.h"
#include "ai/LLMManager.hpp"
#include "clSTCHelper.hpp"
#include "event_notifier.h"
#include "globals.h"

#include <regex>
#include <wx/richtooltip.h>

namespace
{
/**
 * @brief Strips markdown code-block markers from a string.
 *
 * This function removes opening markdown code-block markers (```<LANG>) and
 * closing markers (```) from the input string, leaving only the code content.
 *
 * @param input The string containing markdown code blocks
 * @return std::string The string with code-block markers removed
 */
std::string StripMarkdownCodeBlocks(const std::string& input)
{
    // Pattern explanation:
    // ^```[a-zA-Z]*\n?  - matches opening marker: ``` followed by optional language identifier and optional newline
    // ```$              - matches closing marker: ``` at end of line
    std::regex pattern("^```[a-zA-Z]*\\n?|```$", std::regex::multiline);
    return std::regex_replace(input, pattern, "");
}

} // namespace

TextGenerationPreviewFrame::TextGenerationPreviewFrame(PreviewKind kind, wxWindow* parent)
    : TextGenerationPreviewFrameBase(parent == nullptr ? EventNotifier::Get()->TopFrame() : parent)
    , m_kind{kind}
{
    wxIconBundle app_icons;
    if (clGetManager()->GetStdIcons()->GetIconBundle("codelite-logo", &app_icons)) {
        SetIcons(app_icons);
    }

    m_indicator_panel = new IndicatorPanel(m_main_panel, _("Ready"));
    m_main_panel->GetSizer()->Add(m_indicator_panel, wxSizerFlags(0).Expand().Border(wxALL, 0));
    m_editor->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& e) {
        if (e.GetKeyCode() == WXK_ESCAPE) {
            Hide();
        } else {
            e.Skip();
        }
    });

    SendSizeEvent();
    CenterOnParent();
    m_editor->CallAfter(&wxStyledTextCtrl::SetFocus);
}

TextGenerationPreviewFrame::~TextGenerationPreviewFrame()
{
    clDEBUG() << "~TextGenerationPreviewFrame() destructor has been called" << endl;
}

/**
 * @brief Handles the copy event by copying the editor's text content to the clipboard.
 *
 * This method retrieves the trimmed text from the editor, strips any markdown code blocks,
 * copies the processed text to the system clipboard, and displays a tooltip notification
 * to inform the user that the text has been copied successfully.
 *
 * @param event The command event triggered by the copy action
 */
void TextGenerationPreviewFrame::OnCopy(wxCommandEvent& event)
{
    event.Skip();

    wxString text = m_editor->GetText().Trim().Trim(false);

    auto stripped_text = StripMarkdownCodeBlocks(text.ToStdString(wxConvUTF8));
    auto text_to_copy = wxString::FromUTF8(stripped_text);
    text_to_copy.Trim().Trim(false);
    ::CopyToClipboard(text_to_copy);

    wxRichToolTip tooltip(_("Text Copied!"), _("The preview text has been copied to the clipboard"));
    tooltip.SetTimeout(1000);
    tooltip.ShowFor(m_button_copy);
}

void TextGenerationPreviewFrame::OnCopyUI(wxUpdateUIEvent& event) {}

void TextGenerationPreviewFrame::UpdateProgress(const wxString& message) { m_indicator_panel->SetMessage(message); }
void TextGenerationPreviewFrame::StartProgress(const wxString& message)
{
    if (!m_indicator_panel->IsRunning()) {
        m_indicator_panel->Start(message);
    }
}

void TextGenerationPreviewFrame::StopProgress(const wxString& message)
{
    if (m_indicator_panel->IsRunning()) {
        m_indicator_panel->Stop(message);
    }
}

void TextGenerationPreviewFrame::AppendText(const wxString& text)
{
    // Update the commit message
    m_editor->SetInsertionPointEnd();
    m_editor->AppendText(text);
    m_editor->EnsureCaretVisible();
    m_editor->ClearSelections();
    clSTCHelper::SetCaretAt(m_editor, m_editor->GetLastPosition());
}

void TextGenerationPreviewFrame::Reset()
{
    m_indicator_panel->Stop(_("Ready"));
    m_editor->ClearAll();
}

void TextGenerationPreviewFrame::InitialiseFor(PreviewKind kind)
{
    Reset();
    m_kind = kind;
    switch (m_kind) {
    case PreviewKind::kDefault:
        break;
    case PreviewKind::kCommentGeneration: {
        auto editor = clGetManager()->GetActiveEditor();
        if (editor) {
            auto lexer = ColoursAndFontsManager::Get().GetLexerForFile(editor->GetRemotePathOrLocal());
            if (lexer) {
                lexer->Apply(m_editor, true);
            }
        }
    } break;
    }
}

void TextGenerationPreviewFrame::OnClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Hide();
}

void TextGenerationPreviewFrame::OnCloseWindow(wxCloseEvent& event)
{
    wxUnusedVar(event);
    Hide();
}
