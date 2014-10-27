#ifndef CLKEYBOARDBINDINGCONFIG_H
#define CLKEYBOARDBINDINGCONFIG_H

#include "codelite_exports.h"
#include "clKeyboardManager.h"

class WXDLLIMPEXP_SDK clKeyboardBindingConfig
{
    clKeyboardManager::KeyBinding::Map_t m_bindings;

public:
    clKeyboardBindingConfig();
    virtual ~clKeyboardBindingConfig();

    clKeyboardBindingConfig& Load();
    clKeyboardBindingConfig& Save();

    clKeyboardBindingConfig& SetBindings(const clKeyboardManager::KeyBinding::Map_t& bindings)
    {
        this->m_bindings = bindings;
        return *this;
    }
    const clKeyboardManager::KeyBinding::Map_t& GetBindings() const { return m_bindings; }
};

#endif // CLKEYBOARDBINDINGCONFIG_H
