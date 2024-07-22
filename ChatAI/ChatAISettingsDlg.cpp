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

    std::shared_ptr<ChatAIConfig::Model> model(new ChatAIConfig::Model(name, wxEmptyString));
    ModelPage* page = new ModelPage(m_notebook, model);
    m_notebook->AddPage(page, name, true);
}

void ChatAISettingsDlg::OnOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
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
    EndModal(wxID_OK);
}
