//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileexplorer.h
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
#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "wx/panel.h"
#include <imanager.h>

class wxTreeCtrl;
class clTreeCtrlPanel;

class FileExplorer : public wxPanel
{
private:
    clTreeCtrlPanel* m_view;
    wxString m_caption;
    void CreateGUIControls();

protected:
    void OnFolderDropped(clCommandEvent& event);
    void OnOpenFolder(wxCommandEvent& event);

public:
    FileExplorer(wxWindow* parent, const wxString& caption);
    virtual ~FileExplorer();
    const wxString& GetCaption() const { return m_caption; }
    TreeItemInfo GetItemInfo() { return m_view->GetSelectedItemInfo(); }
    clTreeCtrl* GetTree() { return m_view->GetTreeCtrl(); }
    void OpenFolder(const wxString& path);
};

#endif // FILEEXPLORER_H
