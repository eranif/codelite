//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buidltab.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "editor_config.h"
#include "wx/regex.h"
#include "buildtabsettingsdata.h"
#include "regex_processor.h"
#include "macros.h"
#include "wx/xrc/xmlres.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "manager.h"
#include "project.h"
#include "wx/wxscintilla.h"
#include "errorstab.h"

#include <wx/log.h>

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

BEGIN_EVENT_TABLE(ErrorsTab, wxPanel)
	EVT_MENU(XRCID("next_error"), ErrorsTab::OnNextBuildError)
END_EVENT_TABLE()

// NOTE: copied much from buidltab.cpp (BuildTab class). It is actually better to refactor the redundant code
// into a common class (either base or utility):

// 	CompilerPtr GetCompilerByLine(int lineClicked);
//	bool DoTryOpenFile(const wxArrayString& files, const wxFileName &fn, int lineNumber, int lineClicked);
//	bool DoOpenFile(const wxFileName &fn, int lineNumber, int lineClicked);
//  void Initialize();

ErrorsTab::ErrorsTab(wxWindow *parent, wxWindowID id, const wxString &name)
		: OutputTabWindow(parent, id, name)
		, m_nextBuildError_lastLine(wxNOT_FOUND)		
		, m_errorCount(0)
		, m_warningCount(0)
		, m_IncludeResults(TYPE_ERROR | TYPE_WARNING)
{
	//set some colours to our styles
	m_sci->MarkerDefine(0x7, wxSCI_MARK_ARROW);
	m_sci->SetMarginWidth(1, 0);
	Initialize();
}

ErrorsTab::~ErrorsTab()
{
}

void ErrorsTab::Initialize()
{
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	m_sci->SetLexer(wxSCI_LEX_GCC);
	m_sci->MarkerSetBackground(0x7, wxT("PINK"));
	m_sci->MarkerSetForeground(0x7, wxT("BLACK"));

	m_sci->StyleSetForeground(wxSCI_LEX_GCC_WARNING, options.GetWarnColour());
	m_sci->StyleSetBackground(wxSCI_LEX_GCC_WARNING, options.GetWarnColourBg());

	m_sci->StyleSetForeground(wxSCI_LEX_GCC_ERROR, options.GetErrorColour());
	m_sci->StyleSetBackground(wxSCI_LEX_GCC_ERROR, options.GetErrorColourBg());

	font.SetWeight(options.GetBoldWarnFont() ? wxBOLD : wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_WARNING, font);

	font.SetWeight(wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_DEFAULT, font);

	m_sci->StyleSetForeground(wxSCI_LEX_GCC_FILE_LINK, wxT("BLUE"));
	m_sci->StyleSetFont(wxSCI_LEX_GCC_FILE_LINK, font);

	font.SetWeight(options.GetBoldErrFont() ? wxBOLD : wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_ERROR, font);

	font.SetWeight(wxBOLD);
	m_sci->StyleSetFont(wxSCI_LEX_GCC_BUILDING, font);
	m_sci->StyleSetHotSpot(wxSCI_LEX_GCC_FILE_LINK, true);
}

void ErrorsTab::OnNextBuildError(wxCommandEvent &event)
{
	wxUnusedVar(event);
	
	if ( m_sci->GetLineCount() <= 0 ) {
		return;
	}
	
	// iterate through the lines, keeping the last known one
	// and pass the text on to OnBuildWindowDClick
	int startAt;
	if ((m_nextBuildError_lastLine < 0) || (m_nextBuildError_lastLine >= m_sci->GetLineCount()))
	{
		startAt = 0;
	}
	else 
	{
		startAt = m_nextBuildError_lastLine+1;
	}
	
	bool match(false);
	for (m_nextBuildError_lastLine = startAt; m_nextBuildError_lastLine < m_sci->GetLineCount(); ++m_nextBuildError_lastLine)
	{
		if (m_lineInfo[m_nextBuildError_lastLine].type & m_IncludeResults)
		{
			wxString lineText = m_sci->GetLine(m_nextBuildError_lastLine);
			lineText.Replace(wxT("\\"), wxT("/"));
			if (OnBuildWindowDClick(m_nextBuildError_lastLine)) {
				match = true;
				break;
			}
		}
	}

	//no match? try from top again
	if (!match && startAt > 0) {
		wxLogDebug(wxT("no match"));
		for (m_nextBuildError_lastLine = 0; m_nextBuildError_lastLine < startAt; ++m_nextBuildError_lastLine)
		{
			if (m_lineInfo[m_nextBuildError_lastLine].type & m_IncludeResults)
			{
				wxString lineText = m_sci->GetLine(m_nextBuildError_lastLine);
				lineText.Replace(wxT("\\"), wxT("/"));
				if (OnBuildWindowDClick(m_nextBuildError_lastLine)) {
					match = true;
					break;
				}
			}
		}
	}
}

