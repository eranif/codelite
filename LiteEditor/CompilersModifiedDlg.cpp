//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilersModifiedDlg.cpp
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

#include "CompilersModifiedDlg.h"
#include <build_settings_config.h>
#include <windowattrmanager.h>

#define SELECT_COMPILER _("<Click to select a compiler...>")

CompilersModifiedDlg::CompilersModifiedDlg(wxWindow* parent, const wxStringSet_t& deletedCompilers)
    : CompilersModifiedDlgBase(parent)
    , m_enableOKButton(false)
{
    wxArrayString compilers;
    compilers = BuildSettingsConfigST::Get()->GetAllCompilersNames();
    compilers.Insert(SELECT_COMPILER, 0);
    
    wxStringSet_t::const_iterator iter = deletedCompilers.begin();
    for(; iter != deletedCompilers.end(); ++iter ) {
        m_table.insert( std::make_pair(*iter, SELECT_COMPILER) );
        wxPGProperty* prop = m_pgMgrCompilers->AppendIn( m_pgPropHeader,  new wxEnumProperty( *iter, wxPG_LABEL, compilers) );
        m_props.push_back( prop );
        
        wxString message;
        message << _("Create a new compiler named '") << *iter << _("' by cloning an existing compiler");
        prop->SetHelpString( message );
    }
    SetName("CompilersModifiedDlg");
    WindowAttrManager::Load(this);
}

CompilersModifiedDlg::~CompilersModifiedDlg()
{
    
}

void CompilersModifiedDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( m_enableOKButton );
}

void CompilersModifiedDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    wxString newCompiler = event.GetProperty()->GetValueAsString();
    wxString oldCompiler = event.GetPropertyName();
    
    m_table.erase( oldCompiler );
    m_table[ oldCompiler ] = newCompiler;
    
    m_enableOKButton = true;
    wxStringMap_t::iterator iter = m_table.begin();
    for(; iter != m_table.end(); ++iter ) {
        if ( iter->second == SELECT_COMPILER ) {
            m_enableOKButton = false;
            break;
        }
    }
}
