#include "referenceanalyser.h"
#include "referenceanalyseroptionsdlg.h"
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/progdlg.h>
#include <vector>

#include "ctags_manager.h" // for TagsManager

static ReferenceAnalyser* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new ReferenceAnalyser(manager);
	}
	return thePlugin;
}

ReferenceAnalyser::ReferenceAnalyser(IManager *manager)
		: IPlugin(manager)
{
	m_longName = wxT("Reference Analyser using Codelite's CC.");
	m_shortName = wxT("ReferenceAnalyser");
	
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	wxTextCtrl* analyserWindow = new wxTextCtrl(m_mgr->GetOutputPaneNotebook(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER| wxTE_MULTILINE);
	analyserWindow->SetFont(font);

	m_mgr->GetOutputPaneNotebook()->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_repo")));
	m_mgr->GetOutputPaneNotebook()->AddPage(analyserWindow, wxT("Analyser"), false, (int)m_mgr->GetOutputPaneNotebook()->GetImageList()->GetCount()-1);
}

ReferenceAnalyser::~ReferenceAnalyser()
{
}

wxToolBar *ReferenceAnalyser::CreateToolBar(wxWindow *parent)
{
	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(24, 24));
	
	tb->AddTool(XRCID("analyse_references"), wxT("Analyse References..."), wxXmlResource::Get()->LoadBitmap(wxT("code_format")), wxT("Format Source Code"));
    tb->AddTool(XRCID("analyser_options"), wxT("Analyser Options"), wxXmlResource::Get()->LoadBitmap(wxT("code_format_options")), wxT("Source Code Formatter Options..."));
#if defined (__WXMAC__)	
	tb->AddSeparator();
#endif	
	tb->Realize();

    //Connect the events to us
    parent->Connect(XRCID("analyse_references"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ReferenceAnalyser::OnAnalyse), NULL, (wxEvtHandler*)this);
    parent->Connect(XRCID("analyser_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ReferenceAnalyser::OnAnalyserOptions), NULL, (wxEvtHandler*)this);
    parent->Connect(XRCID("analyse_references"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReferenceAnalyser::OnAnalyseUI), NULL, (wxEvtHandler*)this);
    parent->Connect(XRCID("analyser_options"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReferenceAnalyser::OnAnalyserOptionsUI), NULL, (wxEvtHandler*)this);
    return tb;
}

void ReferenceAnalyser::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("analyse_references"), wxT("Analyse References"), wxT("Analyse References"), wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("analyser_options"), wxT("Options..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Reference Analyser"), menu);
}

void ReferenceAnalyser::HookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(type);
	wxUnusedVar(menu);
}

void ReferenceAnalyser::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(type);
	wxUnusedVar(menu);
}

void ReferenceAnalyser::UnPlug()
{
	//nothing to do
}

void ReferenceAnalyser::OnAnalyserOptions(wxCommandEvent &e) {
	wxUnusedVar(e);
	
	ReferenceAnalyserOptionsDlg *dlg = new ReferenceAnalyserOptionsDlg(NULL, m_options);
	
	if (dlg->ShowModal() == wxID_OK) {
		m_options = dlg->GetOptions();
		m_mgr->GetConfigTool()->WriteObject(wxT("AnalyserOptions"), &m_options);
	}
	
	dlg->Destroy();
}

void ReferenceAnalyser::OnAnalyseUI(wxUpdateUIEvent &e) {
    e.Enable(true);
}

void ReferenceAnalyser::OnAnalyserOptionsUI(wxUpdateUIEvent &e) {
    e.Enable(true);
}

void ReferenceAnalyser::LogMessage(const wxString& message)
{
	// Copy from SvnDriver::PrintMessage()
	
	wxFlatNotebook *book = m_mgr->GetOutputPaneNotebook();
	wxTextCtrl *analyserWindow (NULL);
	
	size_t position;
	for (position = 0; position < (size_t)book->GetPageCount(); position++) {
		if (book->GetPageText(position) == wxT("Analyser")) {
			analyserWindow = dynamic_cast<wxTextCtrl*>(book->GetPage(position));
			break;
		}
	}

	if (analyserWindow) {
		wxString text(message);
		text += wxT("\n");
		analyserWindow->AppendText(text);
		//make the appended line visible
		analyserWindow->ShowPosition(analyserWindow->GetLastPosition()); 
		book->SetSelection(position);
	}
}

void ReferenceAnalyser::ClearMessagePane()
{
	wxFlatNotebook *book = m_mgr->GetOutputPaneNotebook();
	wxTextCtrl *analyserWindow (NULL);
	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("Analyser")) {
			analyserWindow = dynamic_cast<wxTextCtrl*>(book->GetPage(i));
			break;
		}
	}

	if (analyserWindow) {
		analyserWindow->Clear();
	}
}

