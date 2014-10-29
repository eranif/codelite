#ifndef CLKEYBOARDBINDINGCONFIG_H
#define CLKEYBOARDBINDINGCONFIG_H

#include "codelite_exports.h"
#include "clKeyboardManager.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"

class WXDLLIMPEXP_SDK clKeyboardBindingConfig 
{
    MenuItemDataMap_t m_bindings;
public:
    clKeyboardBindingConfig();
    virtual ~clKeyboardBindingConfig();

    clKeyboardBindingConfig& Load();
    clKeyboardBindingConfig& Save();
    
    bool Exists() const {
        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
        fn.AppendDir("config");
        return fn.Exists();
    }
    
    clKeyboardBindingConfig& SetBindings(const MenuItemDataMap_t& menus, const MenuItemDataMap_t& globals)
    {
        this->m_bindings = menus;
        this->m_bindings.insert(globals.begin(), globals.end());
        return *this;
    }
    const MenuItemDataMap_t& GetBindings() const { return m_bindings; }
};

#endif // CLKEYBOARDBINDINGCONFIG_H
