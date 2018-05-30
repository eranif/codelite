//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : new_build_tab.cpp
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

#include "BuildTabTopPanel.h"
#include "ColoursAndFontsManager.h"
#include "Notebook.h"
#include "attribute_style.h"
#include "bitmap_loader.h"
#include "build_settings_config.h"
#include "buildtabsettingsdata.h"
#include "clSingleChoiceDialog.h"
#include "clStrings.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "manager.h"
#include "new_build_tab.h"
#include "optionsconfig.h"
#include "output_pane.h"
#include "pluginmanager.h"
#include "shell_command.h"
#include "workspace.h"
#include <wx/choicdlg.h>
#include <wx/dataview.h>
#include <wx/dcmemory.h>
#include <wx/fdrepdlg.h>
#include <wx/regex.h>
#include <wx/settings.h>

#define IS_VALID_LINE(lineNumber) ((lineNumber >= 0 && lineNumber < m_view->GetLineCount()))
#ifdef __WXMSW__
#define IS_WINDOWS true
#else
#define IS_WINDOWS false
#endif

// Helper function to post an event to the frame
void SetActive(LEditor* editor)
{
    wxCHECK_RET(editor, wxT("Null editor parameter"));

    wxCommandEvent event(wxEVT_ACTIVATE_EDITOR);
    event.SetEventObject(editor);
    wxPostEvent(clMainFrame::Get(), event);
}

//////////////////////////////////////////////////////////////

struct AnnotationInfo {
    int line;
    LINE_SEVERITY severity;
    std::set<wxString> text;
};
typedef std::map<int, AnnotationInfo> AnnotationInfoByLineMap_t;

#define LEX_GCC_DEFAULT 0
#define LEX_GCC_ERROR 1
#define LEX_GCC_WARNING 2
#define LEX_GCC_INFO 3

#define LEX_GCC_MARKER 1

NewBuildTab::NewBuildTab(wxWindow* parent)
    : wxPanel(parent)
    , m_warnCount(0)
    , m_errorCount(0)
    , m_buildInterrupted(false)
    , m_autoHide(false)
    , m_showMe(BuildTabSettingsData::ShowOnStart)
    , m_skipWarnings(false)
    , m_buildpaneScrollTo(ScrollToFirstError)
    , m_buildInProgress(false)
    , m_maxlineWidth(wxNOT_FOUND)
    , m_lastLineColoured(wxNOT_FOUND)
{
    m_curError = m_errorsAndWarningsList.end();
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    SetSizer(bs);

    m_view = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    // We dont really want to collect undo in the output tabs...
    InitView();
    Bind(wxEVT_IDLE, &NewBuildTab::OnIdle, this);

    m_view->Bind(wxEVT_STC_HOTSPOT_CLICK, &NewBuildTab::OnHotspotClicked, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &NewBuildTab::OnThemeChanged, this);

    bs->Add(m_view, 1, wxEXPAND | wxALL);

    BuildTabTopPanel* toolbox = new BuildTabTopPanel(this);
    bs->Insert(0, toolbox, 0, wxEXPAND);

    EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_STARTED, clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN,
                                  clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_ADDLINE, clCommandEventHandler(NewBuildTab::OnBuildAddLine), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_PROCESS_ENDED, clCommandEventHandler(NewBuildTab::OnBuildEnded),
                                  NULL, this);

    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(NewBuildTab::OnWorkspaceLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(NewBuildTab::OnWorkspaceClosed), NULL,
                                  this);

    wxTheApp->Connect(XRCID("next_build_error"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(NewBuildTab::OnNextBuildError), NULL, this);
    wxTheApp->Connect(XRCID("next_build_error"), wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(NewBuildTab::OnNextBuildErrorUI), NULL, this);
}