void ReferenceAnalyser::OnAnalyse(wxCommandEvent &e) {
    wxUnusedVar(e);
	
	m_classMembersGraph.clear();
	ClearMessagePane();
	
	std::vector<TagEntryPtr> tags;
	m_mgr->GetTagsManager()->GetClasses(tags, true); // true for only workspace
	
	if(tags.size() == 0)
	{
		LogMessage(wxT("Could not find any classes in workspace."));
		return;
	}
	
	AnalyseTags(tags);
	PrintGraph();
	PrintResult();
}

void ReferenceAnalyser::AnalyseTags(std::vector<TagEntryPtr>& tags) 
{
	wxProgressDialog* prgDlg = NULL;
	int maxVal = (int)tags.size();

	// Create a progress dialog
	prgDlg = new wxProgressDialog (wxT("Analysing class references..."), 
	wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), 
		maxVal, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
		
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();

	prgDlg->Update(0, wxT("Analysing..."));

	int i = 0;
	for(std::vector<TagEntryPtr>::iterator it = tags.begin(); it != tags.end(); it++)
	{
		i++;
		
		TagEntryPtr tag = *it;
		if(tag->GetKind() != wxT("class"))
			continue;
			
		// update the progress bar
		wxString msg;
		msg << wxT("Analysing class: '") + tag->GetName() + wxT("'.");
		bool abort = !prgDlg->Update(i, msg);
		
		if(abort)
			break;
			
		Analyse(tag->GetName());
	}
	
	prgDlg->Destroy();
}
	

void ReferenceAnalyser::Analyse(const wxString& theclassname)
{
	wxString classname = theclassname;
	m_classMembersGraph.addNode(theclassname);
	
	typedef std::vector<TagEntryPtr> TagEntryVector;
	TagEntryVector tags;

	m_mgr->GetTagsManager()->TagsByScope(classname, tags);
	
	// LogMessage(classname << wxT(": ") << tags.size() << wxT("."));
	
	wxString type, typeScope;
	
	for(TagEntryVector::iterator it = tags.begin(); it != tags.end(); it++)
	{
		TagEntryPtr tag = *it;
		wxString kind = tag->GetKind();
		if(kind != wxT("member"))
			continue;
		
		wxString name = tag->GetName();
		
		wxString type; // the type of this tag
		wxString typeScope; // the scope of this tag
		
		wxString expressionForType(theclassname);
		expressionForType << wxT("::") << name << wxT(".");
		
		bool worked = m_mgr->GetTagsManager()->ProcessExpression(expressionForType, type, typeScope);
		if(!worked)
		{
			LogMessage(wxT("Could not retreive type of: ") + theclassname + wxT("::") + name);
			continue;
		}
		
		if(type != m_options.getContainerName())
			continue;

		wxString expressionForSmartPtr(theclassname);
		expressionForSmartPtr << wxT("::") << name << m_options.getContainerOperator();
		
		worked = m_mgr->GetTagsManager()->ProcessExpression(expressionForSmartPtr, type, typeScope);
				
		m_classMembersGraph.addNode(type);
		m_classMembersGraph.addEdge(theclassname, type);
	}
}

void ReferenceAnalyser::PrintGraph(const wxString& classname)
{
	if(classname == wxEmptyString)
	{
		// Recursively print all classes
		for(Graph<wxString>::Edges::const_iterator it = m_classMembersGraph.begin(); it != m_classMembersGraph.end(); it++)
		{
			wxString name = it->first;
			if(name == wxEmptyString)
				continue;
				
			PrintGraph(name);
		}
		
		return;
	}
	
	wxStringGraph::EdgesIterator it = m_classMembersGraph.find(classname);
	if(it == m_classMembersGraph.end())
	{
		LogMessage(wxT("ReferenceAnalyser::Print(), unknown class '") + classname + wxT("'."));
		return;
	}
	
	wxStringGraph::Nodes members = it->second;
	if(members.size() == 0)
	{
		// LogMessage(wxT("Class '") + classname + wxT("' has no members."));
		return;
	}
	
	// LogMessage(wxT("Class '") + classname + wxT("' has the references to the following classes:"));
	for(wxStringGraph::NodesIterator it = members.begin(); it != members.end(); it++)
	{
		LogMessage(classname + wxT(" -> ") + *it);
	}
}

void ReferenceAnalyser::PrintResult()
{
	if(m_classMembersGraph.isEmpty())
	{
		LogMessage(wxT("None of the analysed classes contained any references."));
		LogMessage(wxT("Did you properly configure the Container Class? (see options)"));
		return;
	}
	
	if(m_classMembersGraph.isAcyclic())
	{
		LogMessage(wxT("You have no cycles in your dependency graph."));
		return;
	}
	
	LogMessage(wxT("Your dependency graph is not acyclic:"));

	wxString result;
	wxStringGraph::ConstNodeVectorRef path = m_classMembersGraph.getPath();
	for(int i = 0; i < path.size(); i++)
	{
		wxString node = path[i];
		
		result += node;
		result += wxT(" -> ");
	}
	
	result += path[0];
	
	LogMessage(result);
}
