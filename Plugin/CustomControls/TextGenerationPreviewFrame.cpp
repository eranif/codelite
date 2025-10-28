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
 * @brief Removes Markdown code‑fence markers (the opening ``````` and the closing ```````),
 *        including an optional language identifier that may contain letters, digits,
 *        plus, minus, underscore or any other word‑character.
 *
 * Example:
 *   Input:  "```c++\nint x = 0;\n```"
 *   Output: "int x = 0;\n"
 *
 * @param input The string that potentially contains fenced code blocks.
 * @return A copy of `input` with all fence markers stripped.
 */
std::string StripMarkdownCodeBlocks(const std::string& input)
{
    //   ^```                – opening fence at the start of a line
    //   [\w\+\-]*           – optional language tag (letters, digits, _, +, -)
    //   \s*                 – optional whitespace (including the newline that usually follows)
    //   |                   – OR
    //   ```\s*$             – closing fence at the end of a line (allow trailing spaces)
    //
    // std::regex::multiline makes ^ and $ work per‑line instead of only at the very
    // beginning/end of the whole string.
    const std::regex fencePattern(R"(^```[\w\+\-]*\s*|```[\s]*$)", std::regex::multiline);

    // Replace every match with an empty string → fence markers disappear.
    return std::regex_replace(input, fencePattern, "");
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
