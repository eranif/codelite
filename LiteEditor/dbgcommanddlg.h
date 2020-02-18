//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dbgcommanddlg.h
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
#ifndef __dbgcommanddlg__
#define __dbgcommanddlg__
#include "DbgCommandBaseDlg.h"

class DbgCommandDlg : public DbgCommandBaseDlg
{
public:
    /** Constructor */
    DbgCommandDlg(wxWindow* parent);
    virtual ~DbgCommandDlg();

    wxString GetExpression() { return m_textCtrlCommand->GetValue(); }
    wxString GetName() { return m_textCtrlName->GetValue(); }
    wxString GetDbgCommand() { return m_textCtrtDebuggerCommand->GetValue(); }

    void SetDbgCommand(const wxString& dbgCmd) { m_textCtrtDebuggerCommand->SetValue(dbgCmd); }
    void SetName(const wxString& name) { m_textCtrlName->SetValue(name); }
    void SetExpression(const wxString& cmd) { m_textCtrlCommand->SetValue(cmd); }
};

#endif // __dbgcommanddlg__
