#ifndef NEWBUILDTAB_H
#define NEWBUILDTAB_H

#include "cl_defs.h"

#if CL_USE_NEW_BUILD_TAB

#include "precompiled_header.h"
#include <wx/fdrepdlg.h>
#include <wx/dataview.h>
#include <wx/stopwatch.h>
#include <wx/panel.h> // Base class: wxPanel
#include "buildtabsettingsdata.h"
#include "compiler.h"
#include <map>
#include <wx/regex.h>

class wxDataViewListCtrl;

///////////////////////////////
// Holds the information about
// a specific line
enum LINE_SEVERITY {
    SV_NONE    = 0,
    SV_WARNING = 1,
    SV_ERROR   = 2,
    SV_SUCCESS = 3
};

class BuildLineInfo
{
protected:
    wxString      m_filename;
    int           m_line_number;
    LINE_SEVERITY m_severity;
    int           m_lineInBuildTab;

public:
    BuildLineInfo() : m_line_number(-1), m_severity(SV_NONE), m_lineInBuildTab(-1) {}
    ~BuildLineInfo() {}

    /**
     * @brief try to expand the file name and normalize it into a fullpath
     */
    void NormalizeFilename(const wxArrayString &directories, const wxString &cygwinPath);

    void SetFilename(const wxString& filename) {
        this->m_filename = filename;
    }
    void SetLineNumber(int line_number) {
        this->m_line_number = line_number;
    }
    void SetSeverity(LINE_SEVERITY severity) {
        this->m_severity = severity;
    }
    const wxString& GetFilename() const {
        return m_filename;
    }
    int GetLineNumber() const {
        return m_line_number;
    }
    LINE_SEVERITY GetSeverity() const {
        return m_severity;
    }
    void SetLineInBuildTab(int lineInBuildTab) {
        this->m_lineInBuildTab = lineInBuildTab;
    }
    int GetLineInBuildTab() const {
        return m_lineInBuildTab;
    }
};

/////////////////////////////////////////////////////////////////

class CmpPattern
{
protected:
    wxRegEx*      m_regex;
    wxString      m_fileIndex;
    wxString      m_lineIndex;
    LINE_SEVERITY m_severity;

public:
    CmpPattern(wxRegEx *re, const wxString &file, const wxString &line, LINE_SEVERITY severity)
        : m_regex(re)
        , m_fileIndex(file)
        , m_lineIndex(line)
        , m_severity(severity)
    {}

    ~CmpPattern() {
        if (m_regex) {
            delete m_regex;
            m_regex = NULL;
        }
    }

    /**
     * @brief return true if "line" matches this pattern
     * @param lineInfo [output]
     */
    bool Matches(const wxString& line, BuildLineInfo& lineInfo);

    void SetFileIndex(const wxString& fileIndex) {
        this->m_fileIndex = fileIndex;
    }
    void SetLineIndex(const wxString& lineIndex) {
        this->m_lineIndex = lineIndex;
    }
    void SetRegex(wxRegEx* regex) {
        this->m_regex = regex;
    }
    void SetSeverity(LINE_SEVERITY severity) {
        this->m_severity = severity;
    }
    const wxString& GetFileIndex() const {
        return m_fileIndex;
    }
    const wxString& GetLineIndex() const {
        return m_lineIndex;
    }
    wxRegEx* GetRegex() {
        return m_regex;
    }
    LINE_SEVERITY GetSeverity() const {
        return m_severity;
    }
};
typedef SmartPtr<CmpPattern> CmpPatternPtr;

//////////////////////////////////////////////////////////////////

struct CmpPatterns {
    std::vector<CmpPatternPtr> errorsPatterns;
    std::vector<CmpPatternPtr> warningPatterns;
};

///////////////////////////////////////////////////////////////////
class MyTextRenderer;
class LEditor;
class NewBuildTab : public wxPanel
{
    enum BuildpaneScrollTo {
        ScrollToFirstError,
        ScrollToFirstItem,
        ScrollToEnd
    };

    typedef std::map<wxString, CmpPatterns>          MapCmpPatterns_t;
    typedef std::multimap<wxString, BuildLineInfo*>  MultimapBuildInfo_t;
    typedef std::list<BuildLineInfo*>                BuildInfoList_t;

    wxString                            m_output;
    wxDataViewListCtrl *                m_listctrl;
    CompilerPtr                         m_cmp;
    MapCmpPatterns_t                    m_cmpPatterns;
    int                                 m_warnCount;
    int                                 m_errorCount;
    MyTextRenderer*                     m_textRenderer;
    BuildTabSettingsData                m_buildTabSettings;
    bool                                m_buildInterrupted;
    bool                                m_autoHide;
    BuildTabSettingsData::ShowBuildPane m_showMe;
    wxStopWatch                         m_sw;
    MultimapBuildInfo_t                 m_buildInfoPerFile;
    wxArrayString                       m_directories;
    bool                                m_skipWarnings;
    BuildpaneScrollTo                   m_buildpaneScrollTo;
    BuildInfoList_t                     m_errorsAndWarningsList;
    BuildInfoList_t                     m_errorsList;
    BuildInfoList_t::iterator           m_curError;
    bool                                m_buildInProgress;
    wxString                            m_cygwinRoot;
    wxFindReplaceDialog*                m_findDlg;
    wxFindReplaceData                   m_findData;
    wxPoint                             m_findDlgPos;
protected:
    void DoCacheRegexes();
    BuildLineInfo* DoProcessLine(const wxString &line, bool isSummaryLine);
    void DoProcessOutput(bool compilationEnded, bool isSummaryLine);
    void DoSearchForDirectory(const wxString &line);
    void DoUpdateCurrentCompiler(const wxString &line);
    bool DoGetCompilerPatterns(const wxString& compilerName, CmpPatterns& patterns);
    void DoClear();
    void MarkEditor(LEditor *editor);
    void DoToggleWindow();
    bool DoSelectAndOpen(const wxDataViewItem& item);
    wxFont DoGetFont() const;
    
public:
    NewBuildTab(wxWindow* parent);
    virtual ~NewBuildTab();

    bool GetBuildEndedSuccessfully() const {
        return m_errorCount == 0 && !m_buildInterrupted;
    }
    void SetBuildInterrupted(bool b) {
        m_buildInterrupted = b;
    }

    bool IsEmpty() const {
        return m_listctrl->GetItemCount() == 0;
    }

    bool IsBuildInProgress() const {
        return m_buildInProgress;
    }
    
    void Clear() {
        DoClear();
    }
    
    wxString GetBuildContent() const;
protected:
    void OnBuildStarted(wxCommandEvent &e);
    void OnBuildEnded(wxCommandEvent &e);
    void OnBuildAddLine(wxCommandEvent &e);
    void OnLineSelected(wxDataViewEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnNextBuildError(wxCommandEvent &e);
    void OnNextBuildErrorUI(wxUpdateUIEvent &e);
    void OnMenu(wxContextMenuEvent &e);
    void OnFind(wxCommandEvent &e);
    void OnCopy(wxCommandEvent &e);
    void OnCopyUI(wxUpdateUIEvent &e);
    void OnFindUI(wxUpdateUIEvent &e);
    
    // Find Dialog
    void OnFindDlgClose(wxCommandEvent &e);
    
};

#endif // NEWBUILDTAB_H
#endif //CL_USE_NEW_BUILD_TAB
