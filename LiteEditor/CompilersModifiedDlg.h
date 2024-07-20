//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilersModifiedDlg.h
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

#ifndef COMPILERSMODIFIEDDLG_H
#define COMPILERSMODIFIEDDLG_H

#include "CompilersFoundDlgBase.h"
#include "macros.h"

#include <wx/propgrid/property.h>
#include <wx/vector.h>

class CompilersModifiedDlg : public CompilersModifiedDlgBase
{
    wxVector<wxPGProperty*> m_props;
    wxStringMap_t           m_table;
    bool                    m_enableOKButton;
    
public:
    CompilersModifiedDlg(wxWindow* parent, const wxStringSet_t& deletedCompilers);
    virtual ~CompilersModifiedDlg();
    
    const wxStringMap_t& GetReplacementTable() const {
        return m_table;
    }
    
protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // COMPILERSMODIFIEDDLG_H
