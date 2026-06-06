//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : options_dlg2.h
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

#pragma once

#include "OptionsConfigPage.hpp"
#include "options_base_dlg2.hpp"

/** Implementing OptionsBaseDlg2 */
class PreferencesDialog : public OptionsBaseDlg2
{

public:
    explicit PreferencesDialog(wxWindow* parent);
    ~PreferencesDialog() override = default;

    bool IsRestartRequired() const
    {
        for (size_t i = 0; i < m_treeBook->GetPageCount(); ++i) {
            const OptionsConfigPage* p = dynamic_cast<OptionsConfigPage*>(m_treeBook->GetPage(i));
            if (p && p->IsRestartRequired()) {
                return true;
            }
        }
        return false;
    }

protected:
    void OnButtonOK(wxCommandEvent& event) override;
    void OnButtonCancel(wxCommandEvent& event) override;
    void OnButtonApply(wxCommandEvent& event) override;
    void Initialize();
    void DoSave();

    template <typename T>
    void AddPage(T* ptr, wxString const& caption, bool selected = false)
    {
        m_treeBook->AddPage(ptr, caption, selected);
    }

private:
    OptionsConfigPtr m_options;
};
