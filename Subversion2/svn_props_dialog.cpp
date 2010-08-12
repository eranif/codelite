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
	m_textCtrlBugMessage->SetValue( props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE) );

	WindowAttrManager::Load(this, wxT("SvnPropsDlg"), m_plugin->GetManager()->GetConfigTool());
}

SvnPropsDlg::~SvnPropsDlg()
{
	WindowAttrManager::Save(this, wxT("SvnPropsDlg"), m_plugin->GetManager()->GetConfigTool());
}


wxString SvnPropsDlg::GetBugTrackerMessage() const
{
	return m_textCtrlBugMessage->GetValue();
}

wxString SvnPropsDlg::GetBugTrackerURL() const
{
	return m_textCtrlBugURL->GetValue();
}

