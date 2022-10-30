//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : sessionmanager.h
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
#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "clFilesCollector.h"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "debugger.h"
#include "serialized_object.h"
#include "wx/xml/xml.h"

#include <vector>

namespace
{
const wxChar defaultSessionName[] = wxT("Default");
const wxChar sessionTag[] = wxT("Session");
const wxChar tabgroupTag[] = wxT("Tabgroup");
} // namespace

/**
 * \class SessionEntry
 * \brief Session entry is associated per workspace
 * \author Eran
 * \date 09/25/07
 */
class WXDLLIMPEXP_SDK SessionEntry : public SerializedObject
{
protected:
    int m_selectedTab;
    wxArrayString m_tabs;
    wxString m_workspaceName;
    std::vector<TabInfo> m_vTabInfoArr;
    BreakpointInfoArray m_breakpoints;
    wxString m_findInFilesMask;

public:
    // Setters
    void SetSelectedTab(const int& selectedTab) { this->m_selectedTab = selectedTab; }
    // void SetTabs(const wxArrayString& tabs) {this->m_tabs = tabs;}
    void SetWorkspaceName(const wxString& workspaceName) { this->m_workspaceName = workspaceName; }
    void SetTabInfoArr(const std::vector<TabInfo>& _vTabInfoArr) { m_vTabInfoArr = _vTabInfoArr; }
    void SetBreakpoints(const std::vector<clDebuggerBreakpoint>& breakpoints)
    {
        m_breakpoints.SetBreakpoints(breakpoints);
    }

    // Getters
    const int& GetSelectedTab() const { return m_selectedTab; }
    // const wxArrayString& GetTabs() const {return m_tabs;}
    const wxString& GetWorkspaceName() const { return m_workspaceName; }
    const std::vector<TabInfo>& GetTabInfoArr() { return m_vTabInfoArr; }
    const std::vector<clDebuggerBreakpoint>& GetBreakpoints() const { return m_breakpoints.GetBreakpoints(); }

    void SetFindInFilesMask(const wxString& findInFilesMask) { this->m_findInFilesMask = findInFilesMask; }
    const wxString& GetFindInFilesMask() const { return m_findInFilesMask; }

    SessionEntry();
    virtual ~SessionEntry();

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);
};

/**
 * \class TabGroupEntry
 * \brief TabGroupEntry serialises a named group of tabs
 */
class WXDLLIMPEXP_SDK TabGroupEntry : public SessionEntry
{
    wxString m_tabgroupName;

public:
    TabGroupEntry() {}
    virtual ~TabGroupEntry() {}

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);
    void SetTabgroupName(const wxString& tabgroupName) { m_tabgroupName = tabgroupName; }
    const wxString& GetTabgroupName() const { return m_tabgroupName; }
};

enum {
    wxFRD_MATCHCASE = (1 << 0),
    wxFRD_MATCHWHOLEWORD = (1 << 1),
    wxFRD_REGULAREXPRESSION = (1 << 2),
    wxFRD_SEARCHUP = (1 << 3),
    wxFRD_WRAPSEARCH = (1 << 4),
    wxFRD_SELECTIONONLY = (1 << 5),
    wxFRD_DISPLAYSCOPE = (1 << 6),
    wxFRD_SAVE_BEFORE_SEARCH = (1 << 7),
    wxFRD_SKIP_COMMENTS = (1 << 8),
    wxFRD_SKIP_STRINGS = (1 << 9),
    wxFRD_COLOUR_COMMENTS = (1 << 10),
    wxFRD_SEPARATETAB_DISPLAY = (1 << 11),
    wxFRD_ENABLE_PIPE_SUPPORT = (1 << 12),
};

struct WXDLLIMPEXP_SDK FindInFilesSession {
    wxArrayString find_what_array;
    wxString find_what;

    wxArrayString replace_with_array;
    wxString replace_with;

    wxArrayString files_array;
    wxString files = "*.cpp;*.cc;*.c;*.hpp;*.h;CMakeLists.txt;*.plist;*.rc";

    wxArrayString where_array;
    wxString where = "<Workspace Folder>";

    wxString encoding = "ISO-8859-1";
    size_t flags = wxFRD_MATCHCASE | wxFRD_MATCHWHOLEWORD | wxFRD_ENABLE_PIPE_SUPPORT;
    size_t files_scanner_flags = clFilesScanner::SF_EXCLUDE_HIDDEN_DIRS;

    /// construct from a json file
    bool From(const wxString& content);

    /// save to file
    wxString Save() const;
};

/**
 * @class SessionManager
 * @brief
 * @author Eran
 * @date 09/25/07
 */
class WXDLLIMPEXP_SDK SessionManager : public wxEvtHandler
{
    wxXmlDocument m_doc;
    wxFileName m_fileName;

private:
    SessionManager();
    ~SessionManager();

    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

public:
    static SessionManager& Get();
    bool Load(const wxString& fileName);
    bool Save(const wxString& name, SessionEntry& session, const wxString& suffix = wxT(""),
              const wxChar* Tag = sessionTag);
    bool GetSession(const wxString& workspaceFile, SessionEntry& session, const wxString& suffix = wxT(""),
                    const wxChar* Tag = sessionTag);
    void SetLastSession(const wxString& name);
    wxString GetLastSession();

    // find in files related information
    void SaveFindInFilesSession(const FindInFilesSession& session);
    bool LoadFindInFilesSession(FindInFilesSession* session);

private:
    wxFileName GetSessionFileName(const wxString& name, const wxString& suffix = wxT("")) const;
};

#endif // SESSIONMANAGER_H
