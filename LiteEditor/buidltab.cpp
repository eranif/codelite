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
#include "precompiled_header.h"
#include <wx/regex.h>
#include <wx/xrc/xmlres.h>
#include <wx/wxscintilla.h>
#include "macros.h"
#include "editor_config.h"
#include "frame.h"
#include "buildtabsettingsdata.h"
#include "regex_processor.h"
#include "build_settings_config.h"
#include "mainbook.h"
#include "cl_editor.h"
#include "manager.h"
#include "project.h"
#include "buidltab.h"
#include "errorstab.h"

// from sdk/wxscintilla/src/scintilla/src/LexGCC.cxx:
extern void SetGccColourFunction(int (*colorfunc)(int, const char*, size_t&, size_t&) );

BuildTab *BuildTab::s_bt;

BuildTab::BuildTab(wxWindow *parent, wxWindowID id, const wxString &name)
    : OutputTabWindow(parent, id, name)
    , m_showMe(BuildTabSettingsData::ShowOnStart)
    , m_autoHide(false)
    , m_skipWarnings(true)
    , m_errorCount(0)
    , m_warnCount(0)
{
    m_tb->AddTool(XRCID("advance_settings"), wxT("Set compiler colours..."),
                  wxXmlResource::Get()->LoadBitmap(wxT("colourise")), wxT("Set compiler colours..."));
    m_tb->Connect(XRCID("advance_settings"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BuildTab::OnCompilerColours), NULL, this);

    wxTheApp->Connect(wxEVT_SHELL_COMMAND_STARTED,         wxCommandEventHandler(BuildTab::OnBuildStarted),    NULL, this);
    wxTheApp->Connect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, wxCommandEventHandler(BuildTab::OnBuildStarted),    NULL, this);
    wxTheApp->Connect(wxEVT_SHELL_COMMAND_ADDLINE,         wxCommandEventHandler(BuildTab::OnBuildAddLine),    NULL, this);
    wxTheApp->Connect(wxEVT_SHELL_COMMAND_PROCESS_ENDED,   wxCommandEventHandler(BuildTab::OnBuildEnded),      NULL, this);
    wxTheApp->Connect(wxEVT_WORKSPACE_LOADED,              wxCommandEventHandler(BuildTab::OnWorkspaceLoaded), NULL, this);
    wxTheApp->Connect(wxEVT_WORKSPACE_CLOSED,              wxCommandEventHandler(BuildTab::OnWorkspaceClosed), NULL, this);
    wxTheApp->Connect(wxEVT_EDITOR_CONFIG_CHANGED,         wxCommandEventHandler(BuildTab::OnConfigChanged),   NULL, this);

    wxTheApp->Connect(XRCID("next_error"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (BuildTab::OnNextBuildError),   NULL, this);
    wxTheApp->Connect(XRCID("next_error"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(BuildTab::OnNextBuildErrorUI), NULL, this);

    wxTheApp->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(BuildTab::OnActiveEditorChanged), NULL, this);

    s_bt = this;
    SetGccColourFunction(ColorLine);
	m_tb->Realize();
    Initialize();
}

BuildTab::~BuildTab()
{
    s_bt = NULL;
    SetGccColourFunction(NULL);
}

int BuildTab::ColorLine(int, const char *text, size_t &start, size_t &len)
{
    std::map<wxString,int>::iterator i = s_bt->m_lineMap.find(_U(text));
    if (i == s_bt->m_lineMap.end())
        return wxSCI_LEX_GCC_DEFAULT;
    std::map<int,LineInfo>::iterator j = s_bt->m_lineInfo.find(i->second);
    if (j == s_bt->m_lineInfo.end())
        return wxSCI_LEX_GCC_DEFAULT;
    start = j->second.filestart;
    len = j->second.filelen;
    return j->second.linecolor;
}

void BuildTab::Initialize()
{
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

    m_showMe       = options.GetShowBuildPane();
    m_autoHide     = options.GetAutoHide();
	m_skipWarnings = options.GetSkipWarnings();

    SetStyles(m_sci, options);
}

