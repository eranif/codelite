#ifndef CLKEYBOARDBINDINGCONFIG_H
#define CLKEYBOARDBINDINGCONFIG_H

#include "codelite_exports.h"
#include "clKeyboardManager.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"

class WXDLLIMPEXP_SDK clKeyboardBindingConfig
{
    MenuItemDataMap_t m_bindings;
    MenuItemDataMap_t m_globalBindings;

public:
    clKeyboardBindingConfig();
    virtual ~clKeyboardBindingConfig();

    clKeyboardBindingConfig& Load();
    clKeyboardBindingConfig& Save();
    
    bool Exists() const {
        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
        return fn.Exists();
    }
    
    clKeyboardBindingConfig& SetBindings(const MenuItemDataMap_t& bindings)
    {
        this->m_bindings = bindings;
        return *this;
    }
    clKeyboardBindingConfig& SetGlobalBindings(const MenuItemDataMap_t& bindings)
    {
        this->m_globalBindings = bindings;
        return *this;
    }
    const MenuItemDataMap_t& GetBindings() const { return m_bindings; }
    const MenuItemDataMap_t& GetGlobalBindings() const { return m_globalBindings; }
};

#endif // CLKEYBOARDBINDINGCONFIG_H
