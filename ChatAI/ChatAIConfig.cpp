#include "ChatAIConfig.hpp"

#include "cl_config.h"

#include <algorithm>

ChatAIConfig::ChatAIConfig()
    : clConfigItem("chat-ai")
{
    wxFileName defaultCli = clStandardPaths::Get().GetBinaryFullPath("llama-cli");
    m_llamaCli = defaultCli.GetFullPath();
}

ChatAIConfig::~ChatAIConfig() {}

void ChatAIConfig::Load() { clConfig::Get().ReadItem(this); }

void ChatAIConfig::Save() { clConfig::Get().WriteItem(this); }

void ChatAIConfig::FromJSON(const JSONItem& json)
{
    m_llamaCli = json.namedObject("cli").toString(m_llamaCli);

    m_models.clear();
    auto selection = json.namedObject("selected_model").toString();
    auto models = json.namedObject("models");
    for (int i = 0; i < models.arraySize(); ++i) {
        std::shared_ptr<Model> m(new Model());
        m->FromJSON(models[i]);
        if (m->m_name == selection) {
            m_selectedModel = m;
        }
        m_models.push_back(m);
    }
}

JSONItem ChatAIConfig::ToJSON() const
{
    auto obj = JSONItem::createObject(GetName());
    obj.addProperty("cli", m_llamaCli)
        .addProperty("selected_model", m_selectedModel != nullptr ? m_selectedModel->m_name : wxString{});

    auto models = obj.AddArray("models");
    for (auto model : m_models) {
        models.arrayAppend(model->ToJSON());
    }
    return obj;
}

void ChatAIConfig::SetSelectedModelName(const wxString& selectedModel)
{
    auto iter = std::find_if(m_models.begin(), m_models.end(),
                             [&selectedModel](std::shared_ptr<Model> m) { return m->m_name == selectedModel; });

    if (iter != m_models.end()) {
        m_selectedModel = (*iter);
    } else {
        m_selectedModel.reset();
    }
}

bool ChatAIConfig::ContainsModel(const wxString& modelName) const
{
    return std::find_if(m_models.begin(), m_models.end(),
                        [&modelName](std::shared_ptr<Model> m) { return m->m_name == modelName; }) != m_models.end();
}
