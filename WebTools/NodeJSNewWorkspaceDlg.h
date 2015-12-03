//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : NodeJSNewWorkspaceDlg.h
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

#ifndef NODEJSNEWWORKSPACEDLG_H
#define NODEJSNEWWORKSPACEDLG_H
#include "WebToolsBase.h"

class NodeJSNewWorkspaceDlg : public NodeJSNewWorkspaceDlgBase
{
protected:
    virtual void OnExistingFolderSelected(wxFileDirPickerEvent& event);
    virtual void OnCheckNewFolder(wxCommandEvent& event);
    void UpdatePreview();

public:
    NodeJSNewWorkspaceDlg(wxWindow* parent);
    virtual ~NodeJSNewWorkspaceDlg();
    wxString GetWorkspaceFilename() const { return m_staticTextPreview->GetLabel(); }

protected:
    virtual void OnFolderSelected(wxFileDirPickerEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnTextUpdate(wxCommandEvent& event);
};
#endif // NODEJSNEWWORKSPACEDLG_H
