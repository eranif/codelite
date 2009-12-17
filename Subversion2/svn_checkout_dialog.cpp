#include "svn_checkout_dialog.h"
#include <wx/dirdlg.h>
#include "subversion2.h"
#include "imanager.h"
#include "windowattrmanager.h"

SvnCheckoutDialog::SvnCheckoutDialog( wxWindow* parent, Subversion2* plugin )
		: SvnCheckoutDialogBase( parent )
		, m_plugin(plugin)
{
	wxArrayString urls = m_plugin->GetSettings().GetUrls();
	m_comboBoxRepoURL->Append(urls);
	WindowAttrManager::Load(this, wxT("SvnCheckoutDialog"), m_plugin->GetManager()->GetConfigTool());
}

SvnCheckoutDialog::~SvnCheckoutDialog()
{
	SvnSettingsData ssd = m_plugin->GetSettings(); 
	wxArrayString   urls;
	wxString        selection = m_comboBoxRepoURL->GetValue();
	
	urls = ssd.GetUrls();
	selection.Trim().Trim(false);
	if(urls.Index(selection) == wxNOT_FOUND && selection.IsEmpty() == false) {
		urls.Add(selection);
	}
	ssd.SetUrls(urls);
	m_plugin->SetSettings(ssd);
	WindowAttrManager::Save(this, wxT("SvnCheckoutDialog"), m_plugin->GetManager()->GetConfigTool());
}

void SvnCheckoutDialog::OnCheckoutDirectoryText( wxCommandEvent& event )
{
	wxString dir = m_textCtrl20->GetValue();
	if(dir.IsEmpty() == false) {
		wxString name = m_comboBoxRepoURL->GetValue().AfterLast(wxT('/'));
		dir << wxFileName::GetPathSeparator() << name;
		m_staticTextCheckoutDirectory->SetLabel( dir );	
		
	} else {
		m_staticTextCheckoutDirectory->SetLabel(wxT(""));
	}
}

void SvnCheckoutDialog::OnBrowseDirectory( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(wxT(""), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrl20->SetValue(new_path);
	}
}

wxString SvnCheckoutDialog::GetTargetDir()
{
	return m_staticTextCheckoutDirectory->GetLabel();
}

wxString SvnCheckoutDialog::GetURL()
{
	return m_comboBoxRepoURL->GetValue();
}

void SvnCheckoutDialog::OnOkUI(wxUpdateUIEvent& event)
{
	event.Enable(m_textCtrl20->GetValue().IsEmpty() == false && m_comboBoxRepoURL->GetValue().IsEmpty() == false);
}
