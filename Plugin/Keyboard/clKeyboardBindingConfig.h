//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clKeyboardBindingConfig.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLKEYBOARDBINDINGCONFIG_H
#define CLKEYBOARDBINDINGCONFIG_H

#include "clKeyboardManager.h"
#include "cl_standard_paths.h"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK clKeyboardBindingConfig
{
    MenuItemDataMap_t m_bindings;

public:
    clKeyboardBindingConfig();
    virtual ~clKeyboardBindingConfig();

    clKeyboardBindingConfig& Load();
    clKeyboardBindingConfig& Save();

    void MigrateOldResourceID(wxString& resourceID) const;

    bool Exists() const
    {
        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
        fn.AppendDir("config");
        return fn.Exists();
    }

    clKeyboardBindingConfig& SetBindings(const MenuItemDataMap_t& accels)
    {
        this->m_bindings = accels;
        return *this;
    }
    const MenuItemDataMap_t& GetBindings() const { return m_bindings; }
};

#endif // CLKEYBOARDBINDINGCONFIG_H
