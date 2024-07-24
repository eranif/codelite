#include "ChatAISettingsDlg.hpp"

#include "ChatAIConfig.hpp"
#include "ModelPage.hpp"
#include "globals.h"

ChatAISettingsDlg::ChatAISettingsDlg(wxWindow* parent, ChatAIConfig& config)
    : AssistanceAISettingsBaseDlg(parent)
    , m_config(config)
{
    m_config.Load();
    m_filePickerCLI->SetPath(m_config.GetLlamaCli());

    wxString selected_model_name =
        m_config.GetSelectedModel() != nullptr ? m_config.GetSelectedModel()->m_name : wxString{};
    auto models = m_config.GetModels();
    int selected_model_idx = wxNOT_FOUND;
    for (auto model : models) {
        ModelPage* page = new ModelPage(m_notebook, model);
        m_notebook->AddPage(page, model->m_name, false);
        int idx = m_choiceModels->Append(model->m_name);
        if (model->m_name == selected_model_name) {
            selected_model_idx = idx;
        }
    }

    if (selected_model_idx != wxNOT_FOUND) {
        m_choiceModels->SetSelection(selected_model_idx);
    }
}

ChatAISettingsDlg::~ChatAISettingsDlg() {}

void ChatAISettingsDlg::OnNewModel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString name = ::clGetTextFromUser("New model", "Model name:");
    if (name.empty()) {
        return;
    }

    if (m_config.ContainsModel(name)) {
        ::wxMessageBox(_("A model with this name already exists"), "CodeLite", wxOK | wxCENTRE | wxICON_WARNING);
        return;
    }

    std::shared_ptr<ChatAIConfig::Model> model(new ChatAIConfig::Model(name, wxEmptyString));
    ModelPage* page = new ModelPage(m_notebook, model);
    m_notebook->AddPage(page, name, true);
    m_choiceModels->Append(name);

    if (m_choiceModels->GetCount() == 1) {
        // first model
        m_choiceModels->SetSelection(0);
    }
}

void ChatAISettingsDlg::OnOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Save();
    EndModal(wxID_OK);
}

void ChatAISettingsDlg::OnDelete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int sel = m_notebook->GetSelection();
    if (sel == wxNOT_FOUND) {
        return;
    }
    wxString label = m_notebook->GetPageText(sel);

    if (::wxMessageBox(wxString() << _("You are about to delete model: ") << label << _("\nContinue?"), "CodeLite",
                       wxOK | wxCENTRE | wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
        return;
    }
    m_notebook->DeletePage(sel);

    bool deletingActive = m_choiceModels->GetStringSelection() == label;
    sel = m_choiceModels->FindString(label);
    if (sel != wxNOT_FOUND) {
        m_choiceModels->Delete((unsigned int)sel);
    }

    if (deletingActive && !m_choiceModels->IsEmpty()) {
        m_choiceModels->SetSelection(0);
    }
    Save();
}

void ChatAISettingsDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
    // do not allow deleting the "General" page
    event.Enable(m_notebook->GetSelection() != 0);
}

void ChatAISettingsDlg::Save()
{
    wxString selected_model = m_choiceModels->GetStringSelection();
    std::vector<std::shared_ptr<ChatAIConfig::Model>> models;
    models.reserve(m_notebook->GetPageCount() - 1);
    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        auto page = dynamic_cast<ModelPage*>(m_notebook->GetPage(i));
        if (page) {
            page->Save();
            models.push_back(page->GetModel());
        }
    }

    m_config.SetModels(models);
    m_config.SetSelectedModelName(selected_model);
    m_config.SetLlamaCli(m_filePickerCLI->GetPath());
    m_config.Save();
}

void ChatAISettingsDlg::OnSearchModels(wxHyperlinkEvent& event) { ::wxLaunchDefaultBrowser(event.GetURL()); }
