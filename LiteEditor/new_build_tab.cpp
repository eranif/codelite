//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

#include "new_build_tab.h"
#include "file_logger.h"
#include "environmentconfig.h"
#include "build_settings_config.h"
#include <wx/choicdlg.h>
#include "BuildTabTopPanel.h"
#include "workspace.h"
#include "globals.h"
#include "frame.h"
#include "cl_editor.h"
#include "manager.h"
#include "shell_command.h"
#include "event_notifier.h"
#include <wx/dataview.h>
#include <wx/settings.h>
#include <wx/regex.h>
#include "pluginmanager.h"
#include "editor_config.h"
#include "bitmap_loader.h"
#include <wx/dcmemory.h>
#include "notebook_ex.h"
#include "output_pane.h"
#include "macros.h"
#include <wx/fdrepdlg.h>
#include "buildtabsettingsdata.h"
#include "cl_command_event.h"

static size_t BUILD_PANE_WIDTH = 10000;

static const wxChar* WARNING_MARKER = wxT("@@WARNING@@");
static const wxChar* ERROR_MARKER = wxT("@@ERROR@@");
static const wxChar* SUMMARY_MARKER_ERROR = wxT("@@SUMMARY_ERROR@@");
static const wxChar* SUMMARY_MARKER_WARNING = wxT("@@SUMMARY_WARNING@@");
static const wxChar* SUMMARY_MARKER_SUCCESS = wxT("@@SUMMARY_SUCCESS@@");
static const wxChar* SUMMARY_MARKER = wxT("@@SUMMARY@@");

#define IS_VALID_LINE(lineNumber) ((lineNumber >= 0 && lineNumber < m_listctrl->GetItemCount()))
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

static void StripBuildMarkders(wxString& s)
{
    s.StartsWith(WARNING_MARKER, &s);
    s.StartsWith(ERROR_MARKER, &s);
    s.StartsWith(SUMMARY_MARKER, &s);
    s.StartsWith(SUMMARY_MARKER_ERROR, &s);
    s.StartsWith(SUMMARY_MARKER_SUCCESS, &s);
    s.StartsWith(SUMMARY_MARKER_WARNING, &s);
}

// A renderer for drawing the text
class MyTextRenderer : public wxDataViewCustomRenderer
{
    wxFont m_font;
    wxColour m_greyColor;
    wxDataViewListCtrl* m_listctrl;
    wxColour m_warnFgColor;
    wxColour m_errFgColor;
    wxVariant m_value;
    wxBitmap m_errorBmp;
    wxBitmap m_warningBmp;
    wxBitmap m_successBmp;
    int m_charWidth;

public:
    MyTextRenderer(wxDataViewListCtrl* listctrl)
        : m_listctrl(listctrl)
        , m_charWidth(12)
    {
        m_errorBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/error-message"));
        m_warningBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/warning-message"));
        m_successBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/success-message"));

#if defined(__WXGTK__) || defined(__WXMAC__)
        m_greyColor = wxColour(wxT("GREY"));
#else
        m_greyColor = wxColour(wxT("LIGHT GREY"));
#endif
        EnableEllipsize();
    }

    virtual ~MyTextRenderer() {}

    virtual wxSize GetSize() const
    {
        int xx, yy;
        wxBitmap bmp(1, 1);
        wxMemoryDC dc;
        dc.SelectObject(bmp);

        wxString s = m_value.GetString();
        wxFont f = m_font;
        dc.GetTextExtent(s, &xx, &yy, NULL, NULL, &f);

        // Adjust the height to fit the bitmap height at least
        yy < m_errorBmp.GetHeight() ? yy = m_errorBmp.GetHeight() : yy = yy;
        return wxSize(xx, yy);
    }

    virtual bool SetValue(const wxVariant& value)
    {
        m_value = value;
        return true;
    }

    virtual bool GetValue(wxVariant& value) const
    {
        value = m_value;
        return true;
    }

