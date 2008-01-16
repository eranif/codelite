#include "output_pane.h"
#include <wx/xrc/xmlres.h>
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "manager.h"
#include "regex_processor.h"
#include "build_settings_config.h"
#include "dirsaver.h"
#include "macros.h"
#include "shell_window.h"

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

const wxString OutputPane::FIND_IN_FILES_WIN = wxT("Find Results");
const wxString OutputPane::BUILD_WIN         = wxT("Build");
const wxString OutputPane::OUTPUT_WIN        = wxT("Output");
const wxString OutputPane::OUTPUT_DEBUG       = wxT("Debug");

BEGIN_EVENT_TABLE(OutputPane, wxPanel)
EVT_MENU(XRCID("next_error"), OutputPane::OnNextBuildError)
END_EVENT_TABLE()

OutputPane::OutputPane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
, m_caption(caption)
, m_canFocus(true)
, m_logTargetOld(NULL)
, m_nextBuildError_lastLine(wxNOT_FOUND)
{
	CreateGUIControls();	
}

void OutputPane::OnPaint(wxPaintEvent &event){
	wxPaintDC dc(this);
	event.Skip();
}

OutputPane::~OutputPane()
{
	delete wxLog::SetActiveTarget(m_logTargetOld);
	m_book->DeleteAllPages();
}

void OutputPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	long style = wxFNB_NO_X_BUTTON | 
				 wxFNB_NO_NAV_BUTTONS | 
				 wxFNB_DROPDOWN_TABS_LIST | 
				 wxFNB_FF2 | 
				 wxFNB_CUSTOM_DLG | 
				 wxFNB_BACKGROUND_GRADIENT | 
				 wxFNB_TABS_BORDER_SIMPLE |
				 wxFNB_BOTTOM ; 
				 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 1);

	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("find_results")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("build")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("output_win")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("debug_window")));
	m_images.Add(wxXmlResource::Get()->LoadBitmap(wxT("debugger_tab")));
	m_book->SetImageList( &m_images );

	// Create the 'Find In Files Window'
	OutputPaneWinodw *findInFilesWin = new OutputPaneWinodw(m_book, wxID_ANY, FIND_IN_FILES_WIN);
	OutputPaneWinodw *buildWin = new OutputPaneWinodw(m_book, wxID_ANY, BUILD_WIN);
	m_outputDebug = new OutputPaneWinodw(m_book, wxID_ANY, OUTPUT_DEBUG, WINTYPE_SHELL);
	m_outputWind = new OutputPaneWinodw(m_book, wxID_ANY, OUTPUT_WIN, WINTYPE_SHELL);
	
	findInFilesWin->Connect(wxID_ANY, wxEVT_SCI_DOUBLECLICK, wxScintillaEventHandler(OutputPane::OnMouseDClick), NULL, this);
	findInFilesWin->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(OutputPane::OnSetFocus), NULL, this);
	buildWin->Connect(wxID_ANY, wxEVT_SCI_DOUBLECLICK, wxScintillaEventHandler(OutputPane::OnMouseDClick), NULL, this);
	buildWin->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(OutputPane::OnSetFocus), NULL, this);
	
	m_book->AddPage(findInFilesWin, FIND_IN_FILES_WIN, true, 0);
	m_book->AddPage(buildWin, BUILD_WIN, false, 1);
	m_book->AddPage(m_outputWind, OUTPUT_WIN, false, 2);
	m_book->AddPage(m_outputDebug, OUTPUT_DEBUG, false, 4);
	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );
	
	m_book->AddPage(text, wxT("Trace"), false, 3);

	mainSizer->Fit(this);
	mainSizer->Layout();
}

void OutputPane::OnSetFocus(wxFocusEvent &event)
{
	if( m_canFocus ){
		return;
	}

	wxWindow *prevFocusWin = event.GetWindow();
	if( prevFocusWin ){
		prevFocusWin->SetFocus();
	}
	event.Skip();
}

void OutputPane::SelectTab(const wxString &tabName)
{
	int index = CaptionToIndex(tabName);
	if( index == wxNOT_FOUND )
		return;
	
	if(index != m_book->GetSelection()){
		m_book->SetSelection((size_t)index);
	}
}