NewBuildTab::~NewBuildTab()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &NewBuildTab::OnThemeChanged, this);
    EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_STARTED, clCommandEventHandler(NewBuildTab::OnBuildStarted),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN,
                                     clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_ADDLINE, clCommandEventHandler(NewBuildTab::OnBuildAddLine),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_PROCESS_ENDED,
                                     clCommandEventHandler(NewBuildTab::OnBuildEnded), NULL, this);
    wxTheApp->Disconnect(XRCID("next_build_error"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(NewBuildTab::OnNextBuildError), NULL, this);
    wxTheApp->Disconnect(XRCID("next_build_error"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(NewBuildTab::OnNextBuildErrorUI), NULL, this);
}

void NewBuildTab::OnBuildEnded(clCommandEvent& e)
{
    e.Skip();
    CL_DEBUG("Build Ended!");
    m_buildInProgress = false;

    DoProcessOutput(true, false);

    std::vector<LEditor*> editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_Default);
    for(size_t i = 0; i < editors.size(); i++) {
        MarkEditor(editors.at(i));
    }

    // Add a summary line
    wxString problemcount =
        wxString::Format(wxT("%d %s, %d %s"), m_errorCount, _("errors"), m_warnCount, _("warnings"));
    wxString term = problemcount;
    long elapsed = m_sw.Time() / 1000;
    if(elapsed > 10) {
        long sec = elapsed % 60;
        long hours = elapsed / 3600;
        long minutes = (elapsed % 3600) / 60;
        term << wxString::Format(wxT(", %s: %02ld:%02ld:%02ld %s"), _("total time"), hours, minutes, sec, _("seconds"));
    }

    m_output = term;
    DoProcessOutput(true, true);

    if(m_buildInterrupted) {
        wxString InterruptedMsg;
        InterruptedMsg << _("(Build Cancelled)") << wxT("\n\n");
        m_output = InterruptedMsg;
        DoProcessOutput(true, false);
    }

    // Hide / Show the build tab according to the settings
    DoToggleWindow();

    // make it invalid
    m_curError = m_errorsAndWarningsList.begin();
    CL_DEBUG("Posting wxEVT_BUILD_ENDED event");

    // 0 = first error
    // 1 = first error or warning
    // 2 = to the end
    if(m_buildTabSettings.GetBuildPaneScrollDestination() == ScrollToFirstError && !m_errorsList.empty()) {
        BuildLineInfo* bli = m_errorsList.front();
        DoSelectAndOpen(bli->GetLineInBuildTab(), true);
    }

    if(m_buildTabSettings.GetBuildPaneScrollDestination() == ScrollToFirstItem && !m_errorsAndWarningsList.empty()) {
        BuildLineInfo* bli = m_errorsAndWarningsList.front();
        DoSelectAndOpen(bli->GetLineInBuildTab(), true);
    }

    if(m_buildTabSettings.GetBuildPaneScrollDestination() == ScrollToEnd) { m_view->ScrollToEnd(); }

    // notify the plugins that the build has ended
    clBuildEvent buildEvent(wxEVT_BUILD_ENDED);
    buildEvent.SetErrorCount(m_errorCount);
    buildEvent.SetWarningCount(m_warnCount);
    EventNotifier::Get()->AddPendingEvent(buildEvent);
}

void NewBuildTab::OnBuildStarted(clCommandEvent& e)
{
    e.Skip();

    if(IS_WINDOWS) {
        m_cygwinRoot.Clear();
        EnvSetter es;
        wxString cmd;
        cmd << "cygpath -w /";
        wxArrayString arrOut;
        ProcUtils::SafeExecuteCommand(cmd, arrOut);

        if(arrOut.IsEmpty() == false) { m_cygwinRoot = arrOut.Item(0); }
    }

    m_buildInProgress = true;

    // Reload the build settings data
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &m_buildTabSettings);
    InitView();

    m_autoHide = m_buildTabSettings.GetAutoHide();
    m_showMe = (BuildTabSettingsData::ShowBuildPane)m_buildTabSettings.GetShowBuildPane();
    m_skipWarnings = m_buildTabSettings.GetSkipWarnings();

    if(e.GetEventType() != wxEVT_SHELL_COMMAND_STARTED_NOCLEAN) {
        DoClear();
        DoCacheRegexes();
    }

    // Show the tab if needed
    OutputPane* opane = clMainFrame::Get()->GetOutputPane();

    wxWindow* win(NULL);
    int sel = opane->GetNotebook()->GetSelection();
    if(sel != wxNOT_FOUND) { win = opane->GetNotebook()->GetPage(sel); }

    if(m_showMe == BuildTabSettingsData::ShowOnStart) {
        ManagerST::Get()->ShowOutputPane(BUILD_WIN, true);

    } else if(m_showMe == BuildTabSettingsData::ShowOnEnd && m_autoHide &&
              ManagerST::Get()->IsPaneVisible(opane->GetCaption()) && win == this) {
        // user prefers to see build/errors tabs only at end of unsuccessful build
        ManagerST::Get()->HidePane(opane->GetName());
    }
    m_sw.Start();

    m_cmp.Reset(NULL);
    BuildEventDetails* bed = dynamic_cast<BuildEventDetails*>(e.GetClientObject());
    if(bed) {
        BuildConfigPtr buildConfig =
            clCxxWorkspaceST::Get()->GetProjBuildConf(bed->GetProjectName(), bed->GetConfiguration());
        if(buildConfig) { m_cmp = buildConfig->GetCompiler(); }

        // notify the plugins that the build had started
        clBuildEvent buildEvent(wxEVT_BUILD_STARTED);
        buildEvent.SetProjectName(bed->GetProjectName());
        buildEvent.SetConfigurationName(bed->GetConfiguration());
        EventNotifier::Get()->AddPendingEvent(buildEvent);
    }
}

void NewBuildTab::OnBuildAddLine(clCommandEvent& e)
{
    e.Skip(); // Allways call skip..
    m_output << e.GetString();
    DoProcessOutput(false, false);
}