    void SetErrFgColor(const wxColour& errFgColor) { this->m_errFgColor = errFgColor; }
    void SetWarnFgColor(const wxColour& warnFgColor) { this->m_warnFgColor = warnFgColor; }
    const wxColour& GetErrFgColor() const { return m_errFgColor; }
    const wxColour& GetWarnFgColor() const { return m_warnFgColor; }
    virtual bool Render(wxRect cell, wxDC* dc, int state)
    {
        wxVariant v;
        GetValue(v);
        wxString str = v.GetString();
        str.Trim();
        wxPoint pt = cell.GetTopLeft();
        wxFont f = m_font;
        bool isSelected = false; // state & wxDATAVIEW_CELL_SELECTED;

        if(str.StartsWith(ERROR_MARKER, &str)) {
            if(!isSelected) {
                dc->SetTextForeground(m_errFgColor);
            }

        } else if(str.StartsWith(WARNING_MARKER, &str)) {
            if(!isSelected) {
                dc->SetTextForeground(m_warnFgColor);
            }

        } else if(str.StartsWith(SUMMARY_MARKER, &str)) {
            f.SetWeight(wxFONTWEIGHT_BOLD);

        } else if(str.StartsWith(wxT("----"))) {
            f.SetStyle(wxFONTSTYLE_ITALIC);
            if(!isSelected)
                dc->SetTextForeground(m_greyColor);

        } else if(str.Contains(wxT("Entering directory")) || str.Contains(wxT("Leaving directory"))) {
            f.SetStyle(wxFONTSTYLE_ITALIC);
            if(!isSelected)
                dc->SetTextForeground(m_greyColor);
        }

        if(str.StartsWith(SUMMARY_MARKER_ERROR, &str)) {
            dc->DrawBitmap(m_errorBmp, pt);
            pt.x += m_errorBmp.GetWidth() + 2;
            str.Prepend(wxT(": "));

        } else if(str.StartsWith(SUMMARY_MARKER_WARNING, &str)) {
            dc->DrawBitmap(m_warningBmp, pt);
            pt.x += m_warningBmp.GetWidth() + 2;
            str.Prepend(wxT(": "));

        } else if(str.StartsWith(SUMMARY_MARKER_SUCCESS, &str)) {
            dc->DrawBitmap(m_successBmp, pt);
            pt.x += m_successBmp.GetWidth() + 2;
            str.Prepend(wxT(": "));
        }

        dc->SetFont(f);

        if((str.length() * m_charWidth) > BUILD_PANE_WIDTH) {
            size_t newWidth = (BUILD_PANE_WIDTH / m_charWidth) - 1;
            str = str.Mid(0, newWidth);
        }

        dc->DrawText(str, pt);
        return true;
    }
    void SetFont(const wxFont& font)
    {

        this->m_font = font;

        // Calculate a single character width
        wxMemoryDC memDc;
        wxBitmap bmp(1, 1);
        memDc.SelectObject(bmp);
        memDc.SetFont(m_font);
        wxSize sz = memDc.GetTextExtent("X");
        m_charWidth = sz.x;
    }
};

//////////////////////////////////////////////////////////////

struct AnnotationInfo
{
    int line;
    LINE_SEVERITY severity;
    wxString text;
};
typedef std::map<int, AnnotationInfo> AnnotationInfoByLineMap_t;

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
{
    m_curError = m_errorsAndWarningsList.end();
    wxBoxSizer* bs = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(bs);

    // Determine the row height
    wxBitmap tmpBmp(1, 1);
    wxMemoryDC memDc;
    memDc.SelectObject(tmpBmp);
    wxFont fnt = DoGetFont();
    int xx, yy;
    memDc.GetTextExtent(wxT("Tp"), &xx, &yy, NULL, NULL, &fnt);
    int style = wxDV_NO_HEADER | wxDV_MULTIPLE;
    // style |= wxDV_ROW_LINES;

    m_listctrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_listctrl->Connect(
        wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxContextMenuEventHandler(NewBuildTab::OnMenu), NULL, this);

    m_listctrl->Connect(
        XRCID("copy_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NewBuildTab::OnCopy), NULL, this);
    m_listctrl->Connect(
        wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NewBuildTab::OnCopySelection), NULL, this);
    m_listctrl->Connect(
        wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NewBuildTab::OnOpenInEditor), NULL, this);
    m_listctrl->Connect(
        wxID_CLEAR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(NewBuildTab::OnClear), NULL, this);

    m_listctrl->Connect(XRCID("copy_all"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewBuildTab::OnCopyUI), NULL, this);
    m_listctrl->Connect(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewBuildTab::OnCopySelectionUI), NULL, this);
    m_listctrl->Connect(wxID_PASTE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewBuildTab::OnOpenInEditorUI), NULL, this);
    m_listctrl->Connect(wxID_CLEAR, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewBuildTab::OnClearUI), NULL, this);

    // Make sure we have enought height for the icon
    yy < 12 ? yy = 12 : yy = yy;
    m_listctrl->SetRowHeight(yy);

    bs->Add(m_listctrl, 1, wxEXPAND | wxALL);

    BuildTabTopPanel* toolbox = new BuildTabTopPanel(this);

