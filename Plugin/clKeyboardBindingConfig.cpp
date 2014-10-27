#include "clKeyboardBindingConfig.h"
#include "json_node.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"

clKeyboardBindingConfig::clKeyboardBindingConfig()
{
}

clKeyboardBindingConfig::~clKeyboardBindingConfig()
{
}

clKeyboardBindingConfig& clKeyboardBindingConfig::Load()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
    fn.AppendDir("config");
    if(!fn.Exists()) return *this;
    
    m_bindings.clear();
    JSONRoot root(fn);
    int arrSize = root.toElement().arraySize();
    for(int i=0; i<arrSize; ++i) {
        JSONElement item = root.toElement().arrayItem(i);
        clKeyboardManager::KeyBinding binding;
        binding.description = item.namedObject("description").toString();
        binding.accelerator = item.namedObject("accelerator").toString();
        binding.actionId = item.namedObject("actionId").toInt(wxNOT_FOUND);
        if(binding.actionId != wxNOT_FOUND) {
            m_bindings.insert(std::make_pair(binding.actionId, binding));
        }
    }
    return *this;
}

clKeyboardBindingConfig& clKeyboardBindingConfig::Save()
{
    JSONRoot root(cJSON_Array);
    clKeyboardManager::KeyBinding::Map_t::iterator iter = m_bindings.begin();
    for(; iter != m_bindings.end(); ++iter) {
        JSONElement binding = JSONElement::createObject();
        binding.addProperty("description", iter->second.description);
        binding.addProperty("accelerator", iter->second.accelerator);
        binding.addProperty("actionId", iter->second.actionId);
        root.toElement().arrayAppend(binding);
    }
    
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
    fn.AppendDir("config");
    root.save(fn);
    return *this;
}
