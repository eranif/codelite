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
