//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : wxformbuilder.h
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

#ifndef __wxFormBuilder__
#define __wxFormBuilder__

#include "wxfbitemdlg.h"
#include "plugin.h"
#include "cl_command_event.h"
class wxMenuItem;

class wxFormBuilder : public IPlugin
{
    wxEvtHandler* m_topWin;
    wxMenuItem* m_separatorItem;
    wxMenuItem* m_openWithWxFbItem;
    wxMenuItem* m_openWithWxFbSepItem;

public:
    wxFormBuilder(IManager* manager);
    ~wxFormBuilder();

protected:

    void OnSettings(wxCommandEvent& e);
    void OnNewDialog(wxCommandEvent& e);
    void OnNewDialogWithButtons(wxCommandEvent& e);
    void OnNewFrame(wxCommandEvent& e);
    void OnNewPanel(wxCommandEvent& e);
    void OpenWithWxFb(wxCommandEvent& e);
    void OnOpenFile(clCommandEvent& e);
    void OnWxFBTerminated(clProcessEvent& e);
    void OnShowFileContextMenu(clContextMenuEvent &event);
    
    wxMenu* CreatePopupMenu();
    void DoCreateWxFormBuilderProject(const wxFBItemInfo& data);
    void DoLaunchWxFB(const wxString& file);
    wxString GetWxFBPath();

public:
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
};

#endif // wxFormBuilder
