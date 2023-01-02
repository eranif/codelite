//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : copyright.h
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
#ifndef __Copyright__
#define __Copyright__

#include "plugin.h"
#include "cl_command_event.h"

class wxMenuItem;
class Copyright : public IPlugin
{
    wxMenuItem* m_projectSepItem;
    wxMenuItem* m_workspaceSepItem;

protected:
    bool Validate(wxString& content);
    void MassUpdate(const std::vector<wxFileName>& filtered_files, const wxString& content);

public:
    Copyright(IManager* manager);
    ~Copyright();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();

    // event handlers
    void OnOptions(wxCommandEvent& e);
    void OnInsertCopyrights(wxCommandEvent& e);
    void OnBatchInsertCopyrights(wxCommandEvent& e);
    void OnProjectInsertCopyrights(wxCommandEvent& e);
    void OnEditorContextMenu(clContextMenuEvent& event);
};

#endif // Copyright
