//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSDebuggerDlg.h
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

#ifndef NODEJSDEBUGGERDLG_H
#define NODEJSDEBUGGERDLG_H
#include "WebToolsBase.h"
#include <wx/tokenzr.h>

class NodeJSDebuggerDlg : public NodeJSDebuggerDlgBase
{
public:
    enum eDialogType {
        kDebug,
        kDebugCLI,
        kExecute,
    };
    eDialogType m_type;

public:
    NodeJSDebuggerDlg(wxWindow* parent, eDialogType type);
    NodeJSDebuggerDlg(wxWindow* parent, eDialogType type, const wxFileName& script, const wxArrayString& args);
    virtual ~NodeJSDebuggerDlg();
    void GetCommand(wxString& command, wxString& command_args);
    wxArrayString GetArgs() const
    {
        return wxStringTokenize(m_stcCommandLineArguments->GetText(), "\n", wxTOKEN_STRTOK);
    }
    wxString GetScript() const { return m_filePickerScript->GetPath(); }
    wxString GetWorkingDirectory() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NODEJSDEBUGGERDLG_H
