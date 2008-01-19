#include "editor_config.h"
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

#define ERROR_MARKER 	0x5
#define WARN_MARKER 	0x6


#define SCE_STYLE_WARNING 	1
#define SCE_STYLE_ERROR  	2

extern char *arrow_right_green_xpm[];


#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

BEGIN_EVENT_TABLE(BuildTab, wxPanel)
	EVT_MENU(XRCID("next_error"), BuildTab::OnNextBuildError)
END_EVENT_TABLE()

BuildTab::BuildTab(wxWindow *parent, wxWindowID id, const wxString &name)
		: OutputTabWindow(parent, id, name)
		, m_nextBuildError_lastLine(wxNOT_FOUND)
{
	//set some colours to our styles
//	wxImage img(arrow_right_green_xpm);
//	wxBitmap bmp(img);
//	m_sci->MarkerDefineBitmap(0x7, bmp);
	m_sci->MarkerDefine(0x7, wxSCI_MARK_ARROW);
	m_sci->MarkerSetForeground(0x7, wxT("BLACK"));
	m_sci->MarkerSetBackground(0x7, wxT("RED"));
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
	
	m_sci->MarkerDefine(ERROR_MARKER, wxSCI_MARK_BACKGROUND);
	m_sci->MarkerDefine(WARN_MARKER, wxSCI_MARK_BACKGROUND);
	m_sci->MarkerSetBackground(ERROR_MARKER, options.GetErrorColourBg());
	m_sci->MarkerSetBackground(WARN_MARKER, options.GetWarnColourBg());

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
//	m_sci->StyleSetForeground(SCE_STYLE_WARNING, options.GetWarnColour());
//	m_sci->StyleSetForeground(SCE_STYLE_ERROR, options.GetErrorColour());
	m_sci->StyleSetBackground(SCE_STYLE_WARNING, options.GetWarnColourBg());
	m_sci->StyleSetBackground(SCE_STYLE_ERROR, options.GetErrorColourBg());
	
	m_sci->StyleSetFont(SCE_STYLE_WARNING, font);
	m_sci->StyleSetFont(SCE_STYLE_ERROR, font);
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
	ColourLine(m_sci->GetLineCount()-2);
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
		for (size_t i=0; i<files.size(); i++) {
			if (files.at(i).GetFullName() == fn.GetFullName()) {
				//we have a match
				mgr->OpenFile(files.at(i).GetFullPath(), wxEmptyString, (int)lineNumber-1);

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
	m_lineInfo.clear();
	m_nextBuildError_lastLine = wxNOT_FOUND;
}

void BuildTab::OnStyleNeeded(wxScintillaEvent &event)
{
	wxString fileName, strLineNumber;
	bool warn_match = false;
	bool eror_match = false;
	long idx;

	long start_pos = m_sci->GetEndStyled();
	long lineNumber = m_sci->LineFromPosition(start_pos);
	start_pos = m_sci->PositionFromLine(lineNumber);
	int line_length = m_sci->LineLength(lineNumber);

	// search for the compiler definition for the current line
	CompilerPtr cmp = GetCompilerByLine(lineNumber);
	if (!cmp) {
		return;
	}

	wxString lineText = m_sci->GetLine(lineNumber);
	RegexProcessor re(cmp->GetWarnPattern());
	cmp->GetWarnFileNameIndex().ToLong(&idx);
	if (re.GetGroup(lineText, idx, fileName)) {
		//we found the file name, get the line number
		cmp->GetWarnLineNumberIndex().ToLong( &idx );
		re.GetGroup(lineText, idx, strLineNumber);
		warn_match = true;
	}

	if ( !warn_match ) {
		RegexProcessor ere(cmp->GetErrPattern());
		cmp->GetErrFileNameIndex().ToLong(&idx);
		if (ere.GetGroup(lineText, idx, fileName)) {
			//we found the file name, get the line number
			cmp->GetErrLineNumberIndex().ToLong( &idx );
			ere.GetGroup(lineText, idx, strLineNumber);
			eror_match = true;
		}
	}


	// colour the data between end and start position
	m_sci->StartStyling(start_pos, 0x1f);

	if ( warn_match ) {
		//colour this line in orange
		m_sci->SetStyling(line_length, SCE_STYLE_WARNING);
		m_sci->MarkerAdd(lineNumber, WARN_MARKER);
	} else if ( eror_match ) {
		//error line
		m_sci->SetStyling(line_length, SCE_STYLE_ERROR);
		m_sci->MarkerAdd(lineNumber, ERROR_MARKER);
	} else {
		//default
		m_sci->SetStyling(line_length, wxSCI_STYLE_DEFAULT);
	}
	
	//apply changes
	event.Skip();
}

void BuildTab::ColourLine(int lineNumber)
{
	wxString fileName, strLineNumber;
	bool warn_match = false;
	bool eror_match = false;
	long idx;

	// search for the compiler definition for the current line
	CompilerPtr cmp = GetCompilerByLine(lineNumber);
	if (!cmp) {
		return;
	}

	wxString lineText = m_sci->GetLine(lineNumber);
	RegexProcessor re(cmp->GetWarnPattern());
	cmp->GetWarnFileNameIndex().ToLong(&idx);
	if (re.GetGroup(lineText, idx, fileName)) {
		//we found the file name, get the line number
		cmp->GetWarnLineNumberIndex().ToLong( &idx );
		re.GetGroup(lineText, idx, strLineNumber);
		warn_match = true;
	}

	if ( !warn_match ) {
		RegexProcessor ere(cmp->GetErrPattern());
		cmp->GetErrFileNameIndex().ToLong(&idx);
		if (ere.GetGroup(lineText, idx, fileName)) {
			//we found the file name, get the line number
			cmp->GetErrLineNumberIndex().ToLong( &idx );
			ere.GetGroup(lineText, idx, strLineNumber);
			eror_match = true;
		}
	}
		
	if ( warn_match ) {
		//colour this line in orange
		m_sci->MarkerAdd(lineNumber, WARN_MARKER);
	} else if ( eror_match ) {
		//error line
		m_sci->MarkerAdd(lineNumber, ERROR_MARKER);
	}
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
	wxPoint clientPt = e.GetPosition();
	int pos = m_sci->PositionFromPointClose(clientPt.x, clientPt.y);
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);

	lineText.Replace(wxT("\\"), wxT("/"));
	OnBuildWindowDClick(lineText, line);
}
