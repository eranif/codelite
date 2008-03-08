#include "editor_config.h"
#include "frame.h"
#include "precompiled_header.h"
#include "frame.h"
#include "buildtabsettingsdata.h"
#include "wx/wxFlatNotebook/renderer.h"
#include "regex_processor.h"
#include "macros.h"
#include "wx/xrc/xmlres.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "manager.h"
#include "project.h"
#include "wx/wxscintilla.h" 
#include "buidltab.h"

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

BEGIN_EVENT_TABLE(BuildTab, wxPanel)
	EVT_MENU(XRCID("next_error"), BuildTab::OnNextBuildError)
END_EVENT_TABLE()

//----------------------------------------------------
//decide whether to colour gcc as warning or error
//return:
//0 - dont colour this line
//1 - colour as error
//2 - colour as warning
//----------------------------------------------------
int ColourGccLine(int startLine, const char *line) {
	BuildTab *bt = Frame::Get()->GetOutputPane()->GetBuildTab();
	
	wxString fileName, strLineNumber;
	long idx;

	// search for the compiler definition for the current line
	int lineNumber = bt->LineFromPosition(startLine);
	CompilerPtr cmp = bt->GetCompilerByLine(lineNumber);
	if (!cmp) {
		return 0;
	}

	wxString lineText = _U(line);
	//check if this is a 'Building:' line
	if(lineText.StartsWith(wxT("Building:"))){
		return wxSCI_LEX_GCC_BUILDING;
	}
	
	RegexProcessor re(cmp->GetWarnPattern());
	cmp->GetWarnFileNameIndex().ToLong(&idx);
	if (re.GetGroup(lineText, idx, fileName)) {
		//we found the file name, get the line number
		cmp->GetWarnLineNumberIndex().ToLong( &idx );
		re.GetGroup(lineText, idx, strLineNumber);
		return wxSCI_LEX_GCC_WARNING;
	}

	RegexProcessor ere(cmp->GetErrPattern());
	cmp->GetErrFileNameIndex().ToLong(&idx);
	if (ere.GetGroup(lineText, idx, fileName)) {
		//we found the file name, get the line number
		cmp->GetErrLineNumberIndex().ToLong( &idx );
		ere.GetGroup(lineText, idx, strLineNumber);
		return wxSCI_LEX_GCC_ERROR;
	}
	
	return wxSCI_LEX_GCC_DEFAULT;
}

BuildTab::BuildTab(wxWindow *parent, wxWindowID id, const wxString &name)
		: OutputTabWindow(parent, id, name)
		, m_nextBuildError_lastLine(wxNOT_FOUND)
{
	//set some colours to our styles
	m_sci->MarkerDefine(0x7, wxSCI_MARK_ARROW);
	m_sci->SetMarginWidth(1, 16);
	Initialize();
}

BuildTab::~BuildTab()
{
}

void BuildTab::Initialize()
{
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);
	m_skipWarnings = options.GetSkipWarnings();
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	m_sci->SetLexer(wxSCI_LEX_GCC);
	m_sci->MarkerSetBackground(0x7, wxT("BLUE"));
	m_sci->MarkerSetForeground(0x7, wxT("BLACK"));
	
	m_sci->StyleSetForeground(wxSCI_LEX_GCC_WARNING, options.GetWarnColour());
	m_sci->StyleSetBackground(wxSCI_LEX_GCC_WARNING, options.GetWarnColourBg());
	
	m_sci->StyleSetForeground(wxSCI_LEX_GCC_ERROR, options.GetErrorColour());
	m_sci->StyleSetBackground(wxSCI_LEX_GCC_ERROR, options.GetErrorColourBg());
	
	font.SetWeight(options.GetBoldWarnFont() ? wxBOLD : wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_WARNING, font);
	
	font.SetWeight(wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_DEFAULT, font);
	
	font.SetWeight(options.GetBoldErrFont() ? wxBOLD : wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_ERROR, font);
	
	font.SetWeight(wxBOLD);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_BUILDING, font);
	
	m_sci->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(BuildTab::OnLeftDown), NULL, this);
}

