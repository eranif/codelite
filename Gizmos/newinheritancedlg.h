//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newinheritancedlg.h
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
#ifndef __newinheritancedlg__
#define __newinheritancedlg__

#include "newclassbasedlg.h"
class IManager;

/** Implementing NewIneritanceBaseDlg */
class NewIneritanceDlg : public NewIneritanceBaseDlg
{
    IManager *m_mgr;
    wxString m_fileName;

protected:
    // Handlers for NewIneritanceBaseDlg events.
    void OnButtonMore( wxCommandEvent& event );

public:
    /** Constructor */
    NewIneritanceDlg( wxWindow* parent, IManager *mgr, const wxString &parentName = wxEmptyString, const wxString &access = wxEmptyString );
    virtual ~NewIneritanceDlg();
    wxString GetParentName() {
        return m_textCtrlInhertiance->GetValue();
    }
    wxString GetAccess() {
        return m_choiceAccess->GetStringSelection();
    }

    wxString GetFileName() const {
        return m_fileName;
    }
};

#endif // __newinheritancedlg__