void BuildTab::SetStyles(wxScintilla *sci, const BuildTabSettingsData &options)
{
	sci->SetLexer(wxSCI_LEX_GCC);

	sci->SetMarginWidth(1, 0);

	sci->MarkerDefine(0x7, wxSCI_MARK_ARROW);
	sci->MarkerSetBackground(0x7, wxT("PINK"));
	sci->MarkerSetForeground(0x7, wxT("BLACK"));

	sci->StyleSetForeground(wxSCI_LEX_GCC_WARNING, options.GetWarnColour());
	sci->StyleSetBackground(wxSCI_LEX_GCC_WARNING, options.GetWarnColourBg());

	sci->StyleSetForeground(wxSCI_LEX_GCC_ERROR, options.GetErrorColour());
	sci->StyleSetBackground(wxSCI_LEX_GCC_ERROR, options.GetErrorColourBg());

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	font.SetWeight(options.GetBoldWarnFont() ? wxBOLD : wxNORMAL);
	sci->StyleSetFont(wxSCI_LEX_GCC_WARNING, font);

	font.SetWeight(wxNORMAL);
	sci->StyleSetFont(wxSCI_LEX_GCC_DEFAULT, font);

	sci->StyleSetForeground(wxSCI_LEX_GCC_FILE_LINK, wxT("BLUE"));
	sci->StyleSetFont(wxSCI_LEX_GCC_FILE_LINK, font);

	font.SetWeight(options.GetBoldErrFont() ? wxBOLD : wxNORMAL);
	sci->StyleSetFont(wxSCI_LEX_GCC_ERROR, font);

	font.SetWeight(wxBOLD);
	sci->StyleSetFont(wxSCI_LEX_GCC_BUILDING, font);
	sci->StyleSetHotSpot(wxSCI_LEX_GCC_FILE_LINK, true);

    sci->Colourise(0, sci->GetLength());
}

void BuildTab::Clear()
{
	OutputTabWindow::Clear();
    Initialize();
	m_lineInfo.clear();
    m_lineMap.clear();
    m_errorCount = 0;
    m_warnCount = 0;
    m_cmp.Reset(NULL);
    Frame::Get()->GetOutputPane()->GetErrorsTab()->ClearLines();
    LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
    if (editor) {
        editor->DelAllCompilerMarkers();
    }
}

void BuildTab::AppendText(const wxString &text)
{
    LineInfo info;
    int lineno = m_sci->GetLineCount()-1; // get line number before appending new text

	OutputTabWindow::AppendText(text);

	if (text.Contains(BUILD_PROJECT_PREFIX)) {
        // now building the next project
        wxString prj = text.AfterFirst(wxT('[')).BeforeFirst(wxT(']'));
        info.project       = prj.BeforeFirst(wxT('-')).Trim(false).Trim();
        info.configuration = prj.AfterFirst (wxT('-')).Trim(false).Trim();
        info.linecolor     = wxSCI_LEX_GCC_BUILDING;
        m_cmp.Reset(NULL);
        // need to know the compiler in use for this project to extract
        // file/line and error/warning status from the text
        ProjectPtr proj = ManagerST::Get()->GetProject(info.project);
        if (proj) {
            ProjectSettingsPtr settings = proj->GetSettings();
            if (settings) {
                BuildConfigPtr bldConf = settings->GetBuildConfiguration(info.configuration);
                if (!bldConf) {
                    // no buildconf matching the named conf, so use first buildconf instead
                    ProjectSettingsCookie cookie;
                    bldConf = settings->GetFirstBuildConfiguration(cookie);
                }
                if (bldConf) {
                    m_cmp = BuildSettingsConfigST::Get()->GetCompiler(bldConf->GetCompilerType());
                }
            }
        }
	} else if (!m_lineInfo.empty()) {
        // consider this line part of the currently building project
        info.project       = m_lineInfo.rbegin()->second.project;
        info.configuration = m_lineInfo.rbegin()->second.configuration;
    }

    // check for start-of-build or end-of-build messages
    static wxRegEx reSummary(wxT("[0-9]+ errors, [0-9]+ warnings"));
    if (text.StartsWith(BUILD_START_MSG) || reSummary.Matches(text)) {
        info.linecolor = wxSCI_LEX_GCC_BUILDING;
    }

    if (info.linecolor == wxSCI_LEX_GCC_BUILDING || !m_cmp) {
        // no more line info to get
    } else if (ExtractLineInfo(info, text, m_cmp->GetWarnPattern(),
                        m_cmp->GetWarnFileNameIndex(), m_cmp->GetWarnLineNumberIndex())) {
        info.linecolor = wxSCI_LEX_GCC_WARNING;
        m_warnCount++;
    } else if (ExtractLineInfo(info, text, m_cmp->GetErrPattern(),
                               m_cmp->GetErrFileNameIndex(), m_cmp->GetErrLineNumberIndex())) {
        info.linecolor = wxSCI_LEX_GCC_ERROR;
        m_errorCount++;
    }

    if (info.linecolor != wxSCI_LEX_GCC_DEFAULT) {
        info.linetext = text;
        m_lineInfo[lineno] = info;
        m_lineMap[text] = lineno;
        Frame::Get()->GetOutputPane()->GetErrorsTab()->AppendLine(lineno);
    }
}

