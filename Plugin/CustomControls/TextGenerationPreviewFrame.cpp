#include "TextGenerationPreviewFrame.hpp"

#include "ColoursAndFontsManager.h"
#include "ai/LLMManager.hpp"
#include "clResult.hpp"
#include "clSTCHelper.hpp"
#include "event_notifier.h"
#include "globals.h"

#include <regex>
#include <wx/regex.h>
#include <wx/richtooltip.h>

namespace
{

/**
 * Checks if a string starts with code block fences (``` with optional language)
 * and removes the opening fence and any text on that line.
 *
 * @param str The input string to process
 * @return A new string with the opening code fence removed, or the original string if no fence found
 */
clStatusOr<wxString> RemoveCodeFencePrefix(const wxString& str)
{
    // Pattern: start of string, three or more backticks, optional language specifier, end of line
    wxRegEx fencePattern(wxT("^```[^\n]*\n"));

    // Check if the pattern is valid
    if (!fencePattern.IsValid()) {
        return StatusInavalidArgument("Invalid regular expression");
    }

    // Check if the string starts with a code fence
    if (fencePattern.Matches(str)) {
        // Create a copy to modify
        wxString result = str;
        // Remove the opening fence line (replace first match only)
        fencePattern.ReplaceFirst(&result, wxEmptyString);
        return result;
    }

    // Return error status if no fence found
    return StatusNotFound();
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

    auto status = RemoveCodeFencePrefix(text);
    if (status.ok()) {
        // Found it, remove the trailing if it exists.
        text = status.value();
        if (text.EndsWith("```")) {
            text.RemoveLast(3);
        }
    }

    ::CopyToClipboard(text);
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

void TextGenerationPreviewFrame::OnKeyDown(wxKeyEvent& event)
{
    // If no selection and user clicked Ctrl-C, copy the entire text
    if (event.GetKeyCode() == 'C' && event.ControlDown() && !m_editor->HasSelection()) {
        wxCommandEvent dummy;
        OnCopy(dummy);
    } else {
        event.Skip();
    }
}
