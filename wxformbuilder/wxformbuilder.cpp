#include "confformbuilder.h"
#include "procutils.h"
#include "workspace.h"
#include "project.h"
#include <wx/msgdlg.h>
#include "globals.h"
#include "formbuildsettingsdlg.h"
#include "wxfbitemdlg.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include "wxformbuilder.h"
#include <wx/xrc/xmlres.h>

static wxFormBuilder* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0)
	{
		thePlugin = new wxFormBuilder(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("wxFormBuilder"));
	info.SetDescription(wxT("wxFormBuilder integration with CodeLite"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

wxFormBuilder::wxFormBuilder(IManager *manager)
		: IPlugin(manager)
		, m_separatorItem(NULL)
		, m_openWithWxFbItem(NULL)
		, m_openWithWxFbSepItem(NULL)
{
	m_longName = wxT("wxFormBuilder integration with CodeLite");
	m_shortName = wxT("wxFormBuilder");
	m_topWin = m_mgr->GetTheApp();

	m_topWin->Connect(XRCID("wxfb_new_dialog"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFormBuilder::OnNewDialog), NULL, this);
	m_topWin->Connect(XRCID("wxfb_new_frame"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFormBuilder::OnNewFrame), NULL, this);
	m_topWin->Connect(XRCID("wxfb_new_panel"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFormBuilder::OnNewPanel), NULL, this);
	m_topWin->Connect(XRCID("wxfb_open"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFormBuilder::OpenWithWxFb), NULL, this);
}

wxFormBuilder::~wxFormBuilder()
{
}

wxToolBar *wxFormBuilder::CreateToolBar(wxWindow *parent)
{
	return NULL;
}

void wxFormBuilder::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item( NULL );

	item = new wxMenuItem( menu, XRCID("wxfb_settings"), wxT( "Settings..." ), wxT( "Settings..." ), wxITEM_NORMAL );
	menu->Append( item );

	pluginsMenu->Append(wxID_ANY, wxT("wxFormBuilder"), menu);

	m_topWin->Connect( XRCID("wxfb_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxFormBuilder::OnSettings ), NULL, this );
}

void wxFormBuilder::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileView_Folder)
	{
		if (!menu->FindItem(XRCID("WXFB_POPUP")))
		{
			m_separatorItem = menu->AppendSeparator();
			menu->Append(XRCID("WXFB_POPUP"), wxT("wxFormBuilder"), CreatePopupMenu());
		}
	}
	else if (type == MenuTypeFileView_File)
	{
		bool isFbpFile(false);
		TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo( TreeFileView );
		if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeFile )
		{
			if (item.m_fileName.GetExt() == wxT("fbp"))
			{
				isFbpFile = true;
			}
		}

		if (!m_openWithWxFbItem && isFbpFile)
		{
			m_openWithWxFbSepItem = menu->PrependSeparator();
			m_openWithWxFbItem = menu->Prepend(XRCID("wxfb_open"), wxT("Open with wxFormBuilder..."));
		}
	}
}

void wxFormBuilder::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileView_Folder)
	{
		wxMenuItem *item = menu->FindItem(XRCID("WXFB_POPUP"));
		if (item)
		{
			menu->Destroy(item);
			menu->Destroy(m_separatorItem);
			m_separatorItem = NULL;
		}
	}
	else if (type == MenuTypeFileView_File)
	{
		if ( m_openWithWxFbItem && m_openWithWxFbSepItem )
		{
			menu->Destroy(m_openWithWxFbItem);
			m_openWithWxFbItem = NULL;
			menu->Destroy(m_openWithWxFbSepItem);
			m_openWithWxFbSepItem = NULL;
		}
	}
}

void wxFormBuilder::UnPlug()
{
}

void wxFormBuilder::OnSettings(wxCommandEvent& e)
{
	FormBuildSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.ShowModal();
}

