#include "PromptEditorDlg.hpp"

#include "ColoursAndFontsManager.h"
#include "MarkdownStyler.hpp"
#include "ai/LLMManager.hpp"
#include "file_logger.h"
#include "globals.h"

struct PromptData : public wxClientData {
    llm::PromptKind prompt_kind;

    PromptData(llm::PromptKind kind)
        : prompt_kind{kind}
    {
    }
    ~PromptData() override = default;
};

PromptEditorDlg::PromptEditorDlg(wxWindow* parent)
    : PromptEditorBaseDlg(parent)
{
    size_t prompts_count = static_cast<size_t>(llm::PromptKind::kMax);
    for (size_t i = 0; i < prompts_count; ++i) {
        auto ctrl = new wxStyledTextCtrl(m_listbook);
        auto d = new PromptData{static_cast<llm::PromptKind>(i)};
        ctrl->SetClientObject(d);
        wxString prompt = llm::Manager::GetInstance().GetConfig().GetPrompt(d->prompt_kind);
        wxString label = llm::GetPromptString(d->prompt_kind);
        ctrl->SetText(prompt);
        ctrl->SetSavePoint();
        MarkdownStyler styler(ctrl);
        styler.StyleText(true);
        m_listbook->AddPage(ctrl, label, i == 0);
    }

    SendSizeEvent();
    ::clSetDialogBestSizeAndPosition(this);
}

PromptEditorDlg::~PromptEditorDlg() {}

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
        auto d = dynamic_cast<PromptData*>(ctrl->GetClientObject());

        MarkdownStyler styler(ctrl);
        styler.StyleText(true);

        clDEBUG() << "Saving prompt:" << llm::GetPromptString(d->prompt_kind) << ":" << ctrl->GetText() << endl;
        llm::Manager::GetInstance().GetConfig().SetPrompt(d->prompt_kind, ctrl->GetText());
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
