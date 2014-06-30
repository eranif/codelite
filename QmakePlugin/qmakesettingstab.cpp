//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : qmakesettingstab.cpp
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

#include "qmakesettingstab.h"
#include <wx/dir.h>
#include "procutils.h"
#include <wx/filename.h>
#include "qmakeconf.h"

QmakeSettingsTab::QmakeSettingsTab( wxWindow* parent, const wxString &name, QmakeConf* conf)
    : QmakeSettingsTabBase( parent )
    , m_name(name)
{
    Load(conf);
}

void QmakeSettingsTab::Load(QmakeConf* conf)
{
    if ( conf ) {
        wxString qmakePath(conf->Read(m_name + wxT("/qmake")));
        m_filePickerQmakeExec->SetFileName( wxFileName(qmakePath) );
        
        m_comboBoxQmakespec->Append( GetSpecList(conf->Read(m_name + wxT("/qmake")) ) );
        m_comboBoxQmakespec->SetValue(conf->Read(m_name + wxT("/qmakespec")));
        m_textCtrlQtdir->SetValue(conf->Read(m_name + wxT("/qtdir")));
    }
}

void QmakeSettingsTab::Save(QmakeConf* conf)
{
    conf->Write(m_name + wxT("/qmake"),     m_filePickerQmakeExec->GetPath());
    conf->Write(m_name + wxT("/qmakespec"), m_comboBoxQmakespec->GetValue());
    conf->Write(m_name + wxT("/qtdir"),     m_textCtrlQtdir->GetValue());
    conf->Flush();
}

wxArrayString QmakeSettingsTab::GetSpecList(const wxString& qmakePath)
{
    wxArrayString specs;

    if ( qmakePath.IsEmpty() == false && wxFileName::FileExists(qmakePath) ) {
        wxArrayString cmdOutput;

        ProcUtils::SafeExecuteCommand(wxString::Format(wxT("\"%s\" -query QT_INSTALL_DATA"), qmakePath.c_str()), cmdOutput);
        if ( cmdOutput.IsEmpty() == false ) {
            wxFileName    installData ( cmdOutput.Item(0).Trim().Trim(false), wxEmptyString );
            wxArrayString files;

            installData.AppendDir(wxT("mkspecs"));
            wxDir::GetAllFiles(installData.GetFullPath(), &files, wxT("*.conf"), wxDIR_DEFAULT);

            for ( size_t i=0; i<files.GetCount(); i++) {

                wxFileName fn(files.Item(i));
                if ( specs.Index(fn.GetDirs().Last()) == wxNOT_FOUND ) {
                    specs.Add( fn.GetDirs().Last() );
                }
            }
        }
    }
    return specs;
}

void QmakeSettingsTab::OnFileSelected(wxFileDirPickerEvent& event)
{
    m_comboBoxQmakespec->Clear();
    m_comboBoxQmakespec->Append( GetSpecList(m_filePickerQmakeExec->GetPath()) );
}