BuildLineInfo* NewBuildTab::DoProcessLine(const wxString& line)
{
    BuildLineInfo* buildLineInfo = new BuildLineInfo();
    LINE_SEVERITY severity;
    // Get the matching regex for this line
    CmpPatternPtr cmpPatterPtr = GetMatchingRegex(line, severity);
    buildLineInfo->SetSeverity(severity);
    BuildLineInfo bli;
    if(cmpPatterPtr && cmpPatterPtr->Matches(line, bli)) {
        buildLineInfo->SetFilename(bli.GetFilename());
        buildLineInfo->SetSeverity(bli.GetSeverity());
        buildLineInfo->SetLineNumber(bli.GetLineNumber());
        buildLineInfo->NormalizeFilename(m_directories, m_cygwinRoot);
        buildLineInfo->SetRegexLineMatch(bli.GetRegexLineMatch());
        buildLineInfo->SetColumn(bli.GetColumn());
        if(severity == SV_WARNING) {
            // Warning
            m_errorsAndWarningsList.push_back(buildLineInfo);
            m_warnCount++;
        } else {
            // Error
            m_errorsAndWarningsList.push_back(buildLineInfo);
            m_errorsList.push_back(buildLineInfo);
            m_errorCount++;
        }
    }
    return buildLineInfo;
}

void NewBuildTab::DoCacheRegexes()
{
    m_cmpPatterns.clear();

    // Loop over all known compilers and cache the regular expressions
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while(cmp) {
        CmpPatterns cmpPatterns;
        const Compiler::CmpListInfoPattern& errPatterns = cmp->GetErrPatterns();
        const Compiler::CmpListInfoPattern& warnPatterns = cmp->GetWarnPatterns();
        Compiler::CmpListInfoPattern::const_iterator iter;
        for(iter = errPatterns.begin(); iter != errPatterns.end(); iter++) {

            CmpPatternPtr compiledPatternPtr(new CmpPattern(new wxRegEx(iter->pattern, wxRE_ADVANCED | wxRE_ICASE),
                                                            iter->fileNameIndex, iter->lineNumberIndex,
                                                            iter->columnIndex, SV_ERROR));
            if(compiledPatternPtr->GetRegex()->IsValid()) { cmpPatterns.errorsPatterns.push_back(compiledPatternPtr); }
        }

        for(iter = warnPatterns.begin(); iter != warnPatterns.end(); iter++) {

            CmpPatternPtr compiledPatternPtr(new CmpPattern(new wxRegEx(iter->pattern, wxRE_ADVANCED | wxRE_ICASE),
                                                            iter->fileNameIndex, iter->lineNumberIndex,
                                                            iter->columnIndex, SV_WARNING));
            if(compiledPatternPtr->GetRegex()->IsValid()) { cmpPatterns.warningPatterns.push_back(compiledPatternPtr); }
        }

        m_cmpPatterns.insert(std::make_pair(cmp->GetName(), cmpPatterns));
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }
}

bool NewBuildTab::DoGetCompilerPatterns(const wxString& compilerName, CmpPatterns& patterns)
{
    MapCmpPatterns_t::iterator iter = m_cmpPatterns.find(compilerName);
    if(iter == m_cmpPatterns.end()) { return false; }
    patterns = iter->second;
    return true;
}

void NewBuildTab::DoClear()
{
    wxFont font = DoGetFont();
    m_lastLineColoured = wxNOT_FOUND;
    m_maxlineWidth = wxNOT_FOUND;
    m_buildInterrupted = false;
    m_directories.Clear();
    m_buildInfoPerFile.clear();
    m_warnCount = 0;
    m_errorCount = 0;
    m_errorsAndWarningsList.clear();
    m_errorsList.clear();
    m_cmpPatterns.clear();

    // Delete all the user data
    std::for_each(m_viewData.begin(), m_viewData.end(), [&](std::pair<int, BuildLineInfo*> p) { delete p.second; });
    m_viewData.clear();

    m_view->SetEditable(true);
    m_view->ClearAll();
    m_view->SetEditable(false);

    // Clear all markers from open editors
    std::vector<LEditor*> editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_Default);
    for(size_t i = 0; i < editors.size(); i++) {
        editors.at(i)->DelAllCompilerMarkers();
        editors.at(i)->AnnotationClearAll();
    }
    m_curError = m_errorsAndWarningsList.end();
}

