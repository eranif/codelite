//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : wxcl_log_text_ctrl.h
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

#ifndef WXCLLOGTEXTCTRL_H
#define WXCLLOGTEXTCTRL_H

#include <wx/log.h>
#include <wx/textctrl.h>

class wxclTextCtrl : public wxLog
{
public:
    wxclTextCtrl(wxTextCtrl* text);
    virtual ~wxclTextCtrl();
    void Reset() ;

    wxTextCtrl* GetPTextCtrl() {
        return m_pTextCtrl;
    }
protected:
    void DoInit();

    // implement sink function
    virtual void DoLogText(const wxString& msg) {
        m_pTextCtrl->AppendText(msg + wxS("\n"));
    }

private:
    // the control we use
    wxTextCtrl *m_pTextCtrl;
};


#endif // WXCLLOGTEXTCTRL_H