bool BuildTab::ExtractLineInfo(LineInfo &info, const wxString &text, const wxString &pattern,
                               const wxString &fileidx, const wxString &lineidx)
{
    long fidx, lidx;
    if (!fileidx.ToLong(&fidx) || !lineidx.ToLong(&lidx))
        return false;

    wxRegEx re(pattern);
    if (!re.IsValid() || !re.Matches(text))
        return false;

    size_t start;
    size_t len;
    if (re.GetMatch(&start, &len, fidx)) {
        // sometimes we get leading spaces in the filename match
        while (len > 0 && text[start] == wxT(' ')) {
            start++;
            len--;
        }
        info.filestart = start;
        info.filelen = len;
    }
    if (re.GetMatch(&start, &len, lidx)) {
        text.Mid(start, len).ToLong(&info.linenum);
        info.linenum--; // scintilla starts counting lines from 0
    }
    return true;
}

std::map<int,BuildTab::LineInfo>::iterator BuildTab::GetNextBadLine()
{
    // start scanning from current line
    std::map<int,LineInfo>::iterator i = m_lineInfo.upper_bound(m_sci->GetCurrentLine());
    std::map<int,LineInfo>::iterator e = m_lineInfo.end();
    for (; i != e && i->second.linecolor != wxSCI_LEX_GCC_ERROR &&
            (m_skipWarnings || i->second.linecolor != wxSCI_LEX_GCC_WARNING); i++) { }
    if (i == e) {
        // wrap around to beginning
        i = m_lineInfo.begin();
        e = m_lineInfo.lower_bound(m_sci->GetCurrentLine());
        for (; i != e && i->second.linecolor != wxSCI_LEX_GCC_ERROR &&
                (m_skipWarnings || i->second.linecolor != wxSCI_LEX_GCC_WARNING); i++) { }
    }
    return i != e ? i : m_lineInfo.end();
}

void BuildTab::DoMarkAndOpenFile(std::map<int,LineInfo>::iterator i, bool clearsel)
{
    if (i != m_lineInfo.end() && OpenFile(i->second)) {
        m_sci->MarkerDeleteAll(0x7);
        m_sci->MarkerAdd(i->first, 0x7);
        m_sci->SetCurrentPos(m_sci->PositionFromLine(i->first));
        if (clearsel) {
            m_sci->SetSelection(-1, m_sci->GetCurrentPos());
        }
        Frame::Get()->GetOutputPane()->GetErrorsTab()->MarkLine(i->first);
    }
}

bool BuildTab::OpenFile(const LineInfo &info)
{
    if (info.linecolor != wxSCI_LEX_GCC_ERROR && info.linecolor != wxSCI_LEX_GCC_WARNING)
        return false;
    wxFileName fn = FindFile(info.linetext.Mid(info.filestart, info.filelen), info.project);
    return fn.IsOk() && Frame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, info.linenum) != NULL;
}

wxFileName BuildTab::FindFile(const wxString &filename, const wxString &project)
{
    wxFileName fn(filename);

    if (!fn.FileExists()) {
        // try to open the file as is
        fn.Clear();
    }
    if (!fn.IsOk() && !project.IsEmpty()) {
        // try to open the file in context of its project
        wxArrayString project_files;
        ManagerST::Get()->GetProjectFiles(project, project_files);
        fn = FindFile(project_files, filename);
    }
    if (!fn.IsOk()) {
        // no luck there.  try the whole workspace
        wxArrayString workspace_files;
        ManagerST::Get()->GetWorkspaceFiles(workspace_files);
        fn = FindFile(workspace_files, filename);
    }
    return fn;
}

