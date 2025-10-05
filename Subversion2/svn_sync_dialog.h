//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_sync_dialog.h
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

#ifndef __svn_sync_dialog__
#define __svn_sync_dialog__

#include "subversion2_ui.h"
#include "wxcrafter.h"

class Subversion2;
class SvnSyncDialog : public SvnSyncDialogBaseClass
{
    Subversion2 *m_plugin;
private:
    void UpdateUrl(const wxString& rootDir);

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnButtonOK( wxCommandEvent& event );

public:
    /** Constructor */
    SvnSyncDialog( wxWindow* parent, Subversion2 *plugin, const wxString& rootDir, bool excludeBin, const wxString& excludeExtensions );
    virtual ~SvnSyncDialog() = default;

    const wxString& GetRootDir() const {
        return m_rootDir;
    }
    const wxString& GetExcludeExtensions() const {
        return m_excludeExtensions;
    }
    bool GetExcludeBin() const {
        return m_excludeBin;
    }

private:
    wxString m_rootDir;
    wxString m_excludeExtensions;
    bool m_excludeBin;
};

#endif // __svn_sync_dialog__
