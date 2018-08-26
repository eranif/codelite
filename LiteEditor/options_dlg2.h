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

#ifndef __options_dlg2__
#define __options_dlg2__

#include "options_base_dlg2.h"
#include "treebooknodebase.h"

/** Implementing OptionsBaseDlg2 */
class PreferencesDialog : public OptionsBaseDlg2
{
private:
    // We do not need to clean up those pointers because they are
    // deleted by the wxTreeBook!
    std::list<TreeBookNodeBase*> m_contentObjects;

protected:
    // Handlers for OptionsBaseDlg2 events.
    void OnButtonOK(wxCommandEvent& event);
    void OnButtonCancel(wxCommandEvent& event);
    void OnButtonApply(wxCommandEvent& event);
    void Initialize();
    void DoSave();
    
    template <typename T> void AddSubPage(T* ptr, wxString const& caption, bool selected = false)
    {
        m_treeBook->AddSubPage(ptr, caption, selected);
        if(ptr) {
            m_contentObjects.push_back(ptr);
        }
    }
    template <typename T> void AddPage(T* ptr, wxString const& caption, bool selected = false)
    {
        m_treeBook->AddPage(ptr, caption, selected);
        if(ptr) {
            m_contentObjects.push_back(ptr);
        }
    }

public:
    /** Constructor */
    PreferencesDialog(wxWindow* parent);
    virtual ~PreferencesDialog();
    bool restartRquired;
};

#endif // __options_dlg2__
