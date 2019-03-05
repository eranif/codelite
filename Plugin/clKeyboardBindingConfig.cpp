#include "clKeyboardBindingConfig.h"
#include "JSON.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"

clKeyboardBindingConfig::clKeyboardBindingConfig() {}

clKeyboardBindingConfig::~clKeyboardBindingConfig() {}

clKeyboardBindingConfig& clKeyboardBindingConfig::Load()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
    fn.AppendDir("config");
    if(!fn.Exists()) return *this;

    m_bindings.clear();
    JSON root(fn);

    {
        JSONItem menus = root.toElement().namedObject("menus");
        int arrSize = menus.arraySize();
        for(int i = 0; i < arrSize; ++i) {
            JSONItem item = menus.arrayItem(i);
            MenuItemData binding;
            binding.action = item.namedObject("description").toString();
            binding.accel = item.namedObject("accelerator").toString();
            binding.parentMenu = item.namedObject("parentMenu").toString();
            binding.resourceID = item.namedObject("resourceID").toString();
            if(binding.resourceID == "text_word_complete") {
                // This entry was moved from Word Completion plugin to CodeLite Edit menu entry
                binding.resourceID = "simple_word_completion";
                binding.parentMenu = "Edit";
                binding.action = "Complete Word";
            } else if(binding.resourceID == "complete_word") {
                // The "action" was changed
                binding.action = "Code Complete";
            } else if(binding.resourceID == "word_complete") {
                binding.resourceID = "complete_word";
            }
            m_bindings.insert(std::make_pair(binding.resourceID, binding));
        }
    }
    return *this;
}

clKeyboardBindingConfig& clKeyboardBindingConfig::Save()
{
    JSON root(cJSON_Object);
    JSONItem mainObj = root.toElement();
    JSONItem menuArr = JSONItem::createArray("menus");
    mainObj.append(menuArr);
    for(MenuItemDataMap_t::iterator iter = m_bindings.begin(); iter != m_bindings.end(); ++iter) {
        JSONItem binding = JSONItem::createObject();
        binding.addProperty("description", iter->second.action);
        binding.addProperty("accelerator", iter->second.accel);
        binding.addProperty("resourceID", iter->second.resourceID);
        binding.addProperty("parentMenu", iter->second.parentMenu);
        menuArr.arrayAppend(binding);
    }
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
    fn.AppendDir("config");
    root.save(fn);
    return *this;
}
