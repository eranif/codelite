//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : new_configuration_dlg.h
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

#ifndef __new_configuration_dlg__
#define __new_configuration_dlg__

#include "new_configuration_dlg_base.hpp"

///////////////////////////////////////////////////////////////////////////////
/// Class NewConfigurationDlg
///////////////////////////////////////////////////////////////////////////////
class NewConfigurationDlg : public NewConfigurationDlgBase
{
private:
    wxString m_projectName;

protected:
    void OnButtonOK(wxCommandEvent& event);

public:
    NewConfigurationDlg(wxWindow* parent, const wxString& projName, int id = wxID_ANY,
                        wxString title = _("New Configuration"), wxPoint pos = wxDefaultPosition,
                        wxSize size = wxSize(352, 199), int style = wxDEFAULT_DIALOG_STYLE);
};

#endif //__new_configuration_dlg__
