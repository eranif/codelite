#include "PromptEditorDlg.hpp"

#include "MarkdownStyler.hpp"
#include "ai/LLMManager.hpp"
#include "aui/clAuiToolBarArt.h"
#include "file_logger.h"
#include "globals.h"
#include "wxCustomControls.hpp"

#include <wx/msgdlg.h>

struct PromptData : public wxClientData {
    wxString m_label;

    PromptData(const wxString& label)
        : m_label(label)
    {
    }
    ~PromptData() override = default;
};

PromptEditorDlg::PromptEditorDlg(wxWindow* parent)
    : PromptEditorBaseDlg(parent)
{
    auto images = clGetManager()->GetStdIcons();
    clAuiToolBarArt::AddTool(
        m_auibar, wxID_NEW, _("New Prompt"), images->LoadBitmap("file_new"), wxEmptyString, wxITEM_NORMAL);
    clAuiToolBarArt::AddTool(
        m_auibar, wxID_DELETE, _("Delete Prompt"), images->LoadBitmap("minus"), wxEmptyString, wxITEM_NORMAL);

    Bind(wxEVT_MENU, &PromptEditorDlg::OnNew, this, wxID_NEW);
    Bind(wxEVT_MENU, &PromptEditorDlg::OnDelete, this, wxID_DELETE);
    Bind(wxEVT_UPDATE_UI, &PromptEditorDlg::OnDeleteUI, this, wxID_DELETE);
    m_auibar->Realize();

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
    auto& conf = llm::Manager::GetInstance().GetConfig();
    auto all_prompts = conf.GetAllPrompts();
    int page_number{0};
    for (const auto& [label, prompt] : all_prompts) {
        auto ctrl = new wxStyledTextCtrl(m_simpleBook);
        auto d = new PromptData{label};
        ctrl->SetClientObject(d);
        ctrl->SetText(prompt);
        ctrl->SetWrapMode(wxSTC_WRAP_WORD);
        ctrl->SetSavePoint();
        MarkdownStyler styler(ctrl);
        styler.StyleText(true);

        m_simpleBook->AddPage(ctrl, label, page_number == 0);
        wxVector<wxVariant> cols;
        cols.push_back(label);
        m_dvListCtrlPrompts->AppendItem(cols, wxUIntPtr(page_number));
        page_number++;
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
        llm::Manager::GetInstance().GetConfig().AddPrompt(d->m_label, ctrl->GetText());
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

void PromptEditorDlg::SelectLabel(const wxString& label)
{
    for (size_t i = 0; m_dvListCtrlPrompts->GetItemCount(); ++i) {
        wxString item_label = m_dvListCtrlPrompts->GetTextValue(i, 0);
        if (item_label != label) {
            continue;
        }
        int page = static_cast<int>(m_dvListCtrlPrompts->GetItemData(m_dvListCtrlPrompts->RowToItem(i)));
        if (page >= 0 && page < m_simpleBook->GetPageCount()) {
            m_simpleBook->SetSelection(page);
            m_dvListCtrlPrompts->SelectRow(i);
        }
        break;
    }
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
    if (::clMessageBox(
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

void PromptEditorDlg::OnNew(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString label = ::wxGetTextFromUser(_("Enter the new prompt label"), _("New Prompt"));
    if (label.empty()) {
        return;
    }

    auto& conf = llm::Manager::GetInstance().GetConfig();
    if (conf.IsPromptExists(label)) {
        ::clMessageBox(_("A prompt with this name already exists"), "CodeLite", wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    conf.AddPrompt(label, wxEmptyString);
    conf.Save();

    LoadPrompts();
    CallAfter(&PromptEditorDlg::SelectLabel, label);
}

void PromptEditorDlg::OnDelete(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxDataViewItemArray items;
    m_dvListCtrlPrompts->GetSelections(items);
    if (items.size() != 1) {
        return;
    }

    wxString label = m_dvListCtrlPrompts->GetTextValue(m_dvListCtrlPrompts->ItemToRow(items[0]), 0);

    // Seek user confirmation
    wxString message = wxString::Format(_("This will delete the prompt: '%s'\nContinue?"), label);
    if (clMessageBox(message, _("Confirm"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) != wxYES) {
        return;
    }

    llm::Manager::GetInstance().GetConfig().DeletePrompt(label);
    llm::Manager::GetInstance().GetConfig().Save();
    CallAfter(&PromptEditorDlg::LoadPrompts);
}

void PromptEditorDlg::OnDeleteUI(wxUpdateUIEvent& e)
{
    wxDataViewItemArray items;
    m_dvListCtrlPrompts->GetSelections(items);
    if (items.size() != 1) {
        e.Enable(false);
        return;
    }

    wxString label = m_dvListCtrlPrompts->GetTextValue(m_dvListCtrlPrompts->ItemToRow(items[0]), 0);
    e.Enable(!llm::Manager::GetInstance().GetConfig().IsBuiltInPrompt(label));
}