void NewBuildTab::MarkEditor(LEditor* editor)
{
    if(!editor) return;

    editor->DelAllCompilerMarkers();
    editor->AnnotationClearAll();
    editor->AnnotationSetVisible(2); // Visible with box around it

    BuildTabSettingsData options;
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

    // Are markers or annotations enabled?
    if(options.GetErrorWarningStyle() == BuildTabSettingsData::EWS_NoMarkers) { return; }

    std::pair<MultimapBuildInfo_t::iterator, MultimapBuildInfo_t::iterator> iter =
        m_buildInfoPerFile.equal_range(editor->GetFileName().GetFullPath());
    if(iter.first == iter.second) {
        // could not find any, try the fullname
        iter = m_buildInfoPerFile.equal_range(editor->GetFileName().GetFullName());
#if defined(__WXGTK__)
        if(iter.first == iter.second) {
            // Nope. Perhaps it's a symlink
            iter = m_buildInfoPerFile.equal_range(CLRealPath(editor->GetFileName().GetFullPath()));
        }
#endif
    }

    // Merge all the errors from the same line into a single error
    AnnotationInfoByLineMap_t annotations;

    for(; iter.first != iter.second; ++iter.first) {
        BuildLineInfo* bli = iter.first->second;
        wxString text = m_view->GetLine(bli->GetLineInBuildTab()).Trim().Trim(false);

        // remove the line part from the text
        text = text.Mid(bli->GetRegexLineMatch());

        // if the line starts with ':' remove it as well
        text.StartsWith(":", &text);
        text.Trim().Trim(false);

        if(!text.IsEmpty()) {
            if(bli && (bli->GetSeverity() == SV_ERROR || bli->GetSeverity() == SV_WARNING)) {
                const auto annotationIter = annotations.lower_bound(bli->GetLineNumber());
                if ((annotationIter == annotations.end()) || (annotationIter->first != bli->GetLineNumber()))
                {
                    // insert new one
                    AnnotationInfo info;
                    info.line = bli->GetLineNumber();
                    info.severity = bli->GetSeverity();
                    info.text.emplace(std::move(text));
                    annotations.emplace_hint(annotationIter, bli->GetLineNumber(), std::move(info));
                }
                else {
                    // we already have an error on this line, concatenate the message
                    AnnotationInfo& info = annotationIter->second;
                    info.text.emplace(std::move(text));

                    if(bli->GetSeverity() == SV_ERROR) {
                        // override the severity to ERROR
                        info.severity = SV_ERROR;
                    }
                }
            }
        }
    }

    AnnotationInfoByLineMap_t::iterator annIter = annotations.begin();
    for(; annIter != annotations.end(); ++annIter) {
        wxString concatText;
        for (auto &text : annIter->second.text) {
            if (!concatText.IsEmpty())
                concatText << "\n";
            concatText << text;
        }

        if(annIter->second.severity == SV_ERROR) {
            editor->SetErrorMarker(annIter->first, concatText);
        } else {
            editor->SetWarningMarker(annIter->first, concatText);
        }
    }
    // now place the errors
    editor->Refresh();
}

void NewBuildTab::DoSearchForDirectory(const wxString& line)
{
    // Check for makefile directory changes lines
    if(line.Contains(wxT("Entering directory `"))) {
        wxString currentDir = line.AfterFirst(wxT('`'));
        currentDir = currentDir.BeforeLast(wxT('\''));

        // Collect the m_baseDir
        m_directories.Add(currentDir);

    } else if(line.Contains(wxT("Entering directory '"))) {
        wxString currentDir = line.AfterFirst(wxT('\''));
        currentDir = currentDir.BeforeLast(wxT('\''));

        // Collect the m_baseDir
        m_directories.Add(currentDir);
    }
}

void NewBuildTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    DoClear();
    InitView();
}

void NewBuildTab::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    DoClear();
    InitView();
}

void NewBuildTab::DoProcessOutput(bool compilationEnded, bool isSummaryLine)
{
    wxUnusedVar(isSummaryLine);
    if(!compilationEnded && m_output.Find(wxT("\n")) == wxNOT_FOUND) {
        // still dont have a complete line
        return;
    }

    wxArrayString lines = ::wxStringTokenize(m_output, wxT("\n"), wxTOKEN_RET_DELIMS);
    m_output.Clear();

    // Process only completed lines (i.e. a line that ends with '\n')
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        if(!compilationEnded && !lines.Item(i).EndsWith(wxT("\n"))) {
            m_output << lines.Item(i);
            return;
        }

        wxString buildLine = lines.Item(i); //.Trim().Trim(false);
        // If this is a line similar to 'Entering directory `'
        // add the path in the directories array
        DoSearchForDirectory(buildLine);
        BuildLineInfo* buildLineInfo = DoProcessLine(buildLine);

        // keep the line info
        if(buildLineInfo->GetFilename().IsEmpty() == false) {
            m_buildInfoPerFile.insert(std::make_pair(buildLineInfo->GetFilename(), buildLineInfo));
        }

        if(isSummaryLine) {
            buildLine.Trim();
            buildLine.Prepend("====");
            buildLine.Append("====");
            buildLineInfo->SetSeverity(SV_NONE);
        }

        // Keep the line number in the build tab
        buildLineInfo->SetLineInBuildTab(m_view->GetLineCount() - 1); // -1 because the view always has 1 extra "\n"
        // Store the line info *before* we add the text
        // it is needed in the OnStyle function
        m_viewData.insert(std::make_pair(buildLineInfo->GetLineInBuildTab(), buildLineInfo));

        m_view->SetEditable(true);
        buildLine.Trim();
        wxString modText;
        ::clStripTerminalColouring(buildLine, modText);

        int curline = m_view->GetLineCount() - 1;
        m_view->AppendText(modText + "\n");

        // get the newly added line width
        int endPosition = m_view->GetLineEndPosition(curline); // get character position from begin
        int beginPosition = m_view->PositionFromLine(curline); // and end of line

        wxPoint beginPos = m_view->PointFromPosition(beginPosition);
        wxPoint endPos = m_view->PointFromPosition(endPosition);

        int curLen = (endPos.x - beginPos.x) + 10;
        m_maxlineWidth = wxMax(m_maxlineWidth, curLen);
        if(m_maxlineWidth > 0) { m_view->SetScrollWidth(m_maxlineWidth); }
        m_view->SetEditable(false);

        if(clConfig::Get().Read(kConfigBuildAutoScroll, true)) { m_view->ScrollToEnd(); }
    }
}