wxMenu* wxFormBuilder::CreatePopupMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("wxfb_new_dialog"), wxT("New wxDialog..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("wxfb_new_frame"), wxT("New wxFrame..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("wxfb_new_panel"), wxT("New wxPanel..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

void wxFormBuilder::OnNewDialog(wxCommandEvent& e)
{
	wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.SetTitle(wxT("New wxDialog"));
	if (dlg.ShowModal() == wxID_OK)
	{
		wxFBItemInfo info;
		info = dlg.GetData();
		info.kind = wxFBItemKind_Dialog;

		DoCreateWxFormBuilderProject(info);
	}
}

void wxFormBuilder::OnNewFrame(wxCommandEvent& e)
{
	wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.SetTitle(wxT("New wxFrame"));
	if (dlg.ShowModal() == wxID_OK)
	{
		wxFBItemInfo info;
		info = dlg.GetData();
		info.kind = wxFBItemKind_Frame;
		DoCreateWxFormBuilderProject(info);
	}
}

void wxFormBuilder::OnNewPanel(wxCommandEvent& e)
{
	wxFBItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.SetTitle(wxT("New wxPanel"));
	if (dlg.ShowModal() == wxID_OK)
	{
		wxFBItemInfo info;
		info = dlg.GetData();
		info.kind = wxFBItemKind_Panel;
		DoCreateWxFormBuilderProject(info);
	}
}

void wxFormBuilder::DoCreateWxFormBuilderProject(const wxFBItemInfo& data)
{
	// add new virtual folder to the selected virtual directory
	wxString formbuilderVD;
	formbuilderVD = data.virtualFolder.BeforeFirst(wxT(':'));

	m_mgr->CreateVirtualDirectory(formbuilderVD, wxT("formbuilder"));
	wxString templateFile(m_mgr->GetInstallDirectory() + wxT("/templates/formbuilder/"));

	switch (data.kind)
	{
	default:
	case wxFBItemKind_Dialog:
		templateFile << wxT("DialogTemplate.fbp");
		break;
	case wxFBItemKind_Frame:
		templateFile << wxT("FrameTemplate.fbp");
		break;
	case wxFBItemKind_Panel:
		templateFile << wxT("PanelTemplate.fbp");
		break;
	}

	wxFileName tmplFile(templateFile);
	if (!tmplFile.FileExists())
	{
		wxMessageBox(wxString::Format(wxT("Cant find wxFormBuilder template file '%s'"), tmplFile.GetFullPath().c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
		return;
	}

	// place the files under the VD's project owner
	wxString err_msg;
	wxString project = data.virtualFolder.BeforeFirst(wxT(':'));
	ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, err_msg);
	if (proj)
	{
		wxString files_path = proj->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME);
		// copy the file to here
		wxFileName fbpFile(files_path, data.file + wxT(".fbp"));
		if (!wxCopyFile(tmplFile.GetFullPath(), fbpFile.GetFullPath()))
		{
			wxMessageBox(wxString::Format(wxT("Failed to copy tempalte file to '%s'"), fbpFile.GetFullPath().c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
			return;
		}
		// open the file, and replace expand its macros
		wxString content;
		if (!ReadFileWithConversion(fbpFile.GetFullPath().c_str(), content))
		{
			wxMessageBox(wxString::Format(wxT("Failed to read file '%s'"), fbpFile.GetFullPath().c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
			return;
		}
		
		content.Replace(wxT("$(BaseFileName)"), data.file);
		content.Replace(wxT("$(ProjectName)"), data.className);
		content.Replace(wxT("$(Title)"), data.title);
		content.Replace(wxT("$(ClassName)"), data.className);
		
		if (!WriteFileWithBackup(fbpFile.GetFullPath().c_str(), content, false))
		{
			wxMessageBox(wxString::Format(wxT("Failed to write file '%s'"), fbpFile.GetFullPath().c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
			return;
		}

		// add the file to the project
		wxArrayString paths;
		paths.Add(fbpFile.GetFullPath());
		m_mgr->AddFilesToVirtualFodler(project + wxT(":formbuilder"), paths);

		// // first we launch wxFB with the -g flag set
		wxString genFileCmd;
		genFileCmd << GetWxFBPath() << wxT(" -g ") << fbpFile.GetFullPath();

		wxArrayString dummy, filesToAdd;
		ProcUtils::SafeExecuteCommand(genFileCmd, dummy);

		wxFileName cppFile(fbpFile.GetPath(), data.file + wxT(".cpp"));
		wxFileName headerFile(fbpFile.GetPath(), data.file + wxT(".h"));

		if (cppFile.FileExists())
		{
			filesToAdd.Add(cppFile.GetFullPath());
		}

		if (headerFile.FileExists())
		{
			filesToAdd.Add(headerFile.GetFullPath());
		}

		if (filesToAdd.GetCount())
		{
			m_mgr->AddFilesToVirtualFodler(data.virtualFolder, filesToAdd);
		}

		DoLaunchWxFB(fbpFile.GetFullPath());
	}
}

void wxFormBuilder::OpenWithWxFb(wxCommandEvent& e)
{
	// get the file name
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeFile )
	{
		if (item.m_fileName.GetExt() == wxT("fbp"))
		{
			DoLaunchWxFB(item.m_fileName.GetFullPath());
		}
		else
		{
			wxMessageBox(wxT("Please select a 'fbp' (Form Builder Project) file only"), wxT("CodeLite"), wxOK|wxCENTER|wxICON_INFORMATION);
			return;
		}
	}
}

void wxFormBuilder::DoLaunchWxFB(const wxString& file)
{
	wxString fbpath = GetWxFBPath();
	if (fbpath.IsEmpty())
	{
		wxMessageBox(wxT("Failed to launch wxFormBuilder, no path specified\nPlease set wxFormBuilder path from Plugins -> wxFormBuilder -> Settings..."),
		             wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
		return;
	}
	ConfFormBuilder confData;
	m_mgr->GetConfigTool()->ReadObject(wxT("wxFormBuilder"), &confData);
	wxString cmd = confData.GetCommand();
	cmd.Replace(wxT("$(wxfb)"), fbpath);
	cmd.Replace(wxT("$(wxfb_project)"), wxString::Format(wxT("\"%s\""), file.c_str()));

	// Launch !
	wxExecute(cmd);
}

wxString wxFormBuilder::GetWxFBPath()
{
	// Launch wxFB
	ConfFormBuilder confData;
	m_mgr->GetConfigTool()->ReadObject(wxT("wxFormBuilder"), &confData);
	wxString fbpath = confData.GetFbPath();

#ifdef __WXGTK__
	if (fbpath.IsEmpty())
	{
		// try to locate the file at '/usr/bin' or '/usr/local/bin'
		if (wxFileName::FileExists(wxT("/usr/local/bin/wxformbuilder")))
		{
			fbpath = wxT("/usr/local/bin/wxformbuilder");
		}
		else if (wxFileName::FileExists(wxT("/usr/bin/wxformbuilder")))
		{
			fbpath = wxT("/usr/bin/wxformbuilder");
		}
	}
#endif

#ifdef __WXMSW__
	if (fbpath.IsEmpty())
	{
		// try to locate the file at '/usr/bin' or '/usr/local/bin'
		if (wxFileName::FileExists(wxT("C:\\Program Files\\wxFormBuilder\\wxFormBuilder.exe")))
		{
			fbpath = wxT("C:\\Program Files\\wxFormBuilder\\wxFormBuilder.exe");
		}
	}
#endif
	return fbpath;
}
