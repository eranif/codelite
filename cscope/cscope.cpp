#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "dirsaver.h"
#include "cscope.h"
#include "procutils.h"
#include "wx/ffile.h"
#include "workspace.h"
#include <wx/xrc/xmlres.h>
#include "cscopetab.h"
#include "cscopedbbuilderthread.h"

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
{
	m_longName = wxT("Cscope Integration for CodeLite");
	m_shortName = wxT("Cscope");
	m_topWindow = wxTheApp;

	//add window to the output pane
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	//m_mgr->GetOutputPaneNotebook()->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_repo")));
	m_cscopeWin = new CscopeTab(m_mgr->GetOutputPaneNotebook(), m_mgr);
	m_mgr->GetOutputPaneNotebook()->AddPage(m_cscopeWin, wxT("cscope"), false/*, (int)m_mgr->GetOutputPaneNotebook()->GetImageList()->GetCount()-1*/);

	Connect(wxEVT_CSCOPE_THREAD_DB_BUILD_DONE, wxCommandEventHandler(Cscope::OnDbBuilderThreadEnded), NULL, this);
	
	//start the helper thread
	CScopeThreadST::Get()->Start();
}

Cscope::~Cscope()
{
}

wxToolBar *Cscope::CreateToolBar(wxWindow *parent)
{
	wxToolBar *tb = NULL;
	// tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
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
	CScopeThreadST::Get()->Stop();
	CScopeThreadST::Free();
}

//---------------------------------------------------------------------------------
wxMenu *Cscope::CreateEditorPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), wxT("&Find this C symbol"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_find_global_definition"), wxT("Find this &global definition"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"), wxT("Find functions &called by this function"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"), wxT("Fi&nd functions calling this function"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	//connect the events
	m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	return menu;
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

void Cscope::DoCscopeCommand(const wxString &command)
{
	//try to locate the cscope database
	wxArrayString output;

	//create the search thread and return
	CscopeRequest *req = new CscopeRequest();
	req->SetOwner(this);
	req->SetCmd(command);
	req->SetWorkingDir(m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(true));
	
	CScopeThreadST::Get()->Add( req );
}

void Cscope::OnFindSymbol(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {return;}
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	command << GetCscopeExeName() << wxT(" -L -0 ") << word << wxT(" -i ") << list_file;
	DoCscopeCommand(command);
}

void Cscope::OnFindGlobalDefinition(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {return;}
	
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	command << GetCscopeExeName() << wxT(" -L -1 ") << word << wxT(" -i ") << list_file;
	DoCscopeCommand(command);
}

void Cscope::OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {return;}
	
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	command << GetCscopeExeName() << wxT(" -L -2 ") << word << wxT(" -i ") << list_file;
	DoCscopeCommand(command);	
}

void Cscope::OnFindFunctionsCallingThisFunction(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {return;}
	
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	command << GetCscopeExeName() << wxT(" -L -3 ") << word << wxT(" -i ") << list_file;
	DoCscopeCommand(command);	
}
wxString Cscope::GetCscopeExeName()
{
	return wxT("cscope ");
}

void Cscope::OnDbBuilderThreadEnded(wxCommandEvent &e)
{
	wxLogMessage(wxT("cscope thread terminated execution of command: ") + e.GetString());
	CscopeResultTable *result = (CscopeResultTable*)e.GetClientData();
	m_cscopeWin->BuildTable( result );

	//set the focus to the cscope tab
	wxFlatNotebook *book = m_mgr->GetOutputPaneNotebook();
	wxString curSel = book->GetPageText((size_t)book->GetSelection());
	if (curSel != wxT("cscope")) {
		for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
			if (book->GetPageText(i) == wxT("cscope")) {
				book->SetSelection(i);
				break;
			}
		}
	}
	
	//release the resources
}