void BuildTab::AppendText(const wxString &text)
{
	// Parse the text and keep information
	//----------------------------------------------
	//Keep information about this line
	if (text.Contains(wxT("----------Building project:"))) {
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
		m_lineInfo[m_sci->GetLineCount()-1] = info;
	} else {
		//set this line with the previous project
		int lineno = m_sci->GetLineCount()-1;
		std::map<int, LineInfo>::iterator iter = m_lineInfo.find(lineno-1);
		if (iter != m_lineInfo.end()) {
			m_lineInfo[lineno] = iter->second;
		}
	}

	OutputTabWindow::AppendText(text);
}

void BuildTab::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);

	lineText.Replace(wxT("\\"), wxT("/"));
	OnBuildWindowDClick(lineText, line);
}

CompilerPtr BuildTab::GetCompilerByLine(int lineClicked)
{
	//lineClicked is zero based.
	//Get the project name that we are currently compiling:
	LineInfo info;
	wxString compilerType(wxEmptyString);

	std::map<int, LineInfo>::iterator iter = m_lineInfo.find(lineClicked);
	if (iter != m_lineInfo.end()) {
		info = iter->second;

		//get the project
		ProjectPtr proj = ManagerST::Get()->GetProject(info.project);
		if (proj) {
			ProjectSettingsPtr settings = proj->GetSettings();
			if (settings) {
				BuildConfigPtr bldConf = settings->GetBuildConfiguration(info.configuration);
				if (bldConf) {
					compilerType = bldConf->GetCompilerType();
				} else {
					//try to get the first build configuration
					ProjectSettingsCookie cookie;
					bldConf = settings->GetFirstBuildConfiguration(cookie);
					if (bldConf) {
						compilerType = bldConf->GetCompilerType();
					}
				}
			}
		}
	}

	//get the selected compiler for the current line that was DClicked
	//find the project selected build configuration for the workspace selected
	//configuration
	return BuildSettingsConfigST::Get()->GetCompiler(compilerType);
}

bool BuildTab::OnBuildWindowDClick(const wxString &line, int lineClicked)
{
	wxString fileName, strLineNumber;
	bool match = false;

	//get the selected compiler for the current line that was DClicked
	//find the project selected build configuration for the workspace selected
	//configuration
	CompilerPtr cmp = GetCompilerByLine(lineClicked);
	if (!cmp) {
		return false;
	}

	long idx;
	RegexProcessor re(cmp->GetWarnPattern());
	cmp->GetWarnFileNameIndex().ToLong(&idx);
	if (re.GetGroup(line, idx, fileName)) {
		//we found the file name, get the line number
		cmp->GetWarnLineNumberIndex().ToLong(&idx);
		re.GetGroup(line, idx, strLineNumber);
		match = true;
	}

	//try to match warning pattern
	if (!match) {
		//try to match an error pattern to the line
		RegexProcessor re(cmp->GetErrPattern());
		cmp->GetErrFileNameIndex().ToLong(&idx);
		if (re.GetGroup(line, idx, fileName)) {
			//we found the file name, get the line number
			cmp->GetErrLineNumberIndex().ToLong(&idx);
			re.GetGroup(line, idx, strLineNumber);
			match = true;
		}
	} else {
		//match was of a warning, 
		//incase of the user is not interesting in warnings, skip it
		if(m_skipWarnings) {
			match = false;
		}
	}

	if (match) {
		long lineNumber = -1;
		strLineNumber.ToLong(&lineNumber);

		//do an educated guess for the reason why this failed
		wxFileName fn(fileName);

		Manager *mgr = ManagerST::Get();
		std::vector<wxFileName> files;
		mgr->GetWorkspaceFiles(files, true);
		
		bool fileOpened(false);
		for (size_t i=0; i<files.size(); i++) {
			if (files.at(i).GetFullName() == fn.GetFullName()) {
				//we have a match
				mgr->OpenFile(files.at(i).GetFullPath(), wxEmptyString, (int)lineNumber-1);
				fileOpened = true;
				if (lineClicked != wxNOT_FOUND || m_nextBuildError_lastLine != wxNOT_FOUND) {
					if (lineClicked != wxNOT_FOUND) {
						//the call came from mouse double click, update
						//the m_nextBuildError_lastLine member
						m_nextBuildError_lastLine = lineClicked;
					}

					m_sci->MarkerDeleteAll(0x7);
					m_sci->MarkerAdd(m_nextBuildError_lastLine, 0x7);
					m_sci->GotoLine(m_nextBuildError_lastLine);
				}
			}
		}
		
		if( !fileOpened ) {
			//try to open the file as is
			//we have a match
			mgr->OpenFile(fn.GetFullPath(), wxEmptyString, (int)lineNumber-1);
			if (lineClicked != wxNOT_FOUND || m_nextBuildError_lastLine != wxNOT_FOUND) {
				if (lineClicked != wxNOT_FOUND) {
					//the call came from mouse double click, update
					//the m_nextBuildError_lastLine member
					m_nextBuildError_lastLine = lineClicked;
				}

				m_sci->MarkerDeleteAll(0x7);
				m_sci->MarkerAdd(m_nextBuildError_lastLine, 0x7);
				m_sci->GotoLine(m_nextBuildError_lastLine);
			}
		}
	}

	return match;
}

