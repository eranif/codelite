#include "svn_props_dialog.h"
#include "svn_local_properties.h"
#include "windowattrmanager.h"
#include "subversion2.h"

SvnPropsDlg::SvnPropsDlg( wxWindow* parent, const wxString& url, Subversion2 *plugin )
	: SvnPropsBaseDlg( parent )
	, m_plugin       (plugin)
	, m_url          (url)
{
	m_staticTextURL->SetLabel( m_url );

	SubversionLocalProperties props(m_url);
	m_textCtrlBugURL->SetValue( props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_URL) );
	m_textCtrlFrURL->SetValue ( props.ReadProperty(SubversionLocalProperties::FR_TRACKER_URL) );
	m_textCtrlBugMsg->SetValue( props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE) );
	m_textCtrlFrMsg->SetValue ( props.ReadProperty(SubversionLocalProperties::FR_TRACKER_MESSAGE) );
	WindowAttrManager::Load(this, wxT("SvnPropsDlg"), m_plugin->GetManager()->GetConfigTool());
}

SvnPropsDlg::~SvnPropsDlg()
{
	WindowAttrManager::Save(this, wxT("SvnPropsDlg"), m_plugin->GetManager()->GetConfigTool());
}

wxString SvnPropsDlg::GetBugTrackerURL() const
{
	return m_textCtrlBugURL->GetValue();
}

wxString SvnPropsDlg::GetFRTrackerURL() const
{
	return m_textCtrlFrURL->GetValue();
}

wxString SvnPropsDlg::GetBugMsg() const
{
	return m_textCtrlBugMsg->GetValue();
}

wxString SvnPropsDlg::GetFRMsg() const
{
	return m_textCtrlFrMsg->GetValue();
}