#ifdef __WXMAC__
    bs->Add(toolbox, 0, wxEXPAND);
#else
    bs->Insert(0, toolbox, 0, wxEXPAND);
#endif

    int screenWidth = BUILD_PANE_WIDTH; // use a long screen width to allow long lines
    m_textRenderer = new MyTextRenderer(m_listctrl);

    m_listctrl->AppendColumn(new wxDataViewColumn(_("Message"), m_textRenderer, 0, screenWidth, wxALIGN_LEFT));

    EventNotifier::Get()->Connect(
        wxEVT_SHELL_COMMAND_STARTED, clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_SHELL_COMMAND_ADDLINE, clCommandEventHandler(NewBuildTab::OnBuildAddLine), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_SHELL_COMMAND_PROCESS_ENDED, clCommandEventHandler(NewBuildTab::OnBuildEnded), NULL, this);

    EventNotifier::Get()->Connect(
        wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(NewBuildTab::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(NewBuildTab::OnWorkspaceClosed), NULL, this);

    wxTheApp->Connect(XRCID("next_build_error"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(NewBuildTab::OnNextBuildError),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("next_build_error"),
                      wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(NewBuildTab::OnNextBuildErrorUI),
                      NULL,
                      this);

    m_listctrl->Connect(
        wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(NewBuildTab::OnLineSelected), NULL, this);
}

NewBuildTab::~NewBuildTab()
{
    m_listctrl->Disconnect(
        wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxContextMenuEventHandler(NewBuildTab::OnMenu), NULL, this);

    EventNotifier::Get()->Disconnect(
        wxEVT_SHELL_COMMAND_STARTED, clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, clCommandEventHandler(NewBuildTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_SHELL_COMMAND_ADDLINE, clCommandEventHandler(NewBuildTab::OnBuildAddLine), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_SHELL_COMMAND_PROCESS_ENDED, clCommandEventHandler(NewBuildTab::OnBuildEnded), NULL, this);
    wxTheApp->Disconnect(XRCID("next_build_error"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(NewBuildTab::OnNextBuildError),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("next_build_error"),
                         wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(NewBuildTab::OnNextBuildErrorUI),
                         NULL,
                         this);
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

    // notify the plugins that the build has ended
    clBuildEvent buildEvent(wxEVT_BUILD_ENDED);
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

        if(arrOut.IsEmpty() == false) {
            m_cygwinRoot = arrOut.Item(0);
        }
    }

    m_buildInProgress = true;

    // Reload the build settings data
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &m_buildTabSettings);
    m_textRenderer->SetErrFgColor(m_buildTabSettings.GetErrorColour());
    m_textRenderer->SetWarnFgColor(m_buildTabSettings.GetWarnColour());

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
    size_t sel = opane->GetNotebook()->GetSelection();
    if(sel != Notebook::npos)
        win = opane->GetNotebook()->GetPage(sel);

    if(m_showMe == BuildTabSettingsData::ShowOnStart) {
        ManagerST::Get()->ShowOutputPane(OutputPane::BUILD_WIN, true);

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
            WorkspaceST::Get()->GetProjBuildConf(bed->GetProjectName(), bed->GetConfiguration());
        if(buildConfig) {
            m_cmp = buildConfig->GetCompiler();
        }

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

BuildLineInfo* NewBuildTab::DoProcessLine(const wxString& line, bool isSummaryLine)
{
    BuildLineInfo* buildLineInfo = new BuildLineInfo();

    if(isSummaryLine) {
        // Set the severity
        if(m_errorCount == 0 && m_warnCount == 0) {
            buildLineInfo->SetSeverity(SV_SUCCESS);

        } else if(m_errorCount) {
            buildLineInfo->SetSeverity(SV_ERROR);

        } else {

            buildLineInfo->SetSeverity(SV_WARNING);
        }

    } else {

        // Find *warnings* first
        bool isWarning = false;

        if(!m_cmp) {
            return buildLineInfo;
        }

        CmpPatterns cmpPatterns;
        if(!DoGetCompilerPatterns(m_cmp->GetName(), cmpPatterns)) {
            return buildLineInfo;
        }

        // If it is not an error, maybe it's a warning
        for(size_t i = 0; i < cmpPatterns.warningPatterns.size(); i++) {
            CmpPatternPtr cmpPatterPtr = cmpPatterns.warningPatterns.at(i);
            BuildLineInfo bli;
            if(cmpPatterPtr->Matches(line, bli)) {
                buildLineInfo->SetFilename(bli.GetFilename());
                buildLineInfo->SetSeverity(bli.GetSeverity());
                buildLineInfo->SetLineNumber(bli.GetLineNumber());
                buildLineInfo->NormalizeFilename(m_directories, m_cygwinRoot);
                buildLineInfo->SetRegexLineMatch(bli.GetRegexLineMatch());

                // keep this info in the errors+warnings list only
                m_errorsAndWarningsList.push_back(buildLineInfo);

                m_warnCount++;
                isWarning = true;
                break;
            }
        }
        if(!isWarning) {
            for(size_t i = 0; i < cmpPatterns.errorsPatterns.size(); i++) {
                BuildLineInfo bli;
                CmpPatternPtr cmpPatterPtr = cmpPatterns.errorsPatterns.at(i);
                if(cmpPatterPtr->Matches(line, bli)) {
                    buildLineInfo->SetFilename(bli.GetFilename());
                    buildLineInfo->SetSeverity(bli.GetSeverity());
                    buildLineInfo->SetLineNumber(bli.GetLineNumber());
                    buildLineInfo->NormalizeFilename(m_directories, m_cygwinRoot);
                    buildLineInfo->SetRegexLineMatch(bli.GetRegexLineMatch());

                    // keep this info in both lists (errors+warnings AND errors)
                    m_errorsAndWarningsList.push_back(buildLineInfo);
                    m_errorsList.push_back(buildLineInfo);
                    m_errorCount++;
                    break;
                }
            }
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

            CmpPatternPtr compiledPatternPtr(
                new CmpPattern(new wxRegEx(iter->pattern), iter->fileNameIndex, iter->lineNumberIndex, SV_ERROR));
            if(compiledPatternPtr->GetRegex()->IsValid()) {
                cmpPatterns.errorsPatterns.push_back(compiledPatternPtr);
            }
        }

        for(iter = warnPatterns.begin(); iter != warnPatterns.end(); iter++) {

            CmpPatternPtr compiledPatternPtr(
                new CmpPattern(new wxRegEx(iter->pattern), iter->fileNameIndex, iter->lineNumberIndex, SV_WARNING));
            if(compiledPatternPtr->GetRegex()->IsValid()) {
                cmpPatterns.warningPatterns.push_back(compiledPatternPtr);
            }
        }

        m_cmpPatterns.insert(std::make_pair(cmp->GetName(), cmpPatterns));
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }
}

bool NewBuildTab::DoGetCompilerPatterns(const wxString& compilerName, CmpPatterns& patterns)
{
    MapCmpPatterns_t::iterator iter = m_cmpPatterns.find(compilerName);
    if(iter == m_cmpPatterns.end()) {
        return false;
    }
    patterns = iter->second;
    return true;
}

void NewBuildTab::DoClear()
{
    wxFont font = DoGetFont();
    m_textRenderer->SetFont(font);

    m_buildInterrupted = false;
    m_directories.Clear();
    m_buildInfoPerFile.clear();
    m_warnCount = 0;
    m_errorCount = 0;
    m_errorsAndWarningsList.clear();
    m_errorsList.clear();
    m_cmpPatterns.clear();

    // Delete all the user data
    int count = m_listctrl->GetItemCount();
    for(int i = 0; i < count; i++) {
        wxDataViewItem item = m_listctrl->GetStore()->GetItem(i);
        if(item.IsOk()) {
            BuildLineInfo* bli = (BuildLineInfo*)m_listctrl->GetItemData(item);
            if(bli) {
                delete bli;
            }
        }
    }
    m_listctrl->DeleteAllItems();

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
    if(!editor)
        return;

    editor->DelAllCompilerMarkers();
    editor->AnnotationClearAll();
    editor->AnnotationSetVisible(2); // Visible with box around it

    BuildTabSettingsData options;
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

    // Are markers or annotations enabled?
    if(options.GetErrorWarningStyle() == BuildTabSettingsData::EWS_NoMarkers) {
        return;
    }

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

    editor->InitializeAnnotations();

    // Merge all the errors from the same line into a single error
    AnnotationInfoByLineMap_t annotations;

    for(; iter.first != iter.second; ++iter.first) {
        BuildLineInfo* bli = iter.first->second;
        wxString text = m_listctrl->GetTextValue(bli->GetLineInBuildTab(), 0).Trim().Trim(false);

        // strip any build markers
        StripBuildMarkders(text);

        // remove the line part from the text
        text = text.Mid(bli->GetRegexLineMatch());

        // if the line starts with ':' remove it as well
        text.StartsWith(":", &text);
        text.Trim().Trim(false);

        if(!text.IsEmpty()) {
            if(bli && (bli->GetSeverity() == SV_ERROR || bli->GetSeverity() == SV_WARNING)) {
                if(annotations.count(bli->GetLineNumber())) {
                    // we already have an error on this line, concatenate the message
                    AnnotationInfo& info = annotations[bli->GetLineNumber()];
                    info.text << "\n" << text;

                    if(bli->GetSeverity() == SV_ERROR) {
                        // override the severity to ERROR
                        info.severity = SV_ERROR;
                    }

                } else {
                    // insert new one
                    AnnotationInfo info;
                    info.line = bli->GetLineNumber();
                    info.severity = bli->GetSeverity();
                    info.text = text;
                    annotations.insert(std::make_pair(bli->GetLineNumber(), info));
                }
            }
        }
    }

    AnnotationInfoByLineMap_t::iterator annIter = annotations.begin();
    for(; annIter != annotations.end(); ++annIter) {
        if(annIter->second.severity == SV_ERROR) {
            editor->SetErrorMarker(annIter->first, annIter->second.text);
        } else {
            editor->SetWarningMarker(annIter->first, annIter->second.text);
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
    }
}

void NewBuildTab::OnLineSelected(wxDataViewEvent& e)
{
    if(e.GetItem().IsOk()) {
        DoSelectAndOpen(e.GetItem());
    }
}

void NewBuildTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    DoClear();
}

void NewBuildTab::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    DoClear();
}

void NewBuildTab::DoProcessOutput(bool compilationEnded, bool isSummaryLine)
{
    if(!compilationEnded && m_output.Find(wxT("\n")) == wxNOT_FOUND) {
        // still dont have a complete line
        return;
    }

    wxArrayString lines = ::wxStringTokenize(m_output, wxT("\n"), wxTOKEN_RET_DELIMS);
    m_output.Clear();

    // Process only completed lines (i.e. a line that ends with '\n')
    for(size_t i = 0; i < lines.GetCount(); i++) {
        if(!compilationEnded && !lines.Item(i).EndsWith(wxT("\n"))) {
            m_output << lines.Item(i);
            return;
        }

        wxString buildLine = lines.Item(i); //.Trim().Trim(false);
        // If this is a line similar to 'Entering directory `'
        // add the path in the directories array
        DoSearchForDirectory(buildLine);
        BuildLineInfo* buildLineInfo = DoProcessLine(buildLine, isSummaryLine);

        // keep the line info
        if(buildLineInfo->GetFilename().IsEmpty() == false) {
            m_buildInfoPerFile.insert(std::make_pair(buildLineInfo->GetFilename(), buildLineInfo));
        }

        // Append the line content

        if(buildLineInfo->GetSeverity() == SV_ERROR) {
            if(!isSummaryLine) {
                buildLine.Prepend(ERROR_MARKER);
            }

        } else if(buildLineInfo->GetSeverity() == SV_WARNING) {
            if(!isSummaryLine) {
                buildLine.Prepend(WARNING_MARKER);
            }
        }

        if(isSummaryLine) {

            // Add a marker for drawing the bitmap
            if(m_errorCount) {
                buildLine.Prepend(SUMMARY_MARKER_ERROR);

            } else if(m_warnCount) {
                buildLine.Prepend(SUMMARY_MARKER_WARNING);

            } else {
                buildLine.Prepend(SUMMARY_MARKER_SUCCESS);
            }
            buildLine.Prepend(SUMMARY_MARKER);
        }

        wxVector<wxVariant> data;
        data.push_back(wxVariant(buildLine));

        // Keep the line number in the build tab
        buildLineInfo->SetLineInBuildTab(m_listctrl->GetItemCount());
        m_listctrl->AppendItem(data, (wxUIntPtr)buildLineInfo);

        if(clConfig::Get().Read("build-auto-scroll", true)) {
            unsigned int count = m_listctrl->GetStore()->GetItemCount();
            wxDataViewItem lastItem = m_listctrl->GetStore()->GetItem(count - 1);
            m_listctrl->EnsureVisible(lastItem);
        }
    }
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
                if(bli) {
                    int line = bli->GetLineInBuildTab();
                    if(IS_VALID_LINE(line)) {
                        // scroll to line of the build tab
                        wxDataViewItem item = m_listctrl->GetStore()->GetItem(line);
                        if(item.IsOk()) {
                            m_listctrl->EnsureVisible(item);
                            m_listctrl->Select(item);
                        }
                    }
                }
            }
        }

    } else if(m_autoHide && viewing && !m_buildInterrupted) {
        ManagerST::Get()->HidePane(clMainFrame::Get()->GetOutputPane()->GetCaption());

    } else if(m_showMe == BuildTabSettingsData::ShowOnEnd && !m_autoHide) {
        ManagerST::Get()->ShowOutputPane(OutputPane::BUILD_WIN);
    }
}

