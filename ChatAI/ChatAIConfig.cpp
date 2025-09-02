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

void ChatAIConfig::FromJSON(const JSONItem& json) { m_selectedModel = json.namedObject("selected_model").toString(); }

JSONItem ChatAIConfig::ToJSON() const
{
    auto obj = JSONItem::createObject(GetName());
    obj.addProperty("selected_model", m_selectedModel);
    return obj;
}