void ErrorsTab::Clear()
{
	OutputTabWindow::Clear();
	m_lineInfo.clear();
	m_errorCount   = 0;
	m_warningCount = 0;
}

CompilerPtr ErrorsTab::GetCompilerByLine(int lineClicked)
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

void ErrorsTab::AppendText(const wxString &text)
{
	wxString fileName, strLineNumber;
	CompilerPtr cmp;

	//we use here GetLineCount()-1 since an empty docuement contains
	//1 line, but scintilla refers to lines from 0
	int lineno = m_sci->GetLineCount() - 1;

	// Parse the text and find project name and config
	if (text.Contains(wxT("----------Building project:"))) {
		LineInfo info;
		
		wxString projectName = text.AfterFirst(wxT('['));
		projectName = projectName.Trim(false).Trim();
		int where   = projectName.Find(wxT(" - "));

		if (where != wxNOT_FOUND) {
			info.project = projectName.Mid(0, where);
			TrimString(info.project);

			info.configuration = text.AfterFirst(wxT('['));
			info.configuration = info.configuration.AfterFirst(wxT('-'));
			info.configuration = info.configuration.BeforeFirst(wxT(']'));
			TrimString(info.configuration);
			
			//keep the information about this line
			m_lineInfo[lineno] = info;			
		} else {
			// do the fallback
			//set this line with the previous project
			std::map<int, LineInfo>::iterator iter = m_lineInfo.find(lineno);
			if (iter != m_lineInfo.end()) {
				m_lineInfo[lineno] = iter->second;
			}
		}
	} else {
		//set this line with the previous project
		std::map<int, LineInfo>::iterator iter = m_lineInfo.find(lineno);
		if (iter != m_lineInfo.end()) {
			m_lineInfo[lineno] = iter->second;
		}
	}

	cmp = GetCompilerByLine(lineno);
	if (!cmp) {
		return;
	}	
	
	long idx;
	
	bool match = false;
	
	// Find warnings
	RegexProcessor reWarning(cmp->GetWarnPattern());
	cmp->GetWarnFileNameIndex().ToLong(&idx);
	if (reWarning.GetGroup(text, idx, fileName)) {
		cmp->GetWarnLineNumberIndex().ToLong(&idx);
		if (reWarning.GetGroup(text, idx, strLineNumber)) {
			m_lineInfo[lineno].compilerOutput = text;
			m_lineInfo[lineno].fileName       = fileName;
			m_lineInfo[lineno].lineNo         = strLineNumber;
			m_lineInfo[lineno].type           = TYPE_WARNING;
			m_warningCount++;
			match = true;
		}
	}
	
	if (!match)
	{
		// Find errors
		RegexProcessor reError(cmp->GetErrPattern());
		cmp->GetErrFileNameIndex().ToLong(&idx);
		if (reError.GetGroup(text, idx, fileName)) {
			cmp->GetErrLineNumberIndex().ToLong(&idx);
			if (reError.GetGroup(text, idx, strLineNumber)) {
				m_lineInfo[lineno].compilerOutput = text;
				m_lineInfo[lineno].fileName       = fileName;
				m_lineInfo[lineno].lineNo         = strLineNumber;
				m_lineInfo[lineno].type           = TYPE_ERROR;
				m_errorCount++;
				match = true;
			}
		}
	}
	
	// Only include the text if we found a match and we want to include the results.
	// User may be able to override by setting the ViewType.
	if (match && (m_lineInfo[lineno].type & m_IncludeResults)) OutputTabWindow::AppendText(text);
}

int ErrorsTab::GetViewType()
{
	return m_IncludeResults;
}

void ErrorsTab::SetViewType(int type)
{
	if (m_IncludeResults != type)
	{
		m_IncludeResults = type;
		
		UpdateView();
	}
}

void ErrorsTab::UpdateView()
{
	OutputTabWindow::Clear();
	
	std::map<int, LineInfo>::iterator iter;
	for (iter = m_lineInfo.begin(); iter != m_lineInfo.end(); ++iter)
	{
		if ((iter->second).type & m_IncludeResults)
		{
			OutputTabWindow::AppendText((iter->second).compilerOutput);
		}
	}
}

