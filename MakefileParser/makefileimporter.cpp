#include "precompiled_header.h"
#include "makefileimporter.h"
#include <wx/xrc/xmlres.h>
#include "workspace.h"

#include "VariableLexer.h"
#include "MakefileParser.h"
#include "LineTypes.h"
#include "Target.h"
#include "TargetLexer.h"
#include "Node.h"

static MakefileImporter* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new MakefileImporter(manager);
	}
	return thePlugin;
} 

MakefileImporter::MakefileImporter(IManager *manager)
		: IPlugin(manager)
{
	m_longName = wxT("Import makefiles to CodeLite.");
	m_shortName = wxT("MakefileImporter");
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	wxTextCtrl* analyserWindow = new wxTextCtrl(m_mgr->GetOutputPaneNotebook(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER| wxTE_MULTILINE);
	analyserWindow->SetFont(font);

	m_mgr->GetOutputPaneNotebook()->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_repo")));
	m_mgr->GetOutputPaneNotebook()->AddPage(analyserWindow, wxT("Importer"), false, (int)m_mgr->GetOutputPaneNotebook()->GetImageList()->GetCount()-1);
}

MakefileImporter::~MakefileImporter()
{
}

wxToolBar *MakefileImporter::CreateToolBar(wxWindow *parent)
{
	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(24, 24));
	
	tb->AddTool(XRCID("import_makefile"), wxT("Import Makefile..."), wxXmlResource::Get()->LoadBitmap(wxT("code_format")), wxT("Import Makefile"));
    tb->AddTool(XRCID("import_options"), wxT("Analyser Options"), wxXmlResource::Get()->LoadBitmap(wxT("code_format_options")), wxT("Source Code Formatter Options..."));
#if defined (__WXMAC__)	
	tb->AddSeparator();
#endif	
	tb->Realize();

    //Connect the events to us
    parent->Connect(XRCID("import_makefile"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MakefileImporter::OnImportMakefile), NULL, (wxEvtHandler*)this);
    parent->Connect(XRCID("import_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MakefileImporter::OnImporterOptions), NULL, (wxEvtHandler*)this);
    parent->Connect(XRCID("import_makefile"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MakefileImporter::OnImportMakefileUI), NULL, (wxEvtHandler*)this);
    parent->Connect(XRCID("import_options"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MakefileImporter::OnImporterOptionsUI), NULL, (wxEvtHandler*)this);
    return tb;
}

void MakefileImporter::OnImportMakefileUI(wxUpdateUIEvent &event)
{
	event.Enable(true);
}

void MakefileImporter::OnImportMakefile(wxCommandEvent &event)
{
	wxUnusedVar(event);
	const wxString ALL(wxT("Import makefile files (Makefile) |Makefile|")
	                   wxT("All Files (*)|*"));
	
	// TODO: parent = NULL?
	wxFileDialog *dlg = new wxFileDialog(NULL, wxT("Open Makefile"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);
	
	if (dlg->ShowModal() == wxID_OK) {
		ImportFromMakefile(dlg->GetPath());
	}
	dlg->Destroy();
}

void MakefileImporter::OnImporterOptions(wxCommandEvent &e) 
{
	wxUnusedVar(e);
	
	/*
	ReferenceAnalyserOptionsDlg *dlg = new ReferenceAnalyserOptionsDlg(NULL, m_options);
	
	if (dlg->ShowModal() == wxID_OK) {
		m_options = dlg->GetOptions();
		m_mgr->GetConfigTool()->WriteObject(wxT("AnalyserOptions"), &m_options);
	}
	
	dlg->Destroy();
	*/
}

void MakefileImporter::OnImporterOptionsUI(wxUpdateUIEvent &e) 
{
    e.Enable(true);
}

void MakefileImporter::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("import_makefile"), wxT("Import Makefile"), wxT("Import Makefile"), wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("import_options"), wxT("Options..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Makefile Importer"), menu);

}

void MakefileImporter::LogMessage(const wxString& message)
{
	// Copy from SvnDriver::PrintMessage()
	
	wxFlatNotebook *book = m_mgr->GetOutputPaneNotebook();
	wxTextCtrl *analyserWindow (NULL);
	
	size_t position;
	for (position = 0; position < (size_t)book->GetPageCount(); position++) {
		if (book->GetPageText(position) == wxT("Importer")) {
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

void MakefileImporter::ClearMessagePane()
{
	wxFlatNotebook *book = m_mgr->GetOutputPaneNotebook();
	wxTextCtrl *analyserWindow (NULL);
	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("Importer")) {
			analyserWindow = dynamic_cast<wxTextCtrl*>(book->GetPage(i));
			break;
		}
	}

	if (analyserWindow) {
		analyserWindow->Clear();
	}
}

/*
void MakefileImporter::ImportFromMakefile(const wxString &path)
{
	LogMessage(path + wxT("\n"));

	wxFileName fileName = path;
	LogMessage(fileName.GetPath() + wxT("\n"));
	
	VariableLexer expander(path.data());
	wxArrayString expanded = expander.getResult();

	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();

	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	wxString errorMsg;
	m_mgr->GetWorkspace()->CreateWorkspace(wxT("import_from_") + fileName.GetName(), fileName.GetPath(), errorMsg);
	
	if(errorMsg.size())
	{
		LogMessage(errorMsg);
		return;
	}

	wxArrayString extentions;
	extentions.Add(wxT(".h"));
	extentions.Add(wxT(".hpp"));
	extentions.Add(wxT(".c"));
	extentions.Add(wxT(".cc"));
	extentions.Add(wxT(".cpp"));
	extentions.Add(wxT(".cxx"));

	for (size_t i = 0; i < lexed.size(); i++) {
		Target targ = lexed[i];
		wxArrayString deps = targ.getDeps();
		wxString name = targ.getName();
		LogMessage(wxT("Processing target ") + name + wxT(".\n"));
				
		size_t dotPos = dep.find_last_of(wxT("."));
		if(dotPos == wxNOT_FOUND)
		{			
			ProjectPtr proj(new Project());
			proj->Create(name, fileName.GetPath(), wxT("importedProject"));
			proj->SetSettings(new ProjectSettings(NULL));

			for (size_t j = 0; j < deps.size(); j++) {
				wxString dep = deps[j];
				size_t dotPos = dep.find_last_of(wxT("."));
				if(dotPos == wxNOT_FOUND)
				{
					LogMessage(wxT("No Extension found.\n"));
					continue;
				}
				
				wxString ext = dep.Mid(dotPos);
				size_t pos = extentions.Index(ext);
				
				if(pos != wxNOT_FOUND) {
					wxString file = dep.Left(dep.size()-2);
					wxString ext = extentions[pos];
					wxFileName fileName = proj->GetFileName().GetPathWithSep() + file + ext;
					if (fileName.FileExists()) {
						bool added = proj->AddFile(fileName.GetFullPath(), wxT("Source Files"));
						if (!added) {
							LogMessage(wxT("Could not add file: '") + fileName.GetFullPath() + wxT("'.\n"));
						}
					}				
				} else if (deps.Index(dep, false) != wxNOT_FOUND) { // look for inter-project dependency
					wxArrayString dependencies= proj->GetDependencies();
					dependencies.Add(dep);
					proj->SetDependencies(dependencies);
				} 
			}
			
			proj->Save();
			wxString errMsg;
			wxString path = proj->GetFileName().GetFullPath();
			m_mgr->GetWorkspace()->AddProject(path, errMsg);
			
			if(errMsg.length())
			{
				LogMessage(path);
				LogMessage(errMsg);
			}
		}
		else {
			wxString ext = dep.Mid(dotPos);
		}
	}
	return;
}
*/

void MakefileImporter::ImportFromMakefile(const wxString &path)
{
	LogMessage(path + wxT("\n"));

	wxFileName fileName = path;
	LogMessage(fileName.GetPath() + wxT("\n"));
	
	Tokens tokens;
	tokens["RM"] = "rm";
	tokens["MAKE"] = "make";

	VariableLexer expander(path.data(), tokens);
	wxArrayString expanded = expander.getResult();

	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();

	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	
	MakefileNode* result = NULL;

	for(Targets::iterator it = lexed.begin(); it != lexed.end(); it++)
	{				
		Target* target = *it;
		
		if(result == NULL)
			result = new MakefileNode(NULL, target);
		else
			result->addNode(target);
	}
	
	LogMessage(result->toString());
}

void MakefileImporter::HookPopupMenu(wxMenu *menu, MenuType type)
{
}

void MakefileImporter::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
}

void MakefileImporter::UnPlug()
{
}