void OutputPane::AppendText(const wxString &winName, const wxString &text)
{
	int index = CaptionToIndex(winName);
	if( index == wxNOT_FOUND )
		return;

	wxWindow *w = m_book->GetPage((size_t)index);
	OutputPaneWinodw *window = dynamic_cast<OutputPaneWinodw*>(w);
	if(window){
		if(window->GetCaption() == BUILD_WIN){
			wxScintilla *sci = (wxScintilla *)window->GetWindow();
			//Keep information about this line
			if(text.Contains(wxT("----------Building project:"))){
				LineInfo info;

				info.project = text.AfterFirst(wxT('['));
				info.project = info.project.BeforeFirst(wxT('-'));
				TrimString(info.project);

				info.configuration = text.AfterFirst(wxT('['));
				info.configuration = info.configuration.AfterFirst(wxT('-'));
				info.configuration = info.configuration.BeforeFirst(wxT(']'));
				TrimString(info.configuration);

				//keep the information about this line
				//we use here GetLineCount()-1 since an empty docuement contains
				//1 line, but scintilla refers to lines from 0
				m_lineInfo[sci->GetLineCount()-1] = info;
			}else{
				//set this line with the previous project 
				int lineno = sci->GetLineCount()-1;
				std::map<int, LineInfo>::iterator iter = m_lineInfo.find(lineno-1);
				if(iter != m_lineInfo.end()){
					m_lineInfo[lineno] = iter->second;
				}
			}
		}

		window->AppendText(text);
	}
}

void OutputPane::Clear()
{
	int index = m_book->GetSelection();
	if( index == wxNOT_FOUND )
		return;

	wxWindow *w = m_book->GetPage((size_t)index);
	OutputPaneWinodw *win = dynamic_cast<OutputPaneWinodw*>(w);
	if(win){
		
		if(win->GetCaption() == BUILD_WIN){
			m_lineInfo.clear();
		}

		win->Clear();
		m_nextBuildError_lastLine = wxNOT_FOUND;
	}
}