void ErrorsTab::OnHotspotClicked(wxScintillaEvent &event)
{
	OnMouseDClick(event);
}

void ErrorsTab::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);

	lineText.Replace(wxT("\\"), wxT("/"));
	OnBuildWindowDClick(line);
}

bool ErrorsTab::OnBuildWindowDClick(int lineClicked)
{
	wxArrayString files;
	wxFileName fn(m_lineInfo[lineClicked].fileName);
	
	Manager *mgr = ManagerST::Get();
	mgr->GetWorkspaceFiles(files);
	
	long lineNo;
	m_lineInfo[lineClicked].lineNo.ToLong(&lineNo);
	
	bool fileOpened = DoTryOpenFile(files, fn, lineNo, lineClicked);
	if ( !fileOpened ) {
		// try to open the file as is
		// we have a match
		mgr->OpenFile(fn.GetFullPath(), wxEmptyString, (int)lineNo - 1);
		if (lineClicked != wxNOT_FOUND || m_nextBuildError_lastLine != wxNOT_FOUND) {
			if (lineClicked != wxNOT_FOUND) {
				
				// the call came from mouse double click, update
				// the m_nextBuildError_lastLine member
				m_nextBuildError_lastLine = lineClicked;
			}

			m_sci->MarkerDeleteAll(0x7);
			m_sci->MarkerAdd(m_nextBuildError_lastLine, 0x7);
			m_sci->GotoLine(m_nextBuildError_lastLine);
			m_sci->EnsureVisible(m_nextBuildError_lastLine);
			
			fileOpened = true;
		}
	}
	
	return fileOpened;
}

bool ErrorsTab::DoTryOpenFile(const wxArrayString& files, const wxFileName &fn, int lineNumber, int lineClicked)
{
	// Iterate over the files twice:
	// first, try to full path
	// if the first iteration failes, iterate the files again
	// and compare full name only
	if(fn.IsAbsolute()){
		return DoOpenFile(fn, lineNumber, lineClicked);
	}
	
	for (size_t i=0; i< files.GetCount(); i++) {
		wxFileName tmpFileName( files.Item(i) );
		if (tmpFileName.GetFullPath() == fn.GetFullPath()) {
			return DoOpenFile(tmpFileName, lineNumber, lineClicked);
		}
	}
	
	std::vector<wxFileName> matches;
	
	for (size_t i=0; i< files.GetCount(); i++) {
		wxFileName tmpFileName( files.Item(i) );
		if (tmpFileName.GetFullName() == fn.GetFullName()) {
			matches.push_back(tmpFileName);
		}
	}
	
	wxString lastDir;
	wxArrayString dirs = fn.GetDirs();
	if(dirs.GetCount() > 0){
		lastDir = dirs.Last();
	}
	
	if(matches.size() == 1){
		
		return DoOpenFile(matches.at(0), lineNumber, lineClicked);
		
	} else if(matches.size() > 1){
		// take the best match
		std::vector<wxFileName> betterMatches;
		for(size_t i=0; i<matches.size(); i++){
			
			wxFileName filename(matches.at(i));
			wxArrayString tmpdirs = filename.GetDirs();
			if(tmpdirs.GetCount() > 0){
				if(tmpdirs.Last() == lastDir){
					betterMatches.push_back(filename);
				}
			}		
		}
		
		if(betterMatches.size() == 1){
			return DoOpenFile(betterMatches.at(0), lineNumber, lineClicked);
		} else {
			// open the first match
			return DoOpenFile(matches.at(0), lineNumber, lineClicked);
		}
	}
	return false;
}

bool ErrorsTab::DoOpenFile(const wxFileName& fn, int lineNumber, int lineClicked)
{
	//we got a match
	ManagerST::Get()->OpenFile(fn.GetFullPath(), wxEmptyString, (int)lineNumber - 1);
	if (lineClicked != wxNOT_FOUND || m_nextBuildError_lastLine != wxNOT_FOUND) {
		if (lineClicked != wxNOT_FOUND) {
			//the call came from mouse double click, update
			//the m_nextBuildError_lastLine member
			m_nextBuildError_lastLine = lineClicked;
		}

		m_sci->MarkerDeleteAll(0x7);
		m_sci->MarkerAdd(m_nextBuildError_lastLine, 0x7);
		m_sci->GotoLine(m_nextBuildError_lastLine);
		m_sci->EnsureVisible(m_nextBuildError_lastLine);
	}
	return true;
}