// Please do not change this code!
wxFileName BuildTab::FindFile(const wxArrayString& files, const wxFileName &fn)
{
	// Iterate over the files twice:
	// first, try to full path
	// if the first iteration failes, iterate the files again
	// and compare full name only
	if (fn.IsAbsolute() && !fn.GetFullPath().Contains(wxT(".."))) {
		return fn;
	}

	// try to convert it to absolute path
	wxFileName f1(fn);
	if (f1.MakeAbsolute() && f1.FileExists() && !f1.GetFullPath().Contains(wxT(".."))) {
		return f1;
	}

	for (size_t i=0; i< files.GetCount(); i++) {
		wxFileName tmpFileName( files.Item(i) );
		if (tmpFileName.GetFullPath() == fn.GetFullPath()) {
			wxFileName tt(tmpFileName);
			if (tt.MakeAbsolute()) {
				return tt;
			} else {
				return tmpFileName;
			}
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
	if (dirs.GetCount() > 0) {
		lastDir = dirs.Last();
	}

	if (matches.size() == 1) {
		wxFileName tt(matches.at(0));
		if (tt.MakeAbsolute()) {
			return tt;
		} else {
			return matches.at(0);
		}

	} else if (matches.size() > 1) {
		// take the best match
		std::vector<wxFileName> betterMatches;
		for (size_t i=0; i<matches.size(); i++) {

			wxFileName filename(matches.at(i));
			wxArrayString tmpdirs = filename.GetDirs();
			if (tmpdirs.GetCount() > 0) {
				if (tmpdirs.Last() == lastDir) {
					betterMatches.push_back(filename);
				}
			}
		}

		if (betterMatches.size() == 1) {
			wxFileName tt(betterMatches.at(0));
			if (tt.MakeAbsolute()) {
				return tt;
			} else {
				return betterMatches.at(0);
			}
		} else {
			// open the first match
			wxFileName tt(matches.at(0));
			if (tt.MakeAbsolute()) {
				return tt;
			} else {
				return matches.at(0);
			}
		}
	}
	return wxFileName();
}

void BuildTab::MarkEditor(LEditor *editor)
{
    if (!editor)
        return;
    editor->DelAllCompilerMarkers();
    for (std::map<int,LineInfo>::iterator i = m_lineInfo.begin(); i != m_lineInfo.end(); i++) {
        if (i->second.filelen == 0 || i->second.project != editor->GetProject())
            continue;
        wxFileName fn = FindFile(i->second.linetext.Mid(i->second.filestart, i->second.filelen), i->second.project);
        if (fn != editor->GetFileName())
            continue;
        if (i->second.linecolor == wxSCI_LEX_GCC_ERROR) {
            editor->SetErrorMarker(i->second.linenum);
        } else if (i->second.linecolor == wxSCI_LEX_GCC_WARNING) {
            editor->SetWarningMarker(i->second.linenum);
        }
    }
}

void BuildTab::OnClearAll(wxCommandEvent &e)
{
    Clear();
}

void BuildTab::OnBuildStarted(wxCommandEvent &e)
{
    e.Skip();
    m_tb->EnableTool(XRCID("clear_all_output"), false);
    if (e.GetEventType() != wxEVT_SHELL_COMMAND_STARTED_NOCLEAN) {
        Clear();
    }
    AppendText(BUILD_START_MSG);
    Frame::Get()->SetStatusMessage(e.GetString(), 4, XRCID("build"));
    if (m_showMe == BuildTabSettingsData::ShowOnStart) {
        ManagerST::Get()->ShowOutputPane(m_name);
    } else if (m_showMe == BuildTabSettingsData::ShowOnEnd) {
        bool viewing = ManagerST::Get()->IsPaneVisible(Frame::Get()->GetOutputPane()->GetCaption()) &&
                        (Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() == this ||
                         Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() ==
                            Frame::Get()->GetOutputPane()->GetErrorsTab());
        if (m_autoHide && viewing) {
            ManagerST::Get()->HidePane(Frame::Get()->GetOutputPane()->GetName());
        }
    }
    m_sw.Start();
}

void BuildTab::OnBuildAddLine(wxCommandEvent &e)
{
    e.Skip();
    AppendText(e.GetString());
}

void BuildTab::OnBuildEnded(wxCommandEvent &e)
{
    e.Skip();

    wxString term = wxString::Format(wxT("%d errors, %d warnings"), m_errorCount, m_warnCount);
    long elapsed = m_sw.Time() / 1000;
    if (elapsed > 10) {
        long sec = elapsed % 60;
        long hours = elapsed / 3600;
        long minutes = (elapsed % 3600) / 60;
        term << wxString::Format(wxT(", total time: %02d:%02d:%02d seconds"), hours, minutes, sec);

    }
    term << wxT('\n');
    AppendText(term);
    AppendText(BUILD_END_MSG);

    Frame::Get()->SetStatusMessage(wxEmptyString, 4, XRCID("build"));

    bool success = m_errorCount == 0 && (m_skipWarnings || m_warnCount == 0);
    bool viewing = ManagerST::Get()->IsPaneVisible(Frame::Get()->GetOutputPane()->GetCaption()) &&
                    (Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() == this ||
                     Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() ==
                        Frame::Get()->GetOutputPane()->GetErrorsTab());

    if (!success) {
        if (viewing) {
            std::map<int,LineInfo>::iterator i = GetNextBadLine();
            m_sci->GotoLine(i->first-1); // minus one line so user can type F4 to open the first error
        } else {
            ManagerST::Get()->ShowOutputPane(Frame::Get()->GetOutputPane()->GetErrorsTab()->GetCaption());
        }
    } else if (m_autoHide && viewing) {
        ManagerST::Get()->HidePane(Frame::Get()->GetOutputPane()->GetCaption());
    } else if (m_showMe == BuildTabSettingsData::ShowOnEnd && !m_autoHide) {
        ManagerST::Get()->ShowOutputPane(m_name);
    }

    m_tb->EnableTool(XRCID("clear_all_output"), true);
    MarkEditor(Frame::Get()->GetMainBook()->GetActiveEditor());
}

void BuildTab::OnWorkspaceLoaded(wxCommandEvent &e)
{
    e.Skip();
    Clear();
}

void BuildTab::OnWorkspaceClosed(wxCommandEvent &e)
{
    e.Skip();
    Clear();
}

void BuildTab::OnConfigChanged(wxCommandEvent &e)
{
    e.Skip();
    const wxString *config = (const wxString *) e.GetClientData();
    if (config && *config == wxT("build_tab_settings")) {
        Initialize();
        Frame::Get()->GetOutputPane()->GetErrorsTab()->OnRedisplayLines(e);
    }
}

void BuildTab::OnCompilerColours(wxCommandEvent &e)
{
    // tell Frame to open Advance Settings dlg on compiler colors page
    e.SetInt(1);
    e.Skip();
}

void BuildTab::OnNextBuildError(wxCommandEvent &e)
{
    wxUnusedVar(e);
    if (m_errorCount > 0 || !m_skipWarnings && m_warnCount > 0) {
        std::map<int,LineInfo>::iterator i = GetNextBadLine();
        if (i != m_lineInfo.end()) {
            wxString showpane = m_name;
            if (Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() ==
                    Frame::Get()->GetOutputPane()->GetErrorsTab()) {
                showpane = Frame::Get()->GetOutputPane()->GetErrorsTab()->GetCaption();
            }
            ManagerST::Get()->ShowOutputPane(showpane);
            DoMarkAndOpenFile(i, true);
        }
    }
}

void BuildTab::OnNextBuildErrorUI(wxUpdateUIEvent &e)
{
	e.Enable(m_errorCount > 0 || !m_skipWarnings && m_warnCount > 0);
}

void BuildTab::OnActiveEditorChanged(wxCommandEvent &e)
{
    e.Skip();
    MarkEditor(Frame::Get()->GetMainBook()->GetActiveEditor());
}

void BuildTab::OnHotspotClicked(wxScintillaEvent &e)
{
	OnMouseDClick(e);
}

void BuildTab::OnMouseDClick(wxScintillaEvent &e)
{
    wxUnusedVar(e);
    DoMarkAndOpenFile(m_lineInfo.find(m_sci->LineFromPosition(e.GetPosition())), true);
}
