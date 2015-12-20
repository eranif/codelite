//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CScopeSettingsDlg.h
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

#ifndef CSCOPESETTINGSDLG_H
#define CSCOPESETTINGSDLG_H
#include "CscopeTabBase.h"

class CScopeSettingsDlg : public CScopeSettingsDlgBase
{
public:
    CScopeSettingsDlg(wxWindow* parent);
    virtual ~CScopeSettingsDlg();
    
    wxString GetPath() const { return m_filePickerCScopeExe->GetPath(); }
};
#endif // CSCOPESETTINGSDLG_H