void NewBuildTab::CenterLineInView(int line)
{
    if(line > m_view->GetLineCount()) return;
    int linesOnScreen = m_view->LinesOnScreen();
    // To place our line in the middle, the first visible line should be
    // the: line - (linesOnScreen / 2)
    int firstVisibleLine = line - (linesOnScreen / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }
    m_view->EnsureVisible(firstVisibleLine);
    m_view->SetFirstVisibleLine(firstVisibleLine);
    m_view->ClearSelections();
    m_view->SetCurrentPos(m_view->PositionFromLine(line));
}

void NewBuildTab::DoToggleWindow()
{
    bool success = m_errorCount == 0 && (m_skipWarnings || m_warnCount == 0);
    bool viewing = ManagerST::Get()->IsPaneVisible(wxT("Output View")) &&
                   (clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() == this);
    bool skipwarnings(false);

    if(!success) {
        if(viewing) {
            if(m_buildpaneScrollTo != ScrollToEnd) {
                // The user may have opted to go to the first error, the first item, or /dev/null
                skipwarnings = (m_errorCount > 0) && (m_buildpaneScrollTo == ScrollToFirstError);
                BuildLineInfo* bli = NULL;
                if(skipwarnings && !m_errorsList.empty()) {
                    bli = m_errorsList.front();

                } else if(!m_errorsAndWarningsList.empty()) {
                    bli = m_errorsAndWarningsList.front();
                }

                // Sanity
                if(bli) { CenterLineInView(bli->GetLineInBuildTab()); }
            }
        }

    } else if(m_autoHide && viewing && !m_buildInterrupted) {
        ManagerST::Get()->HidePane(clMainFrame::Get()->GetOutputPane()->GetCaption());

    } else if(m_showMe == BuildTabSettingsData::ShowOnEnd && !m_autoHide) {
        ManagerST::Get()->ShowOutputPane(BUILD_WIN);
    }
}

void NewBuildTab::OnNextBuildError(wxCommandEvent& e)
{
    // if we are here, we have at least something in the list of errors
    if(m_errorsAndWarningsList.empty()) return;

    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &m_buildTabSettings);
    bool skipWarnings = m_buildTabSettings.GetSkipWarnings();

    if(m_curError == m_errorsAndWarningsList.end()) {

        m_curError = m_errorsAndWarningsList.begin();
        if(m_curError == m_errorsAndWarningsList.end())
            // nothing to point to
            return;
    }

    // m_curError is valid
    if(skipWarnings) {

        do {
            if((*m_curError)->GetSeverity() == SV_ERROR) {
                // get the wxDataViewItem
                int line = (*m_curError)->GetLineInBuildTab();
                if(IS_VALID_LINE(line)) {
                    DoSelectAndOpen(line, true);
                    ++m_curError;
                    return;
                }

            } else {
                ++m_curError;
            }

        } while(m_curError != m_errorsAndWarningsList.end());

    } else {
        int line = (*m_curError)->GetLineInBuildTab();
        if(IS_VALID_LINE(line)) {
            DoSelectAndOpen(line, true);
            ++m_curError;
        }
    }
}

void NewBuildTab::OnNextBuildErrorUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_errorsAndWarningsList.empty() && !m_buildInProgress);
}

bool NewBuildTab::DoSelectAndOpen(int buildViewLine, bool centerLine)
{
    if(!m_viewData.count(buildViewLine)) { return false; }

    BuildLineInfo* bli = m_viewData.find(buildViewLine)->second;
    if(bli) {
        wxFileName fn(bli->GetFilename());

        // Highlight the clicked line on the view
        m_view->MarkerDeleteAll(LEX_GCC_MARKER);
        m_view->MarkerAdd(bli->GetLineInBuildTab(), LEX_GCC_MARKER);

        if(!fn.IsAbsolute()) {
            std::set<wxString> files;
            std::vector<wxFileName> candidates;
            ManagerST::Get()->GetWorkspaceFiles(files);

            std::for_each(files.begin(), files.end(), [&](const wxString& filepath) {
                wxFileName fnFilePath(filepath);
                if(fnFilePath.GetFullName() == fn.GetFullName()) { candidates.push_back(fnFilePath); }
            });

            if(candidates.empty()) { return false; }

            if(candidates.size() == 1) {
                fn = candidates.at(0);

            } else {
                // prompt the user
                wxArrayString fileArr;
                for(size_t i = 0; i < candidates.size(); ++i) {
                    fileArr.Add(candidates.at(i).GetFullPath());
                }

                clSingleChoiceDialog dlg(EventNotifier::Get()->TopFrame(), fileArr);
                dlg.SetLabel(_("Select a file to open"));
                if(dlg.ShowModal() != wxID_OK) return false;

                wxString selection = dlg.GetSelection();
                if(selection.IsEmpty()) return false;

                fn = wxFileName(selection);
                // if we resolved it now, open the file there is no point in searching this file
                // in m_buildInfoPerFile since the key on this map is kept as full name
                LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
                if(!editor) {
                    editor = clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxT(""),
                                                                         bli->GetLineNumber(), wxNOT_FOUND, OF_AddJump);
                }

                if(editor) {
                    DoCentreErrorLine(bli, editor, centerLine);
                    return true;
                }
            }
        }

        if(fn.IsAbsolute()) {

            // try to locate the editor first
            LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
            if(!editor) {
                // Open it
                editor = clMainFrame::Get()->GetMainBook()->OpenFile(bli->GetFilename(), wxT(""), bli->GetLineNumber(),
                                                                     wxNOT_FOUND, OF_AddJump);
            }

            if(editor) {
                if(!editor->HasCompilerMarkers()) MarkEditor(editor);

                int lineNumber = bli->GetLineNumber();
                if(lineNumber > 0) { lineNumber--; }

                DoCentreErrorLine(bli, editor, centerLine);
                return true;
            }
        }
    }
    return false;
}

