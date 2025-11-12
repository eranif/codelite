#include "PromptEditorDlg.hpp"

#include "MarkdownStyler.hpp"
#include "ai/LLMManager.hpp"
#include "file_logger.h"
#include "globals.h"

#include <wx/msgdlg.h>

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
    LoadPrompts();
    SendSizeEvent();
    m_splitter->SetSashPosition(FromDIP(250));
    ::clSetDialogBestSizeAndPosition(this);
}

PromptEditorDlg::~PromptEditorDlg() {}

void PromptEditorDlg::LoadPrompts()
{
    m_dvListCtrlPrompts->DeleteAllItems();
    m_simpleBook->DeleteAllPages();
    size_t prompts_count = static_cast<size_t>(llm::PromptKind::kMax);
    for (size_t i = 0; i < prompts_count; ++i) {
        auto ctrl = new wxStyledTextCtrl(m_simpleBook);
        auto d = new PromptData{static_cast<llm::PromptKind>(i)};
        ctrl->SetClientObject(d);
        wxString prompt = llm::Manager::GetInstance().GetConfig().GetPrompt(d->prompt_kind);
        wxString label = llm::PromptKindToString(d->prompt_kind);
        ctrl->SetText(prompt);
        ctrl->SetWrapMode(wxSTC_WRAP_WORD);
        ctrl->SetSavePoint();
        MarkdownStyler styler(ctrl);
        styler.StyleText(true);
        m_simpleBook->AddPage(ctrl, label, i == 0);
        wxVector<wxVariant> cols;
        cols.push_back(label);
        m_dvListCtrlPrompts->AppendItem(cols, static_cast<wxUIntPtr>(i));
    }

    CallAfter(&PromptEditorDlg::SelectRow, 0);
}

void PromptEditorDlg::OnSave(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clDEBUG() << "Saving prompts" << endl;
    for (size_t i = 0; i < m_simpleBook->GetPageCount(); ++i) {
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(m_simpleBook->GetPage(i));
        if (!ctrl) {
            continue;
        }
        ctrl->SetSavePoint();
        auto d = dynamic_cast<PromptData*>(ctrl->GetClientObject());

        MarkdownStyler styler(ctrl);
        styler.StyleText(true);

        clDEBUG() << "Saving prompt:" << llm::PromptKindToString(d->prompt_kind) << ":" << ctrl->GetText() << endl;
        llm::Manager::GetInstance().GetConfig().SetPrompt(d->prompt_kind, ctrl->GetText());
    }
    llm::Manager::GetInstance().GetConfig().Save();
}

void PromptEditorDlg::OnSaveUI(wxUpdateUIEvent& event)
{
    for (size_t i = 0; i < m_simpleBook->GetPageCount(); ++i) {
        wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(m_simpleBook->GetPage(i));
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

void PromptEditorDlg::OnPromptChanged(wxDataViewEvent& event)
{
    auto item = m_dvListCtrlPrompts->GetSelection();
    CHECK_ITEM_RET(item);
    auto row = static_cast<size_t>(m_dvListCtrlPrompts->GetItemData(item));
    SelectRow(row);
}

void PromptEditorDlg::SelectRow(size_t row)
{
    if (m_simpleBook->GetPageCount() == 0 || row >= m_simpleBook->GetPageCount()) {
        return;
    }

    m_simpleBook->SetSelection(row);
    m_dvListCtrlPrompts->SelectRow(row);
}

void PromptEditorDlg::OnDefaults(wxCommandEvent& event)
{
    if (::wxMessageBox(
            _("This operation discard any changes you have done and restore it to their default values. Continue?"),
            "CodeLite",
            wxOK | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCANCEL_DEFAULT | wxICON_WARNING) != wxYES) {
        return;
    }

    wxUnusedVar(event);
    llm::Manager::GetInstance().GetConfig().ResetPrompts();
    llm::Manager::GetInstance().GetConfig().Save();
    LoadPrompts();
}
