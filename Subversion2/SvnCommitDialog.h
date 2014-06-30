//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : SvnCommitDialog.h
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

#ifndef SVNCOMMITDIALOG_H
#define SVNCOMMITDIALOG_H

#include "wxcrafter.h"
#include "macros.h"

class Subversion2;
class IProcess;
class SvnCommitDialog : public SvnCommitDialogBaseClass
{
    Subversion2 * m_plugin;
    wxString      m_url;
    wxString      m_repoPath;
    IProcess*     m_process;
    wxString      m_output;
    wxStringMap_t m_cache;
    wxString      m_currentFile;
    
public:
    static wxString NormalizeMessage(const wxString &message);
    void OnChoiceMessage(wxCommandEvent &e);

public:
    SvnCommitDialog(wxWindow* parent, Subversion2* plugin);
    SvnCommitDialog(wxWindow* parent, const wxArrayString &paths, const wxString &url, Subversion2 *plugin, const wxString &repoPath);
    virtual ~SvnCommitDialog();

    wxString GetMesasge();
    wxArrayString GetPaths();
    
protected:  
    void DoShowDiff(int selection);
    
protected:
    DECLARE_EVENT_TABLE()
    
    virtual void OnFileSelected(wxCommandEvent& event);
    void OnProcessOutput(wxCommandEvent &e);
    void OnProcessTerminatd(wxCommandEvent &e);
    
};

#endif // SVNCOMMITDIALOG_H
