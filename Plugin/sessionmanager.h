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
}

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
    void SetBreakpoints(const std::vector<BreakpointInfo>& breakpoints) { m_breakpoints.SetBreakpoints(breakpoints); }

    // Getters
    const int& GetSelectedTab() const { return m_selectedTab; }
    // const wxArrayString& GetTabs() const {return m_tabs;}
    const wxString& GetWorkspaceName() const { return m_workspaceName; }
    const std::vector<TabInfo>& GetTabInfoArr() { return m_vTabInfoArr; }
    const std::vector<BreakpointInfo>& GetBreakpoints() const { return m_breakpoints.GetBreakpoints(); }

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

/**
 * \class SessionManager
 * \brief
 * \author Eran
 * \date 09/25/07
 */
class WXDLLIMPEXP_SDK SessionManager
{
    wxXmlDocument m_doc;
    wxFileName m_fileName;

private:
    SessionManager();
    ~SessionManager();

public:
    static SessionManager& Get();
    bool Load(const wxString& fileName);
    bool Save(const wxString& name, SessionEntry& session, const wxString& suffix = wxT(""),
              const wxChar* Tag = sessionTag);
    bool GetSession(const wxString& workspaceFile, SessionEntry& session, const wxString& suffix = wxT(""),
                    const wxChar* Tag = sessionTag);
    void SetLastSession(const wxString& name);
    wxString GetLastSession();
    void UpdateFindInFilesMaskForCurrentWorkspace(const wxString& mask);
    wxString GetFindInFilesMaskForCurrentWorkspace();

private:
    wxFileName GetSessionFileName(const wxString& name, const wxString& suffix = wxT("")) const;
};

#endif // SESSIONMANAGER_H
