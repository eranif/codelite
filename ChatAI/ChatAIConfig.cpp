#include "ChatAIConfig.hpp"

#include "cl_config.h"

#include <algorithm>

ChatAIConfig::ChatAIConfig()
    : clConfigItem("chat-ai")
{
}

ChatAIConfig::~ChatAIConfig() {}

void ChatAIConfig::Load() { clConfig::Get().ReadItem(this); }

void ChatAIConfig::Save() { clConfig::Get().WriteItem(this); }

void ChatAIConfig::FromJSON(const JSONItem& json)
{
    wxString defaultCli = clStandardPaths::Get().GetBinaryFullPath("llama-cli");
    m_llamaCli = json.namedObject("cli").toString(defaultCli);
}

JSONItem ChatAIConfig::ToJSON() const
{
    auto obj = JSONItem::createObject(GetName());
    obj.addProperty("cli", m_llamaCli);
    return obj;
}

void ChatAIConfig::SetSelectedModelName(const wxString& selectedModel)
{
    auto iter = std::find_if(m_models.begin(), m_models.end(),
                             [&selectedModel](std::shared_ptr<Model> m) { return m->m_name == selectedModel; });

    if (iter == m_models.end()) {
        return;
    }

    m_selectedModel = (*iter);
}