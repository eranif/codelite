#include "new_build_tab.h"
#if CL_USE_NEW_BUILD_TAB

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

static const wxChar* WARNING_MARKER = wxT("@@WARNING@@");
static const wxChar* ERROR_MARKER   = wxT("@@ERROR@@");
static const wxChar* SUMMARY_MARKER = wxT("@@SUMMARY@@");

// A renderer for drawing the text
class MyTextRenderer : public wxDataViewCustomRenderer
{
    wxFont              m_font;
    wxColour            m_greyColor;
    wxDataViewListCtrl *m_listctrl;
    wxColour            m_warnFgColor;
    wxColour            m_errFgColor;
    wxVariant           m_value;
    
public:
    MyTextRenderer(wxDataViewListCtrl *listctrl) : m_listctrl(listctrl) {
        m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_font.SetFamily(wxFONTFAMILY_TELETYPE);
        m_greyColor = wxColour(wxT("LIGHT GREY"));
    }

    virtual ~MyTextRenderer() {}
    
    virtual wxSize GetSize() const {
        int xx, yy;
        wxBitmap bmp(1, 1);
        wxMemoryDC dc;
        dc.SelectObject(bmp);
        
        wxString s = m_value.GetString();
        wxFont f(m_font);
        f.SetWeight(wxFONTWEIGHT_BOLD);
        dc.GetTextExtent(s, &xx, &yy, NULL, NULL, &f);
        return wxSize(xx, yy);
    }

    virtual bool SetValue(const wxVariant& value) {
        m_value = value;
        return true;
    }
    
    virtual bool GetValue(wxVariant& value) const {
        value = m_value;
        return true;
    }
    
    void SetErrFgColor(const wxColour& errFgColor) {
        this->m_errFgColor = errFgColor;
    }
    void SetWarnFgColor(const wxColour& warnFgColor) {
        this->m_warnFgColor = warnFgColor;
    }
    const wxColour& GetErrFgColor() const {
        return m_errFgColor;
    }
    const wxColour& GetWarnFgColor() const {
        return m_warnFgColor;
    }
    virtual bool Render(wxRect cell, wxDC *dc, int state) {
        wxVariant v;
        GetValue(v);
        wxString str = v.GetString();
        
        wxFont f(m_font);
        bool isSelected = state & wxDATAVIEW_CELL_SELECTED;

        if ( str.StartsWith(ERROR_MARKER, &str) ) {
            if ( !isSelected ) {
                dc->SetTextForeground( m_errFgColor );
            }

        } else if( str.StartsWith(WARNING_MARKER, &str) ) {
            if ( !isSelected ) {
                dc->SetTextForeground( m_warnFgColor );
            }
        } else if ( str.StartsWith(SUMMARY_MARKER, &str) ) {
            f.SetWeight(wxFONTWEIGHT_BOLD);

        } else if( str.StartsWith(wxT("----")) ) {
            f.SetStyle(wxFONTSTYLE_ITALIC);
            if ( !isSelected )
                dc->SetTextForeground(m_greyColor);

        } else if(str.Contains(wxT("Entering directory")) || str.Contains(wxT("Leaving directory"))) {
            f.SetStyle(wxFONTSTYLE_ITALIC);
            if ( !isSelected )
                dc->SetTextForeground(m_greyColor);
        }
        
        dc->SetFont(f);
        dc->DrawText(str, cell.GetTopLeft());
        return true;
        
    }
};

//////////////////////////////////////////////////////////////

