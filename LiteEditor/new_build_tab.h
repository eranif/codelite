//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : new_build_tab.h
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

#ifndef NEWBUILDTAB_H
#define NEWBUILDTAB_H

#include "cl_defs.h"

#include "precompiled_header.h"
#include <wx/fdrepdlg.h>
#include <wx/dataview.h>
#include <wx/stopwatch.h>
#include <wx/panel.h> // Base class: wxPanel
#include "buildtabsettingsdata.h"
#include "compiler.h"
#include <map>
#include <wx/regex.h>
#include "cl_command_event.h"

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
    int           m_regexLineMatch;

public:
    BuildLineInfo() : m_line_number(-1), m_severity(SV_NONE), m_lineInBuildTab(-1), m_regexLineMatch(0) {}
    ~BuildLineInfo() {}

    /**
     * @brief try to expand the file name and normalize it into a fullpath
     */
    void NormalizeFilename(const wxArrayString &directories, const wxString &cygwinPath);

    void SetRegexLineMatch(int regexLineMatch) {
        this->m_regexLineMatch = regexLineMatch;
    }
    int GetRegexLineMatch() const {
        return m_regexLineMatch;
    }
    void SetFilename(const wxString& filename);
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

protected:
    void DoCacheRegexes();
    BuildLineInfo* DoProcessLine(const wxString &line, bool isSummaryLine);
    void DoProcessOutput(bool compilationEnded, bool isSummaryLine);
    void DoSearchForDirectory(const wxString &line);
    bool DoGetCompilerPatterns(const wxString& compilerName, CmpPatterns& patterns);
    void DoClear();
    void MarkEditor(LEditor *editor);
    void DoToggleWindow();
    bool DoSelectAndOpen(const wxDataViewItem& item);
    wxFont DoGetFont() const;

public:
    NewBuildTab(wxWindow* parent);
    virtual ~NewBuildTab();

    void ScrollToBottom();
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
    void OnBuildStarted(clCommandEvent &e);
    void OnBuildEnded(clCommandEvent &e);
    void OnBuildAddLine(clCommandEvent &e);
    void OnLineSelected(wxDataViewEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnNextBuildError(wxCommandEvent &e);
    void OnNextBuildErrorUI(wxUpdateUIEvent &e);
    void OnMenu(wxContextMenuEvent &e);
    void OnCopy(wxCommandEvent &e);
    void OnCopyUI(wxUpdateUIEvent &e);
    void OnCopySelection(wxCommandEvent &e);
    void OnCopySelectionUI(wxUpdateUIEvent &e);
    void OnOpenInEditorUI(wxUpdateUIEvent &e);
    void OnOpenInEditor(wxCommandEvent &e);
    void OnClear(wxCommandEvent &e);
    void OnClearUI(wxUpdateUIEvent &e);
};

#endif // NEWBUILDTAB_H
