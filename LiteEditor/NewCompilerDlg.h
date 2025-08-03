//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : NewCompilerDlg.h
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

#ifndef NEWCOMPILERDLG_H
#define NEWCOMPILERDLG_H
#include "compiler_pages.h"

class NewCompilerDlg : public NewCompilerDlgBase
{
public:
    NewCompilerDlg(wxWindow* parent);
    virtual ~NewCompilerDlg() = default;

    wxString GetCompilerName() const {
        return m_textCtrlCompilerName->GetValue();
    }

    wxString GetMasterCompiler() const;
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // NEWCOMPILERDLG_H
