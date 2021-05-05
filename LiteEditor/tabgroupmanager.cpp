//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2010 by Eran Ifrah
// file name            : tabgroupmanager.cpp
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

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/xml/xml.h>

#include "codelite_events.h"
#include "event_notifier.h"
#include "frame.h"
#include "manager.h"
#include "tabgroupmanager.h"
#include "tabgroupspane.h"
#include "workspace_pane.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"

TabgroupManager::~TabgroupManager()
{
    m_tabgroups.clear();
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &TabgroupManager::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &TabgroupManager::OnWorkspaceClosed, this);
}

vTabGrps& TabgroupManager::GetTabgroups(bool isGlobal /*=false*/)
{
    vTabGrps& tabgrps = (isGlobal ? m_globalTabgroups : m_tabgroups);
    if(tabgrps.empty()) {
        LoadKnownTabgroups(isGlobal); // Of course there might just not *be* any, but safer to try loading
    }
    return tabgrps;
}

bool TabgroupManager::FindTabgroup(bool isGlobal, const wxString& tabgroupname, wxArrayString& items)
{
    items.Empty();
    vTabGrps& tabgrps = (isGlobal ? m_globalTabgroups : m_tabgroups);
    vTabGrps::const_iterator iter = tabgrps.begin();
    for(; iter != tabgrps.end(); ++iter) {
        if(iter->first == tabgroupname) {
            items = iter->second;
            return true;
        }
    }
    return false;
}

wxString TabgroupManager::GetTabgroupDirectory() { return m_tabgroupdir; }

void TabgroupManager::SetTabgroupDirectory()
{
    wxFileName TabgrpPath = wxFileName::DirName(clCxxWorkspaceST::Get()->GetPrivateFolder() + wxT("/tabgroups/"));
    if(!TabgrpPath.DirExists()) {
        TabgrpPath.Mkdir(0777, wxPATH_MKDIR_FULL);
    }
    m_tabgroupdir = TabgrpPath.GetPath();
}

void TabgroupManager::LoadKnownTabgroups(bool isGlobal /*=false*/)
{
    wxArrayString Tabgrpfiles;
    wxString tabgroupsDir = isGlobal ? clStandardPaths::Get().GetUserDataDir() + "/tabgroups" : GetTabgroupDirectory();

    // Load tabgroups from the tabgroup dir
    wxDir::GetAllFiles(tabgroupsDir, &Tabgrpfiles, wxT("*.tabgroup"), wxDIR_FILES);
    for(size_t n = 0; n < Tabgrpfiles.GetCount(); ++n) {
        LoadTabgroupData(isGlobal, Tabgrpfiles.Item(n));
    }
}

void TabgroupManager::LoadTabgroupData(bool isGlobal, const wxString& tabgroup)
{
    // 'tabgroup' is the filepath of the tabgroup to load
    if(tabgroup.IsEmpty()) {
        return;
    }

    // Load the data: we're only interested in the tab names here, not each CurrentLine etc
    TabGroupEntry session;
    wxString filepath = tabgroup.BeforeLast(wxT('.')); // FindSession() doesn't want the .ext here
    if(SessionManager::Get().GetSession(filepath, session, "tabgroup", tabgroupTag)) {
        wxArrayString tabnames;
        const std::vector<TabInfo>& vTabInfoArr = session.GetTabInfoArr();
        for(size_t i = 0; i < vTabInfoArr.size(); ++i) {
            const TabInfo& ti = vTabInfoArr[i];
            tabnames.Add(ti.GetFileName());
        }
        std::pair<wxString, wxArrayString> TabgroupItem(tabgroup, tabnames);
        vTabGrps& tabgrps = (isGlobal ? m_globalTabgroups : m_tabgroups);
        tabgrps.push_back(TabgroupItem);
    }
}

bool TabgroupManager::DoAddItemToTabgroup(wxXmlDocument& doc, wxXmlNode* node, const wxString& filepath,
                                          const wxString& nextitemfilepath)
{
    wxXmlNode* TabInfoArrayNode = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("TabInfoArray"));
    if(!TabInfoArrayNode) {
        return false;
    }

    // If previousnode is valid, insert the new tab after it
    wxXmlNode* previousnode = NULL;
    if(!nextitemfilepath.IsEmpty()) {
        previousnode = FindTabgroupItem(doc, filepath, nextitemfilepath);
    }
    if(previousnode) {
        return TabInfoArrayNode->InsertChildAfter(node, previousnode); // >=2.8.8 has a convenient function to do this
    } else {
        TabInfoArrayNode->AddChild(node);
    }

    return true;
}

wxXmlNode* TabgroupManager::FindTabgroupItem(wxXmlDocument& doc, const wxString& filepath, const wxString& itemfilepath)
{
    wxXmlNode* TabInfoArrayNode = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("TabInfoArray"));
    if(TabInfoArrayNode) {
        wxXmlNode* TabInfoNode = TabInfoArrayNode->GetChildren();
        while(TabInfoNode) {
            wxXmlNode* child = XmlUtils::FindFirstByTagName(TabInfoNode, wxT("wxString"));
            if(child && child->GetPropVal(wxT("Value"), wxEmptyString) == itemfilepath) {
                // Found it. Return the data in TabInfoNode
                return TabInfoNode;
            }
            TabInfoNode = TabInfoNode->GetNext();
        }
    }
    // The Find failed, so return null
    return NULL;
}

wxXmlNode* TabgroupManager::DoDeleteTabgroupItem(wxXmlDocument& doc, const wxString& filepath,
                                                 const wxString& itemfilepath)
{
    wxXmlNode* TabInfoArrayNode = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("TabInfoArray"));
    if(TabInfoArrayNode) {
        wxXmlNode* TabInfoNode = TabInfoArrayNode->GetChildren();
        while(TabInfoNode) {
            wxXmlNode* child = XmlUtils::FindFirstByTagName(TabInfoNode, wxT("wxString"));
            if(child && child->GetPropVal(wxT("Value"), wxEmptyString) == itemfilepath) {
                TabInfoArrayNode->RemoveChild(TabInfoNode);
                doc.Save(filepath);
                return TabInfoNode;
            }
            TabInfoNode = TabInfoNode->GetNext();
        }
    }

    return NULL;
}

void TabgroupManager::OnWorkspaceLoaded(clWorkspaceEvent& evt)
{
    evt.Skip();
    wxFileName fnWorkspaceFile(evt.GetString());
    fnWorkspaceFile.AppendDir(".codelite");
    fnWorkspaceFile.AppendDir("tabgroups");
    m_tabgroupdir = fnWorkspaceFile.GetPath();

    // Ensure that the folder exists
    if(!fnWorkspaceFile.DirExists()) {
        fnWorkspaceFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }

    // Load the tabs directly from here and not by a handler to ensure that the pane
    // does not try to load the items before the manager is initialised properly
    clMainFrame::Get()->GetWorkspacePane()->GetTabgroupsTab()->DisplayTabgroups(false);
}

void TabgroupManager::OnWorkspaceClosed(clWorkspaceEvent& evt)
{
    evt.Skip();
    m_tabgroupdir.Clear();
    m_tabgroups.clear();
}

TabgroupManager::TabgroupManager()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &TabgroupManager::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &TabgroupManager::OnWorkspaceClosed, this);
}
