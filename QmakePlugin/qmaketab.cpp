//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : qmaketab.cpp
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

#include "qmaketab.h"
#include "imanager.h"
#include <wx/log.h>
#include "project.h"
#include "workspace.h"
#include "qmakeplugindata.h"

QMakeTab::QMakeTab( wxWindow* parent, QmakeConf *conf )
		: QMakeTabBase( parent )
		, m_conf(conf)
{
	m_choiceQmakeSettings->Clear();
	m_choiceQmakeSettings->Append( m_conf->GetAllConfigurations() );
}

void QMakeTab::Save(IManager *manager, const wxString& projectName, const wxString& confgName)
{
	wxString errMsg;
	ProjectPtr p = manager->GetWorkspace()->FindProjectByName(projectName, errMsg);
	if ( p ) {
		wxString data = p->GetPluginData(wxT("qmake"));
		QmakePluginData pd( data );
		QmakePluginData::BuildConfPluginData bcpd;

		bcpd.m_buildConfName      = confgName;
		bcpd.m_enabled            = m_checkBoxUseQmake->IsChecked();
		bcpd.m_freeText           = m_textCtrlFreeText->GetValue();
		bcpd.m_qmakeConfig        = m_choiceQmakeSettings->GetStringSelection();
		bcpd.m_qmakeExecutionLine = m_textCtrlQmakeExeLine->GetValue();

		pd.SetDataForBuildConf(confgName, bcpd);

		p->SetPluginData(wxT("qmake"), pd.ToString());
	}
}

void QMakeTab::Load(IManager* manager, const wxString& projectName, const wxString& confgName)
{
	wxString errMsg;
	ProjectPtr p = manager->GetWorkspace()->FindProjectByName(projectName, errMsg);
	if ( p ) {
		wxString data = p->GetPluginData(wxT("qmake"));
		QmakePluginData pd( data );
		QmakePluginData::BuildConfPluginData bcpd;

		if (pd.GetDataForBuildConf(confgName, bcpd)) {

			m_textCtrlQmakeExeLine->SetValue( bcpd.m_qmakeExecutionLine );
			int where = m_choiceQmakeSettings->FindString( bcpd.m_qmakeConfig );
			if ( where != wxNOT_FOUND ) {
				m_choiceQmakeSettings->SetSelection(where);
			}
			m_textCtrlFreeText->SetValue( bcpd.m_freeText );
			m_checkBoxUseQmake->SetValue( bcpd.m_enabled );
		}
	}
}

void QMakeTab::OnUseQmake(wxUpdateUIEvent& e)
{
	e.Enable( m_checkBoxUseQmake->IsChecked() );
}