void NewBuildTab::OnNextBuildError(wxCommandEvent& e)
{
    // if we are here, we have at least something in the list of errors
    if(m_errorsAndWarningsList.empty())
        return;

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
                    wxDataViewItem item = m_listctrl->GetStore()->GetItem(line);
                    DoSelectAndOpen(item);
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
            wxDataViewItem item = m_listctrl->GetStore()->GetItem(line);
            DoSelectAndOpen(item);
            ++m_curError;
        }
    }
}

void NewBuildTab::OnNextBuildErrorUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_errorsAndWarningsList.empty() && !m_buildInProgress);
}

bool NewBuildTab::DoSelectAndOpen(const wxDataViewItem& item)
{
    if(item.IsOk() == false)
        return false;

    m_listctrl->UnselectAll(); // Clear any selection
    m_listctrl->EnsureVisible(item);
    m_listctrl->Select(item);

    BuildLineInfo* bli = (BuildLineInfo*)m_listctrl->GetItemData(item);
    if(bli) {
        wxFileName fn(bli->GetFilename());

        if(!fn.IsAbsolute()) {
            std::vector<wxFileName> files;
            std::vector<wxFileName> candidates;
            ManagerST::Get()->GetWorkspaceFiles(files, true);

            for(size_t i = 0; i < files.size(); ++i) {
                if(files.at(i).GetFullName() == fn.GetFullName()) {
                    candidates.push_back(files.at(i));
                }
            }

            if(candidates.empty())
                return false;

            if(candidates.size() == 1)
                fn = candidates.at(0);

            else {
                // prompt the user
                wxArrayString fileArr;
                for(size_t i = 0; i < candidates.size(); ++i) {
                    fileArr.Add(candidates.at(i).GetFullPath());
                }

                wxString selection = wxGetSingleChoice(_("Select a file to open:"), _("Choose a file"), fileArr);
                if(selection.IsEmpty())
                    return false;

                fn = wxFileName(selection);
                // if we resolved it now, open the file there is no point in searching this file
                // in m_buildInfoPerFile since the key on this map is kept as full name
                LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
                if(!editor) {
                    editor = clMainFrame::Get()->GetMainBook()->OpenFile(
                        fn.GetFullPath(), wxT(""), bli->GetLineNumber(), wxNOT_FOUND, OF_AddJump);
                }

                if(editor) {
                    // We already got compiler markers set here, just goto the line
                    clMainFrame::Get()->GetMainBook()->SelectPage(editor);
                    editor->GotoLine(bli->GetLineNumber());
                    SetActive(editor);
                    return true;
                }
            }
        }

        if(fn.IsAbsolute()) {

            // try to locate the editor first
            LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
            if(!editor) {
                // Open it
                editor = clMainFrame::Get()->GetMainBook()->OpenFile(
                    bli->GetFilename(), wxT(""), bli->GetLineNumber(), wxNOT_FOUND, OF_AddJump);
            }

            if(editor) {
                if(!editor->HasCompilerMarkers())
                    MarkEditor(editor);

                int lineNumber = bli->GetLineNumber();
                if(lineNumber > 0) {
                    lineNumber--;
                }

                // We already got compiler markers set here, just goto the line
                clMainFrame::Get()->GetMainBook()->SelectPage(editor);
                editor->GotoLine(bli->GetLineNumber());
                editor->ScrollToLine(bli->GetLineNumber());
                editor->EnsureVisible(lineNumber);
                editor->EnsureCaretVisible();
                SetActive(editor);
                return true;
            }
        }
    }
    return false;
}

