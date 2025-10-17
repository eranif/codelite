#include "TextGenerationPreviewFrame.hpp"

#include "ColoursAndFontsManager.h"
#include "ai/LLMManager.hpp"
#include "event_notifier.h"
#include "globals.h"

#include <wx/richtooltip.h>

TextGenerationPreviewFrame::TextGenerationPreviewFrame(PreviewKind kind, wxWindow* parent)
    : TextGenerationPreviewFrameBase(parent == nullptr ? EventNotifier::Get()->TopFrame() : parent)
    , m_kind{kind}
{
    wxIconBundle app_icons;
    if (clGetManager()->GetStdIcons()->GetIconBundle("codelite-logo", &app_icons)) {
        SetIcons(app_icons);
    }

    m_markdownStyler = std::make_unique<MarkdownStyler>(m_prompt);
    m_indicator_panel = new IndicatorPanel(m_main_panel, _("Ready"));
    m_main_panel->GetSizer()->Add(m_indicator_panel, wxSizerFlags(0).Expand().Border(wxALL, 0));
    m_editor->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& e) {
        if (e.GetKeyCode() == WXK_ESCAPE) {
            Hide();
        } else {
            e.Skip();
        }
    });

    m_prompt->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& e) {
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

TextGenerationPreviewFrame::~TextGenerationPreviewFrame() {}

void TextGenerationPreviewFrame::OnCopy(wxCommandEvent& event)
{
    event.Skip();

    ::CopyToClipboard(m_editor->GetText());
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
    m_editor->ClearSelections();
    m_editor->EnsureCaretVisible();
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
        wxString prompt = llm::Manager::GetInstance().GetConfig().GetPrompt(llm::PromptKind::kCommentGeneration);
        m_prompt->SetText(prompt);
        m_prompt->SetSavePoint();
        m_markdownStyler->StyleText();

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
    event.Skip();
    Hide();
}

void TextGenerationPreviewFrame::OnSavePrompt(wxCommandEvent& event)
{
    wxUnusedVar(event);
    llm::Manager::GetInstance().GetConfig().SetPrompt(llm::PromptKind::kCommentGeneration, m_prompt->GetText());
    llm::Manager::GetInstance().GetConfig().Save();
    m_prompt->SetSavePoint();
}

void TextGenerationPreviewFrame::OnSavePromptUI(wxUpdateUIEvent& event) { event.Enable(m_prompt->GetModify()); }
