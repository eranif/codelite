#include "dirsaver.h"
#include "cscope.h"
#include "procutils.h"
#include "wx/ffile.h"
#include "workspace.h"
#include <wx/xrc/xmlres.h>

static Cscope* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new Cscope(manager);
	}
	return thePlugin;
}

Cscope::Cscope(IManager *manager)
		: IPlugin(manager)
		, m_topWindow(NULL)
		, m_thread(NULL)
{
	m_longName = wxT("Cscope Integration for CodeLite");
	m_shortName = wxT("Cscope");
	m_topWindow = wxTheApp;

	Connect(wxEVT_CSCOPE_THREAD_DB_BUILD_DONE, wxCommandEventHandler(Cscope::OnDbBuilderThreadEnded), NULL, this);
}

Cscope::~Cscope()
{
}

wxToolBar *Cscope::CreateToolBar(wxWindow *parent)
{
	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	// TODO :: Add your toolbar items here...
	// Sample code that adds single button to the toolbar
	// and associates an image to it
	// tb->AddTool(XRCID("new_plugin"), wxT("New Plugin Wizard..."), wxXmlResource::Get()->LoadBitmap(wxT("plugin_add")), wxT("New Plugin Wizard..."));
	// tb->Realize();
	// Connect the events to us
	// parent->Connect(XRCID("new_plugin"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnNewPlugin), NULL, (wxEvtHandler*)this);
	// parent->Connect(XRCID("new_plugin"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnNewPluginUI), NULL, (wxEvtHandler*)this);
	return tb;
}

void Cscope::CreatePluginMenu(wxMenu *pluginsMenu)
{
	//TODO:: create the menu for the 'Plugin' menu entry in the
	//menu bar
}

void Cscope::HookPopupMenu(wxMenu *menu, MenuType type)
{
	//at first, we hook cscope into the editor's context menu
	if (type == MenuTypeEditor) {
		m_sepItem = menu->AppendSeparator();
		menu->Append(XRCID("CSCOPE_EDITOR_POPUP"), wxT("cscope"), CreateEditorPopMenu());
	}
}

void Cscope::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		wxMenuItem *item = menu->FindItem(XRCID("CSCOPE_EDITOR_POPUP"));
		if (item) {
			menu->Destroy(item);
			m_topWindow->Disconnect(XRCID("cscope_build_database"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnBuildDatabse), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
		}

		if (m_sepItem) {
			menu->Destroy(m_sepItem);
			m_sepItem = NULL;
		}
	}
}

void Cscope::UnPlug()
{
	//TODO:: perform the unplug action for this plugin
}

//---------------------------------------------------------------------------------
wxMenu *Cscope::CreateEditorPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cscope_build_database"), wxT("&Build Database ..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), wxT("&Find this C symbol"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_find_global_definition"), wxT("Find this &global definition"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"), wxT("Find functions &called by this function"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"), wxT("Fi&nd functions calling this function"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	//connect the events
	m_topWindow->Connect(XRCID("cscope_build_database"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnBuildDatabse), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	return menu;
}

void Cscope::OnBuildDatabse(wxCommandEvent &e)
{
	//avoid reentrance
	if (m_thread) {
		return;
	}

	DoCreateDatabase();
}

wxString Cscope::DoCreateListFile()
{
	wxArrayString projects;
	m_mgr->GetWorkspace()->GetProjectList(projects);
	wxString err_msg;
	std::vector< wxFileName > files;
	for (size_t i=0; i< projects.GetCount(); i++) {
		ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
		if ( proj ) {
			proj->GetFiles(files, true);
		}
	}

	//create temporary file and save the file there
	wxString list_file = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(true);
	list_file << wxT("cscope_file.list");

	wxFFile file(list_file, wxT("w+b"));
	if (!file.IsOpened()) {
		wxLogMessage(wxT("Failed to open temporary file ") + list_file);
		return wxEmptyString;
	}

	//write the content of the files into the tempfile
	for (size_t i=0; i< files.size(); i++) {
		wxFileName fn(files.at(i));
		file.Write( fn.GetFullPath() + wxT("\n") );
	}

	file.Flush();
	file.Close();
	return list_file;
}

void Cscope::DoCreateDatabase()
{
	wxString command;
	command << GetCscopeExeName() << wxT(" -b -u -i ");

	//create a list of files to scan
	wxString list_file = DoCreateListFile();
	if(list_file.IsEmpty()){return;}
	
	command << wxT("\"") << list_file << wxT("\"");
	
	wxLogMessage(wxT("Building cscope database ..."));
	//create the worker therad and launch it
	m_thread = new CscopeDbBuilderThread(this);

	//initialise it with the command and the output file
	m_thread->SetCmd(command);
	m_thread->SetWorkingDir(m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(true));
	m_thread->Create();
	m_thread->Run();
}

void Cscope::OnFindSymbol(wxCommandEvent &e)
{
	//try to locate the cscope database
	if (m_thread) {	return; }

	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if(word.IsEmpty()) {return;}
	wxString list_file = DoCreateListFile();
	
	//Do the actual search
	wxString command;
	
	command << GetCscopeExeName() << wxT(" -L -0 ") << word << wxT(" -i ") << list_file;
	wxArrayString output;
	
	//create the search thread and return
	m_thread = new CscopeDbBuilderThread(this);
	m_thread->SetCmd(command);
	m_thread->SetWorkingDir(m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(true));
	
	m_thread->Create();
	m_thread->Run();
}

void Cscope::OnFindGlobalDefinition(wxCommandEvent &e)
{
}

void Cscope::OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e)
{
}

void Cscope::OnFindFunctionsCallingThisFunction(wxCommandEvent &e)
{
}
wxString Cscope::GetCscopeExeName()
{
	return wxT("cscope ");
}

void Cscope::OnDbBuilderThreadEnded(wxCommandEvent &e)
{
	wxLogMessage(wxT("cscope thread terminated execution of command: ") + e.GetString());
	
	wxString o;
	wxArrayString *poutput = (wxArrayString*)e.GetClientData();
	for(size_t i=0; i< poutput->GetCount(); i++) {
		o << poutput->Item(i) << wxT("\n");
	}
	
	delete poutput;
	wxLogMessage(o);

	//release the resources
	delete m_thread;
	m_thread = NULL;
}