NewBuildTab::NewBuildTab(wxWindow* parent)
    : wxPanel(parent)
    , m_warnCount(0)
    , m_errorCount(0)
    , m_buildInterrupted(false)
    , m_autoHide (false)
    , m_showMe(BuildTabSettingsData::ShowOnStart)
    , m_skipWarnings(false)
    , m_buildpaneScrollTo(ScrollToFirstError)
    , m_buildInProgress(false)
{
    m_curError = m_errorsAndWarningsList.end();
    m_errorBmp   = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/error-strip"));
    m_warningBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/warning-strip"));
    m_successBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/success-strip"));
    m_fillerBmp  = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/filler-strip"));
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    SetSizer(bs);
    
    BuildTabTopPanel* toolbox = new BuildTabTopPanel(this);
    bs->Add(toolbox, 0, wxEXPAND);
    
    // Determine the row height
    wxBitmap tmpBmp(1, 1);
    wxMemoryDC memDc;
    memDc.SelectObject(tmpBmp);
    wxFont f = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    int xx, yy;
    memDc.GetTextExtent(wxT("Tp"), &xx, &yy, NULL, NULL, &f);
    int style = wxDV_NO_HEADER|wxDV_SINGLE;
    style |= wxDV_ROW_LINES;

    m_listctrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

    // Make sure we have enought height for the icon
    yy < 16 ? yy = 16 : yy = yy;
    m_listctrl->SetRowHeight(yy);

    bs->Add(m_listctrl, 1, wxEXPAND|wxALL);
    int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    m_textRenderer = new MyTextRenderer(m_listctrl);
    m_listctrl->AppendBitmapColumn(wxT("!"), 0, wxDATAVIEW_CELL_INERT, 16);
    m_listctrl->AppendColumn(new wxDataViewColumn(_("Message"), m_textRenderer, 1, screenWidth, wxALIGN_LEFT));

    EventNotifier::Get()->Connect ( wxEVT_SHELL_COMMAND_STARTED,         wxCommandEventHandler ( NewBuildTab::OnBuildStarted ),    NULL, this );
    EventNotifier::Get()->Connect ( wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, wxCommandEventHandler ( NewBuildTab::OnBuildStarted ),    NULL, this );
    EventNotifier::Get()->Connect ( wxEVT_SHELL_COMMAND_ADDLINE,         wxCommandEventHandler ( NewBuildTab::OnBuildAddLine ),    NULL, this );
    EventNotifier::Get()->Connect ( wxEVT_SHELL_COMMAND_PROCESS_ENDED,   wxCommandEventHandler ( NewBuildTab::OnBuildEnded ),      NULL, this );
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(NewBuildTab::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(NewBuildTab::OnWorkspaceClosed), NULL, this);
    
    wxTheApp->Connect(XRCID("next_build_error"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler ( NewBuildTab::OnNextBuildError ),   NULL, this );
    wxTheApp->Connect(XRCID("next_build_error"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler ( NewBuildTab::OnNextBuildErrorUI ), NULL, this );
    
    m_listctrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(NewBuildTab::OnLineSelected), NULL, this);
}

NewBuildTab::~NewBuildTab()
{
    EventNotifier::Get()->Disconnect( wxEVT_SHELL_COMMAND_STARTED,         wxCommandEventHandler ( NewBuildTab::OnBuildStarted ),    NULL, this );
    EventNotifier::Get()->Disconnect( wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, wxCommandEventHandler ( NewBuildTab::OnBuildStarted ),    NULL, this );
    EventNotifier::Get()->Disconnect( wxEVT_SHELL_COMMAND_ADDLINE,         wxCommandEventHandler ( NewBuildTab::OnBuildAddLine ),    NULL, this );
    EventNotifier::Get()->Disconnect( wxEVT_SHELL_COMMAND_PROCESS_ENDED,   wxCommandEventHandler ( NewBuildTab::OnBuildEnded ),      NULL, this );
    wxTheApp->Disconnect(XRCID("next_build_error"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler ( NewBuildTab::OnNextBuildError ),   NULL, this );
    wxTheApp->Disconnect(XRCID("next_build_error"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler ( NewBuildTab::OnNextBuildErrorUI ), NULL, this );
}

void NewBuildTab::OnBuildEnded(wxCommandEvent& e)
{
    e.Skip();
    m_buildInProgress = false;
    
    DoProcessOutput(true, false);

    std::vector<LEditor*> editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors);
    for(size_t i=0; i<editors.size(); i++) {
        MarkEditor( editors.at(i) );
    }

    // Add a summary line
    wxString problemcount = wxString::Format ( wxT ( "%d %s, %d %s" ), m_errorCount, _("errors"), m_warnCount, _("warnings") );
    wxString term = problemcount;
    long elapsed = m_sw.Time() / 1000;
    if ( elapsed > 10 ) {
        long sec = elapsed % 60;
        long hours = elapsed / 3600;
        long minutes = ( elapsed % 3600 ) / 60;
        term << wxString::Format ( wxT ( ", %s: %02ld:%02ld:%02ld %s" ), _("total time"), hours, minutes, sec, _("seconds") );
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
    
    // notify the plugins that the build had started
    PostCmdEvent(wxEVT_BUILD_ENDED);
}

void NewBuildTab::OnBuildStarted(wxCommandEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
    
    // Reload the build settings data
    EditorConfigST::Get()->ReadObject ( wxT ( "build_tab_settings" ), &m_buildTabSettings );
    m_textRenderer->SetErrFgColor(  m_buildTabSettings.GetErrorColour() );
    m_textRenderer->SetWarnFgColor( m_buildTabSettings.GetWarnColour() );

    m_autoHide         = m_buildTabSettings.GetAutoHide();
    m_showMe           = (BuildTabSettingsData::ShowBuildPane)m_buildTabSettings.GetShowBuildPane();
    m_skipWarnings     = m_buildTabSettings.GetSkipWarnings();
    
    DoClear();
    DoCacheRegexes();

    // Show the tab if needed
    OutputPane *opane = clMainFrame::Get()->GetOutputPane();

    wxWindow *win(NULL);
    size_t sel =  opane->GetNotebook()->GetSelection();
    if(sel != Notebook::npos)
        win = opane->GetNotebook()->GetPage(sel);

    if(m_showMe == BuildTabSettingsData::ShowOnStart) {
        ManagerST::Get()->ShowOutputPane(OutputPane::BUILD_WIN, true);

    } else if (m_showMe == BuildTabSettingsData::ShowOnEnd &&
               m_autoHide &&
               ManagerST::Get()->IsPaneVisible(opane->GetCaption()) &&
               win == this
              ) {
        // user prefers to see build/errors tabs only at end of unsuccessful build
        ManagerST::Get()->HidePane(opane->GetName());
    }
    m_sw.Start();

    // notify the plugins that the build had started
    PostCmdEvent(wxEVT_BUILD_STARTED);
}

void NewBuildTab::OnBuildAddLine(wxCommandEvent& e)
{
    e.Skip(); // Allways call skip..
    m_output << e.GetString();
    DoProcessOutput(false, false);
}

BuildLineInfo* NewBuildTab::DoProcessLine(const wxString& line, bool isSummaryLine)
{
    BuildLineInfo* buildLineInfo = new BuildLineInfo();

    if ( isSummaryLine ) {
        // Set the severity
        if( m_errorCount == 0 && m_warnCount == 0 ) {
            buildLineInfo->SetSeverity(SV_SUCCESS);

        } else if ( m_errorCount ) {
            buildLineInfo->SetSeverity(SV_ERROR);

        } else {

            buildLineInfo->SetSeverity(SV_WARNING);
        }

    } else {

        DoUpdateCurrentCompiler(line); // Usering the current line, update the active compiler based on the current project being compiled
        // Find *warnings* first
        bool isWarning = false;

        CmpPatterns cmpPatterns;
        if(!DoGetCompilerPatterns(m_cmp->GetName(), cmpPatterns)) {
            return buildLineInfo;
        }

        // If it is not an error, maybe it's a warning
        for(size_t i=0; i<cmpPatterns.warningPatterns.size(); i++) {
            CmpPatternPtr cmpPatterPtr = cmpPatterns.warningPatterns.at(i);
            BuildLineInfo bli;
            if ( cmpPatterPtr->Matches(line, bli) ) {
                buildLineInfo->SetFilename(bli.GetFilename());
                buildLineInfo->SetSeverity(bli.GetSeverity());
                buildLineInfo->SetLineNumber(bli.GetLineNumber());
                buildLineInfo->NormalizeFilename(m_directories);
                
                // keep this info in the errors+warnings list only
                m_errorsAndWarningsList.push_back(buildLineInfo);
                
                m_warnCount++;
                isWarning = true;
                break;
            }
        }
        if ( !isWarning ) {
            for(size_t i=0; i<cmpPatterns.errorsPatterns.size(); i++) {
                BuildLineInfo bli;
                CmpPatternPtr cmpPatterPtr = cmpPatterns.errorsPatterns.at(i);
                if ( cmpPatterPtr->Matches(line, bli) ) {
                    buildLineInfo->SetFilename(bli.GetFilename());
                    buildLineInfo->SetSeverity(bli.GetSeverity());
                    buildLineInfo->SetLineNumber(bli.GetLineNumber());
                    buildLineInfo->NormalizeFilename(m_directories);
                    
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

void NewBuildTab::DoUpdateCurrentCompiler(const wxString& line)
{
    wxString projectName, configuration;
    if ( line.Contains ( wxGetTranslation(BUILD_PROJECT_PREFIX) ) ) {
        // now building the next project

        wxString prj  = line.AfterFirst ( wxT ( '[' ) ).BeforeFirst ( wxT ( ']' ) );
        projectName   = prj.BeforeFirst ( wxT ( '-' ) ).Trim ( false ).Trim();
        configuration = prj.AfterFirst ( wxT ( '-' ) ).Trim ( false ).Trim();

        m_cmp.Reset ( NULL );
        // need to know the compiler in use for this project to extract
        // file/line and error/warning status from the text
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, configuration);
        if ( bldConf ) {
            m_cmp = BuildSettingsConfigST::Get()->GetCompiler ( bldConf->GetCompilerType() );

        } else {
            // probably custom build with project names incorret
            // assign the default compiler for this purpose
            if ( BuildSettingsConfigST::Get()->IsCompilerExist(wxT("gnu g++")) ) {
                m_cmp = BuildSettingsConfigST::Get()->GetCompiler( wxT("gnu g++") );
            }
        }
    }

    if( !m_cmp ) {
        if ( BuildSettingsConfigST::Get()->IsCompilerExist(wxT("gnu g++")) ) {
            m_cmp = BuildSettingsConfigST::Get()->GetCompiler( wxT("gnu g++") );
        }
    }
}

void NewBuildTab::DoCacheRegexes()
{
    m_cmpPatterns.clear();

    // Loop over all known compilers and cache the regular expressions
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while( cmp ) {
        CmpPatterns cmpPatterns;
        const Compiler::CmpListInfoPattern& errPatterns  = cmp->GetErrPatterns();
        const Compiler::CmpListInfoPattern& warnPatterns = cmp->GetWarnPatterns();
        Compiler::CmpListInfoPattern::const_iterator iter;
        for (iter = errPatterns.begin(); iter != errPatterns.end(); iter++) {

            CmpPatternPtr compiledPatternPtr(new CmpPattern(new wxRegEx(iter->pattern), iter->fileNameIndex, iter->lineNumberIndex, SV_ERROR));
            if(compiledPatternPtr->GetRegex()->IsValid()) {
                cmpPatterns.errorsPatterns.push_back( compiledPatternPtr );
            }
        }

        for (iter = warnPatterns.begin(); iter != warnPatterns.end(); iter++) {

            CmpPatternPtr compiledPatternPtr(new CmpPattern(new wxRegEx(iter->pattern), iter->fileNameIndex, iter->lineNumberIndex, SV_WARNING));
            if(compiledPatternPtr->GetRegex()->IsValid()) {
                cmpPatterns.warningPatterns.push_back( compiledPatternPtr );
            }
        }

        m_cmpPatterns.insert(std::make_pair(cmp->GetName(), cmpPatterns));
        cmp =  BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
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
    m_buildInterrupted = false;
    m_directories.Clear();
    m_buildInfoPerFile.clear();
    m_warnCount = 0;
    m_errorCount = 0;
    m_errorsAndWarningsList.clear();
    m_errorsList.clear();
    
    int count = m_listctrl->GetItemCount();
    for(int i=0; i<count; ++i) {
        wxDataViewItem item = m_listctrl->GetStore()->GetItem(i);
        BuildLineInfo* data = (BuildLineInfo*)m_listctrl->GetItemData(item);
        delete data;
    }
    m_listctrl->DeleteAllItems();

    // Clear all markers from open editors
    std::vector<LEditor*> editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors);
    for(size_t i=0; i<editors.size(); i++) {
        editors.at(i)->DelAllCompilerMarkers();
        editors.at(i)->AnnotationClearAll();
    }
}

void NewBuildTab::MarkEditor(LEditor* editor)
{
    if ( !editor )
        return;

    editor->DelAllCompilerMarkers();
    editor->AnnotationClearAll();
    editor->AnnotationSetVisible(2); // Visible with box around it

    BuildTabSettingsData options;
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

    // Are annotations enabled?
    if( options.GetErrorWarningStyle() == BuildTabSettingsData::EWS_NoMarkers ) {
        return;
    }

    std::pair<MultimapBuildInfo_t::iterator, MultimapBuildInfo_t::iterator> iter = m_buildInfoPerFile.equal_range(editor->GetFileName().GetFullPath());
    for(; iter.first != iter.second; ++iter.first) {
        BuildLineInfo *bli = iter.first->second;
        if( bli->GetSeverity() == SV_ERROR ) {
            editor->SetErrorMarker( bli->GetLineNumber() );

        } else if(bli->GetSeverity() == SV_WARNING ) {
            editor->SetWarningMarker( bli->GetLineNumber() );
        }
    }
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
    e.Skip();
    if(e.GetItem().IsOk() == false || !DoSelectAndOpen(e.GetItem()) ) {
        e.Skip();
        return;
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
    if ( !compilationEnded && m_output.Find(wxT("\n")) == wxNOT_FOUND ) {
        // still dont have a complete line
        return;
    }

    wxArrayString lines = ::wxStringTokenize(m_output, wxT("\n"), wxTOKEN_RET_DELIMS);
    m_output.Clear();

    // Process only completed lines (i.e. a line that ends with '\n')
    for(size_t i=0; i<lines.GetCount(); i++) {
        if( !compilationEnded && !lines.Item(i).EndsWith(wxT("\n")) ) {
            m_output << lines.Item(i);
            return;
        }

        wxString buildLine = lines.Item(i).Trim().Trim(false);
        // If this is a line similar to 'Entering directory `'
        // add the path in the directories array
        DoSearchForDirectory(buildLine);
        BuildLineInfo *buildLineInfo = DoProcessLine(buildLine, isSummaryLine);

        //keep the line info
        if(buildLineInfo->GetFilename().IsEmpty() == false) {
            m_buildInfoPerFile.insert(std::make_pair(buildLineInfo->GetFilename(), buildLineInfo));
        }

        // Append the line content
        wxBitmap bmp = m_fillerBmp;
        if( buildLineInfo->GetSeverity() == SV_ERROR ) {
            bmp = m_errorBmp;
            if ( !isSummaryLine ) {
                buildLine.Prepend(ERROR_MARKER);
            }
        } else if( buildLineInfo->GetSeverity() == SV_WARNING ) {
            bmp = m_warningBmp;
            if ( !isSummaryLine ) {
                buildLine.Prepend(WARNING_MARKER);
            }
        } else if ( buildLineInfo->GetSeverity() == SV_SUCCESS ) {
            bmp = m_successBmp;

        }

        if ( isSummaryLine ) {
            buildLine.Prepend(SUMMARY_MARKER);
        }

        wxVector<wxVariant> data;
        data.push_back( wxVariant(bmp) );
        data.push_back( wxVariant(buildLine) );
        m_listctrl->AppendItem(data, (wxUIntPtr)buildLineInfo);
        
        // Keep the line number in the build tab
        buildLineInfo->SetLineInBuildTab( m_listctrl->GetItemCount() - 1 );
        
        unsigned int count = m_listctrl->GetStore()->GetItemCount();
        wxDataViewItem lastItem = m_listctrl->GetStore()->GetItem(count-1);
        m_listctrl->EnsureVisible( lastItem );

    }

}

void NewBuildTab::DoToggleWindow()
{
    bool success = m_errorCount == 0 && ( m_skipWarnings || m_warnCount == 0 );
    bool viewing = ManagerST::Get()->IsPaneVisible ( wxT("Output View") ) && ( clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() == this);
    bool skipwarnings(false);

    if ( !success ) {
        if ( viewing ) {
            if (m_buildpaneScrollTo != ScrollToEnd) {
                // The user may have opted to go to the first error, the first item, or /dev/null
                skipwarnings = (m_errorCount > 0) && (m_buildpaneScrollTo == ScrollToFirstError);
                BuildLineInfo* bli = NULL;
                if( skipwarnings && !m_errorsList.empty()) {
                    bli = m_errorsList.front();
                    
                } else if ( !m_errorsAndWarningsList.empty() ) {
                    bli = m_errorsAndWarningsList.front();
                    
                }
                
                if ( bli && bli->GetLineInBuildTab() != wxNOT_FOUND ) {
                    // scroll to line of the build tab
                    wxDataViewItem item = m_listctrl->GetStore()->GetItem(bli->GetLineInBuildTab());
                    if( item.IsOk() ) {
                        m_listctrl->EnsureVisible(item);
                        m_listctrl->Select(item);
                    }
                }
            }
        }

    } else if ( m_autoHide && viewing && !m_buildInterrupted) {
        ManagerST::Get()->HidePane ( clMainFrame::Get()->GetOutputPane()->GetCaption() );

    } else if ( m_showMe == BuildTabSettingsData::ShowOnEnd && !m_autoHide ) {
        ManagerST::Get()->ShowOutputPane ( OutputPane::BUILD_WIN );

    }
}

void NewBuildTab::OnNextBuildError(wxCommandEvent& e)
{
    // if we are here, we have at least something in the list of errors
    if( m_errorsAndWarningsList.empty() )
        return;

    EditorConfigST::Get()->ReadObject ( wxT ( "build_tab_settings" ), &m_buildTabSettings );
    bool skipWarnings = m_buildTabSettings.GetSkipWarnings();

    if( m_curError == m_errorsAndWarningsList.end() ) {
        
        m_curError = m_errorsAndWarningsList.begin();
        if( m_curError == m_errorsAndWarningsList.end() )
            // nothing to point to
            return;
    }
    
    // m_curError is valid
    if ( skipWarnings ) {
        
        do {
            if ( (*m_curError)->GetSeverity() == SV_ERROR ) {
                // get the wxDataViewItem
                wxDataViewItem item = m_listctrl->GetStore()->GetItem((*m_curError)->GetLineInBuildTab());
                DoSelectAndOpen(item);
                 ++m_curError;
                return;
                
            } else {
                ++m_curError;
            }
            
        } while ( m_curError != m_errorsAndWarningsList.end() );
        
    } else {
        wxDataViewItem item = m_listctrl->GetStore()->GetItem((*m_curError)->GetLineInBuildTab());
        DoSelectAndOpen(item);
         ++m_curError;
    }
}

void NewBuildTab::OnNextBuildErrorUI(wxUpdateUIEvent& e)
{
    e.Enable( !m_errorsAndWarningsList.empty() && !m_buildInProgress );
}

bool NewBuildTab::DoSelectAndOpen(const wxDataViewItem& item)
{
    if( item.IsOk() == false )
        return false;
    
    m_listctrl->EnsureVisible(item);
    m_listctrl->Select(item);
    
    BuildLineInfo* bli = (BuildLineInfo*)m_listctrl->GetItemData(item);
    if( bli ) {
        wxFileName fn(bli->GetFilename());
        
        if ( !fn.IsAbsolute() ) {
            std::vector<wxFileName> files;
            std::vector<wxFileName> candidates;
            ManagerST::Get()->GetWorkspaceFiles(files, true);
            
            for(size_t i=0; i<files.size(); ++i) {
                if( files.at(i).GetFullName() == fn.GetFullName() ) {
                    candidates.push_back( files.at(i) );
                }
            }
            
            if ( candidates.empty() )
                return false;
            
            if ( candidates.size() == 1 )
                fn = candidates.at(0);
                
            else {
                // prompt the user
                wxArrayString fileArr;
                for(size_t i=0; i<candidates.size(); ++i) {
                    fileArr.Add( candidates.at(i).GetFullPath() );
                }
                
                wxString selection = wxGetSingleChoice(_("Select a file to open:"), _("Choose a file"), fileArr);
                if(selection.IsEmpty())
                    return false;
                    
                fn = wxFileName(selection);
                // if we resolved it now, open the file there is no point in searching this file
                // in m_buildInfoPerFile since the key on this map is kept as full name
                LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
                if ( !editor ) {
                    editor = clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxT(""), bli->GetLineNumber(), wxNOT_FOUND, OF_AddJump);
                }
                
                if ( editor ) {
                    // We already got compiler markers set here, just goto the line
                    clMainFrame::Get()->GetMainBook()->SelectPage( editor );
                    editor->GotoLine(bli->GetLineNumber());
                    editor->SetActive();
                    return true;
                }
            }
        }
        
        if ( fn.IsAbsolute() ) {
            // try to locate the editor first
            LEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
            if ( !editor ) {
                
                // Open it
                editor = clMainFrame::Get()->GetMainBook()->OpenFile(bli->GetFilename(), wxT(""), bli->GetLineNumber(), wxNOT_FOUND, OF_AddJump);
                
            } else {
                if ( editor->HasCompilerMarkers() ) {
                    // We already got compiler markers set here, just goto the line
                    clMainFrame::Get()->GetMainBook()->SelectPage( editor );
                    editor->SetActive();
                    editor->GotoLine(bli->GetLineNumber());
                    return true;
                }
            }
            
            if ( editor ) {
                MarkEditor( editor );
                return true;
            }
        }
    }
    return false;
}

wxString NewBuildTab::GetBuildContent() const
{
    wxString output;
    for(int i=0; i<m_listctrl->GetItemCount(); ++i) {
        wxString curline = m_listctrl->GetTextValue(i, 1);
        curline.StartsWith(WARNING_MARKER, &curline);
        curline.StartsWith(ERROR_MARKER,   &curline);
        curline.StartsWith(SUMMARY_MARKER, &curline);
        output << curline << wxT("\n");
    }
    return output;
}

////////////////////////////////////////////
// CmpPatter

bool CmpPattern::Matches(const wxString& line, BuildLineInfo& lineInfo)
{
    long fidx, lidx;
    if ( !m_fileIndex.ToLong ( &fidx ) || !m_lineIndex.ToLong ( &lidx ) )
        return false;

    if ( !m_regex || !m_regex->IsValid() )
        return false;

    if ( !m_regex->Matches( line ) )
        return false;

    lineInfo.SetSeverity(m_severity);
    if ( m_regex->GetMatchCount() > (size_t)fidx ) {
        lineInfo.SetFilename( m_regex->GetMatch(line, fidx) );
    }

    if ( m_regex->GetMatchCount() > (size_t)lidx ) {
        long lineNumber;
        wxString strLine = m_regex->GetMatch(line, lidx);
        strLine.ToLong(&lineNumber);
        lineInfo.SetLineNumber( --lineNumber );
    }
    return true;
}

void BuildLineInfo::NormalizeFilename(const wxArrayString& directories)
{
    wxFileName fn(this->GetFilename());
    if(fn.IsAbsolute()) {
        SetFilename( fn.GetFullPath() );
        return;
    }

    if(directories.IsEmpty()) {
        SetFilename(fn.GetFullName());
        return;
    }

    // we got a relative file name
    int dircount = directories.GetCount();
    for(int i=dircount-1; i>=0; --i) {
        wxFileName tmp = fn;
        if( tmp.MakeAbsolute(directories.Item(i)) && tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG) && tmp.FileExists() ) {
            SetFilename( tmp.GetFullPath() );
            return;
        }
    }

    // failed.. keep it as fullname only
    SetFilename(fn.GetFullName());
}

#endif // CL_USE_NEW_BUILD_TAB
