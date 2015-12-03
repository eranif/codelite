//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : clSingleChoiceDialog.h
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

#ifndef CLSINGLECHOICEDIALOG_H
#define CLSINGLECHOICEDIALOG_H
#include "wxcrafter_plugin.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clSingleChoiceDialog : public clSingleChoiceDialogBase
{
public:
    clSingleChoiceDialog(wxWindow* parent, const wxArrayString& options, int initialSelection = 0);
    virtual ~clSingleChoiceDialog();
    wxString GetSelection() const;
protected:
    virtual void OnItemActivated(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // CLSINGLECHOICEDIALOG_H