wxString NewBuildTab::GetBuildContent() const
{
    wxString output;
    for(int i = 0; i < m_listctrl->GetItemCount(); ++i) {
        wxString curline = m_listctrl->GetTextValue(i, 0);
        StripBuildMarkders(curline);
        curline.Trim();
        output << curline << wxT("\n");
    }
    return output;
}

wxFont NewBuildTab::DoGetFont() const
{
    wxFont font = wxNullFont;
    LexerConf::Ptr_t lexerConf = EditorConfigST::Get()->GetLexer("C++");
    if(lexerConf) {
        font = lexerConf->GetFontForSyle(wxSTC_C_DEFAULT);
    }

    if(font.IsOk() == false) {
        font = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    }
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
    m_listctrl->PopupMenu(&menu);
}

void NewBuildTab::OnCopy(wxCommandEvent& e)
{
    wxString content = this->GetBuildContent();
    ::CopyToClipboard(content);
}

void NewBuildTab::OnCopyUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && m_listctrl->GetItemCount()); }

void NewBuildTab::OnOpenInEditor(wxCommandEvent& e)
{
    wxString content = this->GetBuildContent();
    LEditor* editor = clMainFrame::Get()->GetMainBook()->NewEditor();
    if(editor) {
        editor->SetText(content);
    }
}

