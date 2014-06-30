//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : FolderMappingDlg.cpp
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

#include "FolderMappingDlg.h"
#include "LLDBProtocol/LLDBSettings.h"

FolderMappingDlg::FolderMappingDlg(wxWindow* parent)
    : FolderMappingBaseDlg(parent)
{
    LLDBSettings settings;
    settings.Load();
    
    m_dirPickerLocal->SetPath( settings.GetLastLocalFolder() );
    m_textCtrlRemote->ChangeValue( settings.GetLastRemoteFolder() );
}

FolderMappingDlg::~FolderMappingDlg()
{
    LLDBSettings settings;
    settings.Load();
    settings.SetLastLocalFolder(m_dirPickerLocal->GetPath()  );
    settings.SetLastRemoteFolder(m_textCtrlRemote->GetValue()  );
    settings.Save();
}

void FolderMappingDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlRemote->IsEmpty() && !m_dirPickerLocal->GetPath().IsEmpty());
}

LLDBPivot FolderMappingDlg::GetPivot() const
{
    LLDBPivot pivot;
    pivot.SetLocalFolder( m_dirPickerLocal->GetPath() );
    pivot.SetRemoteFolder( m_textCtrlRemote->GetValue() );
    return pivot;
}