wxString NewBuildTab::GetBuildContent() const { return m_view->GetText(); }

wxFont NewBuildTab::DoGetFont() const
{
    wxFont font = wxNullFont;
    LexerConf::Ptr_t lexerConf = EditorConfigST::Get()->GetLexer("C++");
    if(lexerConf) { font = lexerConf->GetFontForSyle(wxSTC_C_DEFAULT); }

    if(font.IsOk() == false) { font = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT); }

    // if the user selected a font in the settings, use it instead
    wxFont userFont = m_buildTabSettings.GetBuildFont();
    if(userFont.IsOk()) { font = userFont; }
    return font;
}

void NewBuildTab::OnMenu(wxContextMenuEvent& e)
{
    wxMenu menu;
    menu.Append(wxID_COPY, _("Copy Selected Line"));
    menu.Append(XRCID("copy_all"), _("Copy Entire Build Output To Clipboard"));
    menu.Append(wxID_PASTE, _("Open Build Output in an Empty Editor"));
    menu.AppendSeparator();
    menu.Append(wxID_CLEAR, _("Clear"));
    m_view->PopupMenu(&menu);
}

void NewBuildTab::OnOpenInEditor(wxCommandEvent& e)
{
    wxString content = this->GetBuildContent();
    LEditor* editor = clMainFrame::Get()->GetMainBook()->NewEditor();
    if(editor) { editor->SetText(content); }
}

void NewBuildTab::OnOpenInEditorUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && !m_view->IsEmpty()); }
void NewBuildTab::OnClear(wxCommandEvent& e) { Clear(); }

void NewBuildTab::OnClearUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && !IsEmpty()); }

void NewBuildTab::ScrollToBottom() { m_view->ScrollToEnd(); }

void NewBuildTab::AppendLine(const wxString& text)
{
    m_output << text;
    DoProcessOutput(false, false);
}

void NewBuildTab::OnStyleNeeded(wxStyledTextEvent& event)
{
    int startPos = m_view->GetEndStyled();
    int endPos = event.GetPosition();
    wxString text = m_view->GetTextRange(startPos, endPos);
#if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
    // The scintilla syntax in e.g. wx3.1.1 changed
    m_view->StartStyling(startPos);
#else
    m_view->StartStyling(startPos, 0x1f);
#endif

    int curline = m_view->GetLineCount();
    curline -= 1; // The view always ends with a "\n", we don't count it as a line
    wxArrayString lines = ::wxStringTokenize(text, wxT("\n"), wxTOKEN_RET_DELIMS);

    // the last line that we coloured
    curline -= lines.size();

    for(size_t i = 0; i < lines.size(); ++i) {
        const wxString& strLine = lines.Item(i);
        if(m_viewData.count(curline)) {
            BuildLineInfo* b = m_viewData.find(curline)->second;
            switch(b->GetSeverity()) {
            case SV_WARNING:
                m_view->SetStyling(strLine.length(), LEX_GCC_WARNING);
                break;
            case SV_ERROR:
                m_view->SetStyling(strLine.length(), LEX_GCC_ERROR);
                break;
            case SV_SUCCESS:
                m_view->SetStyling(strLine.length(), LEX_GCC_DEFAULT);
                break;
            case SV_DIR_CHANGE:
                m_view->SetStyling(strLine.length(), LEX_GCC_INFO);
                break;
            case SV_NONE:
            default:
                m_view->SetStyling(strLine.length(), LEX_GCC_DEFAULT);
                break;
            }
        } else {
            m_view->SetStyling(strLine.length(), LEX_GCC_DEFAULT);
        }
        ++curline;
    }
}