void NewBuildTab::OnOpenInEditorUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && m_listctrl->GetItemCount()); }

void NewBuildTab::OnClear(wxCommandEvent& e) { Clear(); }

void NewBuildTab::OnClearUI(wxUpdateUIEvent& e) { e.Enable(!m_buildInProgress && !IsEmpty()); }

void NewBuildTab::OnCopySelection(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_listctrl->GetSelections(items);
    if(items.IsEmpty())
        return;
    wxString text;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxString line;
        line << m_listctrl->GetTextValue(m_listctrl->GetStore()->GetRow(items.Item(i)), 0).Trim().Trim(false);
        StripBuildMarkders(line);
        text << line << "\n";
    }

    text.Trim();
    ::CopyToClipboard(text);
}

void NewBuildTab::OnCopySelectionUI(wxUpdateUIEvent& e)
{
    wxDataViewItemArray items;
    m_listctrl->GetSelections(items);
    e.Enable(!m_buildInProgress && !items.IsEmpty());
}

void NewBuildTab::ScrollToBottom()
{
    if(m_listctrl->GetItemCount()) {
        int lastLine = m_listctrl->GetItemCount() - 1;
        wxDataViewItem item = m_listctrl->RowToItem(lastLine);
        if(item.IsOk()) {
            m_listctrl->EnsureVisible(item);
        }
    }
}

void NewBuildTab::AppendLine(const wxString& text)
{
    m_output << text;
    DoProcessOutput(false, false);
}

////////////////////////////////////////////
// CmpPatter

bool CmpPattern::Matches(const wxString& line, BuildLineInfo& lineInfo)
{
    long fidx, lidx;
    if(!m_fileIndex.ToLong(&fidx) || !m_lineIndex.ToLong(&lidx))
        return false;

    if(!m_regex || !m_regex->IsValid())
        return false;

    if(!m_regex->Matches(line))
        return false;

    lineInfo.SetSeverity(m_severity);
    if(m_regex->GetMatchCount() > (size_t)fidx) {
        lineInfo.SetFilename(m_regex->GetMatch(line, fidx));
    }

    // keep the match length
    lineInfo.SetRegexLineMatch(m_regex->GetMatch(line, 0).length());

    if(m_regex->GetMatchCount() > (size_t)lidx) {
        long lineNumber;
        wxString strLine = m_regex->GetMatch(line, lidx);
        strLine.ToLong(&lineNumber);
        lineInfo.SetLineNumber(--lineNumber);
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
