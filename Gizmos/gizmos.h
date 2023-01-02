//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : gizmos.h
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
#ifndef GIZMOS_H
#define GIZMOS_H

#include "newclassdlg.h"
#include "newwxprojectinfo.h"
#include "plugin.h"

#include <vector>

class WizardsPlugin : public IPlugin
{
    void CreateClass(NewClassInfo& info);
    std::vector<wxMenuItem*> m_vdDynItems;
    wxString m_folderpath;

protected:
    void GizmosRemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
    void DoPopupButtonMenu(wxPoint pt);
    void OnFolderContentMenu(clContextMenuEvent& event);

    /**
     * @brief read the contet of multiple files
     * @param files vector of pairs {`file-name` -> `file-content`}
     * @param path_prefix prepend this prefix to each file before reading it
     * @return false if we failed to read one or more files. In addition, both vectors size must me the same
     */
    bool BulkRead(std::vector<std::pair<wxString, wxString*>>& files,
                  const wxString& path_prefix = wxEmptyString) const;
    /**
     * @brief read the contet of multiple files to the file system
     * @param files vector of pairs {`file-name` -> `file-content`}
     * @param path_prefix prepend this prefix to each file before reading it
     * @return false if we failed to write one or more files. In addition, both vectors size must me the same
     */
    bool BulkWrite(const std::vector<std::pair<wxString, wxString>>& files,
                   const wxString& path_prefix = wxEmptyString) const;

public:
    WizardsPlugin(IManager* manager);
    ~WizardsPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    void DoCreateNewPlugin();
    void DoCreateNewClass();
    // event handlers
    virtual void OnNewPlugin(wxCommandEvent& e);
    virtual void OnNewClass(wxCommandEvent& e);
    virtual void OnNewClassUI(wxUpdateUIEvent& e);
    virtual void OnNewPluginUI(wxUpdateUIEvent& e);

    // event handlers
    virtual void OnGizmos(wxCommandEvent& e);
#if USE_AUI_TOOLBAR
    virtual void OnGizmosAUI(wxAuiToolBarEvent& e);
#endif
    virtual void OnGizmosUI(wxUpdateUIEvent& e);
};

#endif // GIZMOS_H