void NewBuildTab::InitView(const wxString& theme)
{
    LexerConf::Ptr_t lexText = ColoursAndFontsManager::Get().GetLexer("text", theme);
    lexText->Apply(m_view);

    m_view->SetUndoCollection(false);
    m_view->EmptyUndoBuffer();
    m_view->HideSelection(true);
    m_view->SetEditable(false);

    StyleProperty::Map_t& props = lexText->GetLexerProperties();
    const StyleProperty& defaultStyle = lexText->GetProperty(0);

    // reset the styles
    m_view->SetLexer(wxSTC_LEX_CONTAINER);
    wxFont defaultFont = lexText->GetFontForSyle(0);
    for(size_t i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_view->StyleSetForeground(i, defaultStyle.GetFgColour());
        m_view->StyleSetBackground(i, defaultStyle.GetBgColour());
        m_view->StyleSetFont(i, defaultFont);
    }

    m_view->StyleSetForeground(LEX_GCC_INFO, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    m_view->StyleSetForeground(LEX_GCC_DEFAULT, props[0].GetFgColour());
    m_view->StyleSetForeground(LEX_GCC_WARNING, this->m_buildTabSettings.GetWarnColour());
    m_view->StyleSetForeground(LEX_GCC_ERROR, this->m_buildTabSettings.GetErrorColour());

    m_view->StyleSetHotSpot(LEX_GCC_ERROR, true);
    m_view->StyleSetHotSpot(LEX_GCC_WARNING, true);
    m_view->SetHotspotActiveUnderline(false); // No underline

    m_view->MarkerDefine(LEX_GCC_MARKER, wxSTC_MARK_BACKGROUND);
    if(lexText->IsDark()) {
        m_view->MarkerSetBackground(LEX_GCC_MARKER, wxColour(defaultStyle.GetBgColour()).ChangeLightness(110));
    } else {
        m_view->MarkerSetBackground(LEX_GCC_MARKER, wxColour(defaultStyle.GetBgColour()).ChangeLightness(90));
    }

    // Hide all margins
    for(int i = 0; i <= wxSTC_MARGIN_RTEXT; ++i) {
        m_view->SetMarginWidth(i, 0);
    }

    // make the symbol margin 5 pixel width
    m_view->SetMarginType(0, wxSTC_MARGIN_SYMBOL);
    m_view->SetMarginWidth(0, 5);
}

void NewBuildTab::OnHotspotClicked(wxStyledTextEvent& event)
{
    long pos = event.GetPosition();
    int line = m_view->LineFromPosition(pos);
    DoSelectAndOpen(line, false);
}

void NewBuildTab::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    InitView();
}

void NewBuildTab::DoCentreErrorLine(BuildLineInfo* bli, LEditor* editor, bool centerLine)
{
    // We already got compiler markers set here, just goto the line
    clMainFrame::Get()->GetMainBook()->SelectPage(editor);
    CHECK_PTR_RET(bli);

    // Convert the compiler column to scintilla's position
    if(bli->GetColumn() != wxNOT_FOUND) {
        editor->CenterLine(bli->GetLineNumber(), bli->GetColumn() - 1);
    } else {
        editor->CenterLine(bli->GetLineNumber());
    }

    if(centerLine) {
        // If the line in the build error tab is not visible, ensure it is
        int linesOnScreen = m_view->LinesOnScreen();

        // Our line is not visible
        int firstVisibleLine = bli->GetLineInBuildTab() - (linesOnScreen / 2);
        if(firstVisibleLine < 0) { firstVisibleLine = 0; }
        m_view->EnsureVisible(firstVisibleLine);
        m_view->SetFirstVisibleLine(firstVisibleLine);
    }
    SetActive(editor);
}

void NewBuildTab::ColourOutput()
{
    // Loop over the lines and colour them
    int fromLine = (m_lastLineColoured == wxNOT_FOUND) ? 0 : m_lastLineColoured;
    int untilLine = (m_view->GetLineCount() - 1);

    if(fromLine == untilLine) { return; }

    for(int i = fromLine; (i < untilLine) && (untilLine >= fromLine); ++i) {
        int startPos = m_view->PositionFromLine(i);
        int lineEndPos = m_view->GetLineEndPosition(i);

#if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
        // The scintilla syntax in e.g. wx3.1.1 changed
        m_view->StartStyling(startPos);
#else
        m_view->StartStyling(startPos, 0x1f);
#endif

        // Run the regexes
        wxString lineText = m_view->GetLine(i);
        LINE_SEVERITY severity;
        GetMatchingRegex(lineText, severity);
        switch(severity) {
        case SV_WARNING:
            m_view->SetStyling((lineEndPos - startPos), LEX_GCC_WARNING);
            break;
        case SV_ERROR:
            m_view->SetStyling((lineEndPos - startPos), LEX_GCC_ERROR);
            break;
        case SV_SUCCESS:
            m_view->SetStyling((lineEndPos - startPos), LEX_GCC_DEFAULT);
            break;
        case SV_DIR_CHANGE:
            m_view->SetStyling((lineEndPos - startPos), LEX_GCC_INFO);
            break;
        case SV_NONE:
        default:
            m_view->SetStyling((lineEndPos - startPos), LEX_GCC_DEFAULT);
            break;
        }
    }
    m_lastLineColoured = untilLine;
}