int OutputPane::CaptionToIndex(const wxString &caption)
{
	int i = 0;
	for(; i<m_book->GetPageCount(); i++)
	{
		if(m_book->GetPageText((size_t)i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

void OutputPane::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
	int fifWinIndex = CaptionToIndex(OutputPane::FIND_IN_FILES_WIN);
	int buildWinIndex = CaptionToIndex(OutputPane::BUILD_WIN);

	OutputPaneWinodw *page = dynamic_cast<OutputPaneWinodw*>(m_book->GetPage(m_book->GetSelection()));
	if( !page ){
		return;
	}

	wxScintilla *win = dynamic_cast<wxScintilla*>( page->GetWindow() );
	if( !win ){
		return;
	}

	int line = win->LineFromPosition(pos);
	wxString lineText = win->GetLine(line);

	//remove selection
	win->SetSelectionStart(pos);
	win->SetSelectionEnd(pos);
	if( fifWinIndex == m_book->GetSelection() )
	{
		//Find in files
		OnFindInFilesDClick(lineText);
	}
	else if(buildWinIndex == m_book->GetSelection())
	{
		//build window
		lineText.Replace(wxT("\\"), wxT("/"));
		OnBuildWindowDClick(lineText, line);
	}
}

void OutputPane::OnFindInFilesDClick(const wxString &line)
{
	// each line has the format of 
	// file(line, col): text
	wxString fileName = line.BeforeFirst(wxT('('));
	wxString strLineNumber = line.AfterFirst(wxT('('));
	strLineNumber = strLineNumber.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();
	long lineNumber = -1;
	strLineNumber.ToLong(&lineNumber);

	// open the file in the editor
	ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 );
}

bool OutputPane::OnBuildWindowDClick(const wxString &line, int lineClicked)
{
	wxString fileName, strLineNumber;
	wxString compilerType(wxEmptyString);
	LineInfo info;
	bool match = false;

	//lineClicked is zero based.
	//Get the project name that we are currently compiling:
	std::map<int, LineInfo>::iterator iter = m_lineInfo.find(lineClicked);
	if(iter != m_lineInfo.end()){
		info = iter->second;

		//get the project 
		ProjectPtr proj = ManagerST::Get()->GetProject(info.project);
		if(proj){
			ProjectSettingsPtr settings = proj->GetSettings();
			if(settings){
				BuildConfigPtr bldConf = settings->GetBuildConfiguration(info.configuration);
				if(bldConf){
					compilerType = bldConf->GetCompilerType();
				}else{
					//try to get the first build configuration
					ProjectSettingsCookie cookie;
					bldConf = settings->GetFirstBuildConfiguration(cookie);
					if(bldConf){
						compilerType = bldConf->GetCompilerType();
					}
				}
			}
		}
	}
	
	//get the selected compiler for the current line that was DClicked
	//find the project selected build configuration for the workspace selected
	//configuration
	CompilerPtr cmp;
	cmp = BuildSettingsConfigST::Get()->GetCompiler(compilerType);
	if(!cmp){
		return false;
	}
	long idx;
	RegexProcessor re(cmp->GetWarnPattern());
	cmp->GetWarnFileNameIndex().ToLong(&idx);
	if(re.GetGroup(line, idx, fileName))
	{
		//we found the file name, get the line number
		cmp->GetWarnLineNumberIndex().ToLong(&idx);
		re.GetGroup(line, idx, strLineNumber);
		match = true;
	}

	//try to match warning pattern
	if(!match)
	{
		//try to match an error pattern to the line
		RegexProcessor re(cmp->GetErrPattern());
		cmp->GetErrFileNameIndex().ToLong(&idx);
		if(re.GetGroup(line, idx, fileName))
		{
			//we found the file name, get the line number
			cmp->GetErrLineNumberIndex().ToLong(&idx);
			re.GetGroup(line, idx, strLineNumber);
			match = true;
		}
	}

	if(match)
	{
		long lineNumber = -1;
		strLineNumber.ToLong(&lineNumber);

		//do an educated guess for the reason why this failed
		wxFileName fn(fileName);
		
		Manager *mgr = ManagerST::Get();
		std::vector<wxFileName> files;
		mgr->GetWorkspaceFiles(files, true);
		for(size_t i=0; i<files.size(); i++){
			if(files.at(i).GetFullName() == fn.GetFullName()){
				//we have a match
				mgr->OpenFile(files.at(i).GetFullPath(), wxEmptyString, (int)lineNumber-1);

				int buildWinIndex = CaptionToIndex(OutputPane::BUILD_WIN);
				OutputPaneWinodw *winPane = dynamic_cast<OutputPaneWinodw*>(m_book->GetPage(buildWinIndex));
				if( winPane ){
					wxScintilla *win = dynamic_cast<wxScintilla*>(winPane->GetWindow());
					if( win && (lineClicked != wxNOT_FOUND || m_nextBuildError_lastLine != wxNOT_FOUND)){
						if(lineClicked != wxNOT_FOUND){
							//the call came from mouse double click, update
							//the m_nextBuildError_lastLine member
							m_nextBuildError_lastLine = lineClicked;
						}

						win->MarkerDeleteAll(0x7);
						win->MarkerAdd(m_nextBuildError_lastLine, 0x7);
						win->GotoLine(m_nextBuildError_lastLine);
					}
				}
			}
		}
	}
	
	return match;
}

void OutputPane::OnNextBuildError(wxCommandEvent &event) 
{
	wxUnusedVar(event);
	// get handle to the build window
	int buildWinIndex = CaptionToIndex(OutputPane::BUILD_WIN);
	OutputPaneWinodw *winPane = dynamic_cast<OutputPaneWinodw*>(m_book->GetPage(buildWinIndex));
	if( !winPane ){
		return;
	}
	
	wxScintilla *win = dynamic_cast<wxScintilla*>(winPane->GetWindow());
	if( !win || win->GetLineCount()<=0 ){
		return;
	}

	// iterate through the lines, keeping the last known one
	// and pass the text on to OnBuildWindowDClick
	int startAt;
	if ((m_nextBuildError_lastLine < 0) ||
		(m_nextBuildError_lastLine >= win->GetLineCount()))
		startAt = 0;
	else startAt = m_nextBuildError_lastLine+1;
	
	bool match(false);
	for (m_nextBuildError_lastLine = startAt; m_nextBuildError_lastLine < win->GetLineCount(); 	++m_nextBuildError_lastLine) {
		wxString lineText = win->GetLine(m_nextBuildError_lastLine);
		lineText.Replace(wxT("\\"), wxT("/"));
		if (OnBuildWindowDClick(lineText, m_nextBuildError_lastLine)) {
			match = true;
			break;
		}
	}

	//no match? try from top again
	if(!match && startAt > 0){
		for (m_nextBuildError_lastLine = 0; m_nextBuildError_lastLine < startAt; 	++m_nextBuildError_lastLine) {
			wxString lineText = win->GetLine(m_nextBuildError_lastLine);
			lineText.Replace(wxT("\\"), wxT("/"));
			if (OnBuildWindowDClick(lineText, m_nextBuildError_lastLine)) {
				match = true;
				break;
			}
		}
	}
}


