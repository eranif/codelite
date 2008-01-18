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

BuildTab::BuildTab(wxWindow *parent, wxWindowID id, const wxString &name)
		: OutputTabWindow(parent, id, name)
		, m_nextBuildError_lastLine(wxNOT_FOUND)
{
	Initialize();
}

BuildTab::~BuildTab()
{
}

void BuildTab::Initialize()
{
	
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


bool BuildTab::OnBuildWindowDClick(const wxString &line, int lineClicked)
{
	wxString fileName, strLineNumber;
	wxString compilerType(wxEmptyString);
	LineInfo info;
	bool match = false;

	//lineClicked is zero based.
	//Get the project name that we are currently compiling:
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
	CompilerPtr cmp;
	cmp = BuildSettingsConfigST::Get()->GetCompiler(compilerType);
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
