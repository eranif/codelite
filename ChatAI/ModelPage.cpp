#include "ModelPage.hpp"

ModelPage::ModelPage(wxWindow* parent, std::shared_ptr<ChatAIConfig::Model> model)
    : ModelPageBase(parent)
    , m_model(model)
{
    if (m_model) {
        m_textCtrlModelName->ChangeValue(m_model->m_name);
        m_filePickerModelFile->SetPath(m_model->m_modelFile);
    } else {
        m_model.reset(new ChatAIConfig::Model(wxEmptyString, wxEmptyString));
    }
}

ModelPage::~ModelPage() {}

void ModelPage::Save()
{
    m_model->m_name = m_textCtrlModelName->GetValue();
    m_model->m_modelFile = m_filePickerModelFile->GetPath();
}