void BuildTab::OnNextBuildError(wxCommandEvent &event)
{
	wxUnusedVar(event);
	// get handle to the build window

	if ( m_sci->GetLineCount() <= 0 ) {
		return;
	}

	// iterate through the lines, keeping the last known one
	// and pass the text on to OnBuildWindowDClick
	int startAt;
	if ((m_nextBuildError_lastLine < 0) ||
	        (m_nextBuildError_lastLine >= m_sci->GetLineCount()))
		startAt = 0;
	else startAt = m_nextBuildError_lastLine+1;

	bool match(false);
	for (m_nextBuildError_lastLine = startAt; m_nextBuildError_lastLine < m_sci->GetLineCount(); 	++m_nextBuildError_lastLine) {
		wxString lineText = m_sci->GetLine(m_nextBuildError_lastLine);
		lineText.Replace(wxT("\\"), wxT("/"));
		if (OnBuildWindowDClick(lineText, m_nextBuildError_lastLine)) {
			match = true;
			break;
		}
	}

	//no match? try from top again
	if (!match && startAt > 0) {
		for (m_nextBuildError_lastLine = 0; m_nextBuildError_lastLine < startAt; 	++m_nextBuildError_lastLine) {
			wxString lineText = m_sci->GetLine(m_nextBuildError_lastLine);
			lineText.Replace(wxT("\\"), wxT("/"));
			if (OnBuildWindowDClick(lineText, m_nextBuildError_lastLine)) {
				match = true;
				break;
			}
		}
	}
}

void BuildTab::Clear()
{
	OutputTabWindow::Clear();
	Initialize();
	m_lineInfo.clear();
	m_nextBuildError_lastLine = wxNOT_FOUND;
}

void BuildTab::OnBuildEnded()
{
}

void BuildTab::ReloadSettings()
{
	Initialize();
}

void BuildTab::OnLeftDown(wxMouseEvent &e)
{
	/*
	wxPoint clientPt = e.GetPosition();
	int pos = m_sci->PositionFromPointClose(clientPt.x, clientPt.y);
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);

	lineText.Replace(wxT("\\"), wxT("/"));
	OnBuildWindowDClick(lineText, line);
	*/
	e.Skip();
}

int BuildTab::LineFromPosition(int pos)
{
	return m_sci->LineFromPosition(pos);
}

void BuildTab::OnCompilerColours(wxCommandEvent &e)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("advance_settings"));
	event.SetEventObject(this);
	event.SetInt(1);
	Frame::Get()->ProcessEvent(event);
}
