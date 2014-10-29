#include "clKeyboardBindingConfig.h"
#include "json_node.h"
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
    JSONRoot root(fn);

    {
        JSONElement menus = root.toElement().namedObject("menus");
        int arrSize = menus.arraySize();
        for(int i = 0; i < arrSize; ++i) {
            JSONElement item = menus.arrayItem(i);
            MenuItemData binding;
            binding.action = item.namedObject("description").toString();
            binding.accel = item.namedObject("accelerator").toString();
            binding.parentMenu = item.namedObject("parentMenu").toString();
            binding.resourceID = item.namedObject("resourceID").toString();
            m_bindings.insert(std::make_pair(binding.resourceID, binding));
        }
    }
#if 0
    {
        JSONElement globals = root.toElement().namedObject("globals");
        int arrSize = globals.arraySize();
        for(int i = 0; i < arrSize; ++i) {
            JSONElement item = globals.arrayItem(i);
            MenuItemData binding;
            binding.action = item.namedObject("description").toString();
            binding.accel = item.namedObject("accelerator").toString();
            binding.parentMenu = item.namedObject("parentMenu").toString();
            binding.resourceID = item.namedObject("actionId").toString();
            m_globalBindings.insert(std::make_pair(binding.resourceID, binding));
        }
    }
#endif
    return *this;
}

clKeyboardBindingConfig& clKeyboardBindingConfig::Save()
{
    JSONRoot root(cJSON_Object);
    JSONElement mainObj = root.toElement();
    JSONElement menuArr = JSONElement::createArray("menus");
    mainObj.append(menuArr);
    for(MenuItemDataMap_t::iterator iter = m_bindings.begin(); iter != m_bindings.end(); ++iter) {
        JSONElement binding = JSONElement::createObject();
        binding.addProperty("description", iter->second.action);
        binding.addProperty("accelerator", iter->second.accel);
        binding.addProperty("resourceID", iter->second.resourceID);
        binding.addProperty("parentMenu", iter->second.parentMenu);
        menuArr.arrayAppend(binding);
    }
#if 0
    JSONElement globalsArr = JSONElement::createArray("globals");
    mainObj.append(globalsArr);

    for(MenuItemDataMap_t::iterator iter = m_globalBindings.begin(); iter != m_globalBindings.end();
        ++iter) {
        JSONElement binding = JSONElement::createObject();
        binding.addProperty("description", iter->second.action);
        binding.addProperty("accelerator", iter->second.accel);
        binding.addProperty("resourceID", iter->second.resourceID);
        binding.addProperty("parentMenu", iter->second.parentMenu);
        globalsArr.arrayAppend(binding);
    }
#endif

    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
    fn.AppendDir("config");
    root.save(fn);
    return *this;
}
