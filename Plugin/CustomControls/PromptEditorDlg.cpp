#include "PromptEditorDlg.hpp"

#include "ColoursAndFontsManager.h"
#include "ai/LLMManager.hpp"
#include "file_logger.h"
#include "globals.h"

PromptEditorDlg::PromptEditorDlg(wxWindow* parent)
    : PromptEditorBaseDlg(parent)
{
    size_t prompts_count = static_cast<size_t>(llm::PromptKind::kMax);
    for (size_t i = 0; i < prompts_count; ++i) {
        llm::PromptKind prompt_kind = static_cast<llm::PromptKind>(i);
        wxString prompt = llm::Manager::GetInstance().GetConfig().GetPrompt(prompt_kind);
        wxString label = llm::GetPromptString(prompt_kind);
        m_listbook->AddPage(CreatePage(prompt, prompt_kind), label, i == 0);
    }

    SendSizeEvent();
    ::clSetDialogBestSizeAndPosition(this);
}

PromptEditorDlg::~PromptEditorDlg() {}

wxStyledTextCtrl* PromptEditorDlg::CreatePage(const wxString& content, llm::PromptKind prompt_kind)
{
    wxStyledTextCtrl* ctrl = new wxStyledTextCtrl(m_listbook);
    ctrl->SetText(content);
    ctrl->SetSavePoint();
    ctrl->SetClientData(reinterpret_cast<void*>(static_cast<wxUIntPtr>(prompt_kind)));
    auto lexer = ColoursAndFontsManager::Get().GetLexer("markdown");
    if (lexer) {
        lexer->Apply(ctrl);
    }
    return ctrl;
}

void PromptEditorDlg::OnSave(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clDEBUG() << "Saving prompts" << endl;
    for (size_t i = 0; i < m_listbook->GetPageCount(); ++i) {
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(m_listbook->GetPage(i));
        if (!ctrl) {
            continue;
        }
        ctrl->SetSavePoint();
        wxUIntPtr prompt_kind = reinterpret_cast<wxUIntPtr>(ctrl->GetClientData());
        llm::PromptKind k = static_cast<llm::PromptKind>(prompt_kind);
        clDEBUG() << "Saving prompt:" << llm::GetPromptString(k) << ":" << ctrl->GetText() << endl;
        llm::Manager::GetInstance().GetConfig().SetPrompt(k, ctrl->GetText());
    }
    llm::Manager::GetInstance().GetConfig().Save();
}

void PromptEditorDlg::OnSaveUI(wxUpdateUIEvent& event)
{
    for (size_t i = 0; i < m_listbook->GetPageCount(); ++i) {
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(m_listbook->GetPage(i));
        if (!ctrl) {
            continue;
        }
        if (ctrl->GetModify()) {
            event.Enable(true);
            return;
        }
    }

    event.Enable(false);
}
