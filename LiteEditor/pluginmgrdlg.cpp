#include "pluginmgrdlg.h"
#include "pluginsdata.h"
#include "manager.h"
#include "pluginconfig.h"

PluginMgrDlg::PluginMgrDlg( wxWindow* parent )
		:
		PluginMgrDlgBase( parent )
{
	this->Initialize();
}

void PluginMgrDlg::Initialize()
{
	PluginsData pluginsData;
	PluginConfig::Instance()->ReadObject(wxT("plugins_data"), &pluginsData);
	
	//Clear the list
	m_checkListPluginsList->Clear();
	
	std::map<wxString, PluginInfo>::const_iterator iter = pluginsData.GetInfo().begin();
	for(; iter != pluginsData.GetInfo().end(); iter++) {
		PluginInfo info = iter->second;
		
		int item = m_checkListPluginsList->Append(info.GetName());
		if(item != wxNOT_FOUND) {
			m_checkListPluginsList->Check((unsigned int)item, info.GetEnabled());
		}
	}
	
	if(m_checkListPluginsList->IsEmpty() == false) {
		m_checkListPluginsList->Select(0);
		CreateInfoPage(0);
	}
}

void PluginMgrDlg::OnItemSelected(wxCommandEvent &event)
{
	int item = event.GetSelection();
	CreateInfoPage((unsigned int)item);
}

void PluginMgrDlg::OnButtonOK(wxCommandEvent &event)
{
	PluginsData pluginsData;
	PluginConfig::Instance()->ReadObject(wxT("plugins_data"), &pluginsData);
	
	std::map< wxString, PluginInfo > pluginsInfo = pluginsData.GetInfo();
	
	for(unsigned int i = 0; i<m_checkListPluginsList->GetCount(); i++) {
		wxString name = m_checkListPluginsList->GetString(i);
		std::map< wxString, PluginInfo >::iterator iter = pluginsInfo.find(name);
		if(iter != pluginsInfo.end()) {
			//update the enable flag of the plugin
			PluginInfo info = iter->second;
			info.SetEnabled( m_checkListPluginsList->IsChecked(i) );
			pluginsInfo[info.GetName()] = info;
		}
	}
	//write back the data to the disk
	pluginsData.SetInfo( pluginsInfo );
	PluginConfig::Instance()->WriteObject(wxT("plugins_data"), &pluginsData);
	EndModal(wxID_OK);
}

void PluginMgrDlg::CreateInfoPage(unsigned int index)
{
	PluginsData pluginsData;
	PluginConfig::Instance()->ReadObject(wxT("plugins_data"), &pluginsData);
	
	//get the plugin name
	wxString pluginName = m_checkListPluginsList->GetString(index);
	std::map<wxString, PluginInfo>::const_iterator iter = pluginsData.GetInfo().find(pluginName);
	if(iter != pluginsData.GetInfo().end()) {
		PluginInfo info = iter->second;
		
		wxString content;
		content << wxT("<html><body>");
		content << wxT("<table border=0 width=\"100%\" >");
		
		//create line with the plugin name
		content << wxT("<tr bgcolor=\"LIGHT GREY\">");
		content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>Plugin Name:</strong></font></td>");
		content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetName() << wxT("</font></td>");
		content << wxT("</tr>");
		
		//plugin author
		content << wxT("<tr bgcolor=\"WHITE\">");
		content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>Author:</strong></font></td>");
		content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetAuthor() << wxT("</font></td>");
		content << wxT("</tr>");

		//plugin version
		content << wxT("<tr bgcolor=\"LIGHT GREY\">");
		content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>Version:</strong></font></td>");
		content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetVersion() << wxT("</font></td>");
		content << wxT("</tr>");
		
		//plugin description
		content << wxT("<tr bgcolor=\"WHITE\">");
		content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>Description:</strong></font></td>");
		content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetDescription() << wxT("</font></td>");
		content << wxT("</tr>");
		
		content << wxT("<tr bgcolor=\"LIGHT GREY\">");
		content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>Status:</strong></font></td>");
		
		wxString status;
		if(info.GetEnabled()) {
			status = wxT("<img src=\"$(InstallPath)/images/plugin_ok.png\" ></img>");
		}else{
			status = wxT("<img src=\"$(InstallPath)/images/plugin_not_ok.png\" > </img>");
		}
		status.Replace(wxT("$(InstallPath)"), ManagerST::Get()->GetStarupDirectory());
		
		content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << status << wxT("</font></td>");
		content << wxT("</tr>");
		
		content << wxT("</table><html><body>");
		
		m_htmlWinDesc->SetPage(content);
	}
}