CmpPatternPtr NewBuildTab::GetMatchingRegex(const wxString& lineText, LINE_SEVERITY& severity)
{
    if(lineText.Lower().Contains("entering directory") || lineText.Lower().Contains("leaving directory")) {
        severity = SV_DIR_CHANGE;
        return NULL;

    } else if(lineText.StartsWith("====")) {
        severity = SV_NONE;
        return NULL;

    } else {

        // Find *warnings* first
        bool isWarning = false;

        if(!m_cmp) {
            severity = SV_NONE;
            return NULL;
        }

        CmpPatterns cmpPatterns;
        if(!DoGetCompilerPatterns(m_cmp->GetName(), cmpPatterns)) {
            severity = SV_NONE;
            return NULL;
        }

        // If it is not an error, maybe it's a warning
        for(size_t i = 0; i < cmpPatterns.warningPatterns.size(); i++) {
            CmpPatternPtr cmpPatterPtr = cmpPatterns.warningPatterns.at(i);
            BuildLineInfo bli;
            if(cmpPatterPtr->Matches(lineText, bli)) {
                severity = SV_WARNING;
                return cmpPatterPtr;
            }
        }
        if(!isWarning) {
            for(size_t i = 0; i < cmpPatterns.errorsPatterns.size(); i++) {
                BuildLineInfo bli;
                CmpPatternPtr cmpPatterPtr = cmpPatterns.errorsPatterns.at(i);
                if(cmpPatterPtr->Matches(lineText, bli)) {
                    severity = SV_ERROR;
                    return cmpPatterPtr;
                }
            }
        }
    }

    // Default
    severity = SV_NONE;
    return NULL;
}

void NewBuildTab::OnIdle(wxIdleEvent& event)
{
    if(m_view->IsEmpty()) { return; }
    ColourOutput();
}

////////////////////////////////////////////
// CmpPatter

bool CmpPattern::Matches(const wxString& line, BuildLineInfo& lineInfo)
{
    long fidx, lidx;
    if(!m_fileIndex.ToLong(&fidx) || !m_lineIndex.ToLong(&lidx)) return false;

    if(!m_regex || !m_regex->IsValid()) return false;
    if(!m_regex->Matches(line)) return false;

    long colIndex;
    if(!m_colIndex.ToLong(&colIndex)) return false;

    lineInfo.SetSeverity(m_severity);
    if(m_regex->GetMatchCount() > (size_t)fidx) { lineInfo.SetFilename(m_regex->GetMatch(line, fidx)); }

    // keep the match length
    lineInfo.SetRegexLineMatch(m_regex->GetMatch(line, 0).length());

    if(m_regex->GetMatchCount() > (size_t)lidx) {
        long lineNumber;
        wxString strLine = m_regex->GetMatch(line, lidx);
        strLine.ToLong(&lineNumber);
        lineInfo.SetLineNumber(--lineNumber);
    }

    if(m_regex->GetMatchCount() > (size_t)colIndex) {
        long column;
        wxString strCol = m_regex->GetMatch(line, colIndex);
        if(strCol.StartsWith(":")) { strCol.Remove(0, 1); }

        if(!strCol.IsEmpty() && strCol.ToLong(&column)) { lineInfo.SetColumn(column); }
    }
    return true;
}

void BuildLineInfo::NormalizeFilename(const wxArrayString& directories, const wxString& cygwinPath)
{
    wxFileName fn(this->GetFilename());

    if(fn.IsAbsolute()) {
        SetFilename(fn.GetFullPath());
        return;

    } else if(fn.IsAbsolute(wxPATH_UNIX) && IS_WINDOWS && !cygwinPath.IsEmpty()) {

        wxFileName cygfile(fn);
        wxString path = cygwinPath + cygfile.GetFullPath();
        SetFilename(wxFileName(path).GetFullPath());
        return;
    }

    if(directories.IsEmpty()) {
        SetFilename(fn.GetFullName());
        return;
    }

    // we got a relative file name
    int dircount = directories.GetCount();
    for(int i = dircount - 1; i >= 0; --i) {
        wxFileName tmp = fn;
        if(tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG)) {
            // Windows sanity
            if(IS_WINDOWS && tmp.GetVolume().length() > 1) {
                // Invalid file path
                SetFilename("");
                return;
            }

            if(tmp.FileExists() && tmp.MakeAbsolute(directories.Item(i))) {
                SetFilename(tmp.GetFullPath());
                return;
            }
        }
    }

    // One more try: the above will fail if the project isn't stored in the cwd that Normalize() assumes
    // So see if one of 'directories' is the correct path to use
    for(int i = dircount - 1; i >= 0; --i) {
        wxFileName tmp = fn;
        if(tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG, directories.Item(i))) {
            // Windows sanity
            if(IS_WINDOWS && tmp.GetVolume().length() > 1) {
                // Invalid file path
                SetFilename("");
                return;
            }

            if(tmp.FileExists()) {
                SetFilename(tmp.GetFullPath());
                return;
            }
        }
    }

    // failed.. keep it as fullname only
    SetFilename(fn.GetFullName());
}

void BuildLineInfo::SetFilename(const wxString& filename)
{
#if defined(__WXGTK__)
    // CLRealPath copes with any symlinks in the filepath
    m_filename = CLRealPath(filename);
#else
    this->m_filename = filename;
#endif
}
