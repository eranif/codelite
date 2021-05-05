//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2010 by Eran Ifrah
// file name            : tabgroupmanager.h
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

#ifndef TABGROUP_MANAGER_H
#define TABGROUP_MANAGER_H

#include "clWorkspaceEvent.hpp"
#include "singleton.h"
#include <utility>
#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/string.h>

/**
 * Each pair consists of the tabgroup name, and an array of the names of the constituent tabs
 */
typedef std::pair<wxString, wxArrayString> spTabGrp;
typedef std::vector<spTabGrp> vTabGrps;

class TabgroupManager : public wxEvtHandler
{
    friend class Singleton<TabgroupManager>;

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& evt);
    void OnWorkspaceClosed(clWorkspaceEvent& evt);

public:
    wxString GetTabgroupDirectory();
    vTabGrps& GetTabgroups(bool isGlobal = false);

    /*!
     * \brief Finds the spTabGrp that matches tabgroupname, returning its TabgroupItems in items
     * \param tabgroupname
     * \param items
     * \return true if tabgroup was found
     */
    bool FindTabgroup(bool isGlobal, const wxString& tabgroupname, wxArrayString& items);
    void LoadTabgroupData(bool isGlobal, const wxString& tabgroup);

    wxXmlNode* FindTabgroupItem(wxXmlDocument& doc, const wxString& filepath, const wxString& itemfilepath);
    bool DoAddItemToTabgroup(wxXmlDocument& doc, wxXmlNode* node, const wxString& filepath,
                             const wxString& nextitemfilepath);
    /*!
     * \brief Remove this item from the tabgroup on disc, optionally returning its data in an xml node to be stored for
     * Cut
     * \param doc
     * \param filepath
     * \param itemfilepath
     * \return
     */
    wxXmlNode* DoDeleteTabgroupItem(wxXmlDocument& doc, const wxString& filepath, const wxString& itemfilepath);

protected:
    TabgroupManager();
    ~TabgroupManager();

    void LoadKnownTabgroups(bool isGlobal = false);
    void SetTabgroupDirectory();

    wxString m_tabgroupdir;
    vTabGrps m_tabgroups;
    vTabGrps m_globalTabgroups;
};

typedef Singleton<TabgroupManager> TabGroupsManager;

#endif // TABGROUP_MANAGER_H
