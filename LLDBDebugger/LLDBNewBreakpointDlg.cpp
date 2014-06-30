//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBNewBreakpointDlg.cpp
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

#include "LLDBNewBreakpointDlg.h"
#include "windowattrmanager.h"

LLDBNewBreakpointDlg::LLDBNewBreakpointDlg(wxWindow* parent)
    : LLDBNewBreakpointDlgBase(parent)
{
    m_checkBoxFileLine->SetValue(true);
    m_textCtrlFile->CallAfter( &wxTextCtrl::SetFocus );
    
    WindowAttrManager::Load(this, "LLDBNewBpDlg");
}

LLDBNewBreakpointDlg::~LLDBNewBreakpointDlg()
{
    WindowAttrManager::Save(this, "LLDBNewBpDlg");
}

void LLDBNewBreakpointDlg::OnFileLineEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxFileLine->IsChecked());
}

void LLDBNewBreakpointDlg::OnFuncNameUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxFuncName->IsChecked());
}

void LLDBNewBreakpointDlg::OnCheckFileAndLine(wxCommandEvent& event)
{
    event.Skip();
    if ( event.IsChecked() ) {
        m_checkBoxFuncName->SetValue( false );
        m_textCtrlLine->CallAfter( &wxTextCtrl::SetFocus );
        
    } else {
        m_checkBoxFuncName->SetValue( true );
        m_textCtrlFunctionName->CallAfter( &wxTextCtrl::SetFocus );
    }
}

void LLDBNewBreakpointDlg::OnCheckFuncName(wxCommandEvent& event)
{
    event.Skip();
    if ( event.IsChecked() ) {
        m_checkBoxFileLine->SetValue( false );
        m_textCtrlFunctionName->CallAfter( &wxTextCtrl::SetFocus );
        
    } else {
        m_checkBoxFileLine->SetValue( true );
        m_textCtrlLine->CallAfter( &wxTextCtrl::SetFocus );
        
    }
}

LLDBBreakpoint::Ptr_t LLDBNewBreakpointDlg::GetBreakpoint()
{
    if ( m_checkBoxFileLine->IsChecked() ) {
        
        long nLine = 0;
        wxString strLine( m_textCtrlLine->GetValue() );
        strLine.ToCLong( &nLine );
        LLDBBreakpoint::Ptr_t bp( new LLDBBreakpoint(m_textCtrlFile->GetValue(), nLine) );
        return bp;
        
    } else if ( m_checkBoxFuncName->IsChecked() ) {
        LLDBBreakpoint::Ptr_t  bp( new LLDBBreakpoint(m_textCtrlFunctionName->GetValue()) );
        return bp;
        
    } else {
        return LLDBBreakpoint::Ptr_t( NULL );
    }
}
