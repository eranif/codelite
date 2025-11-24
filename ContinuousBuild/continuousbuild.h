//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continuousbuild.h
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

#ifndef __ContinuousBuild__
#define __ContinuousBuild__

#include "plugin.h"
#include "buildprocess.h"
#include "compiler.h"
#include "cl_command_event.h"
#include "clTabTogglerHelper.h"

class wxEvtHandler;
class ContinuousBuildPane;
class ShellCommand;

class ContinuousBuild : public IPlugin
{
    ContinuousBuildPane* m_view;
    wxEvtHandler* m_topWin;
    BuildProcess m_buildProcess;
    wxArrayString m_files;
    bool m_buildInProgress;
    clTabTogglerHelper::Ptr_t m_tabHelper;
    
public:
    void DoBuild(const wxString& fileName);

public:
    ContinuousBuild(IManager* manager);
    ~ContinuousBuild() override = default;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void HookPopupMenu(wxMenu* menu, MenuType type) override;
    void UnPlug() override;

    void StopAll();

    void OnFileSaved(clCommandEvent& e);
    void OnIgnoreFileSaved(wxCommandEvent& e);
    void OnStopIgnoreFileSaved(wxCommandEvent& e);
    void OnBuildProcessEnded(clProcessEvent& e);
    void OnBuildProcessOutput(clProcessEvent& e);
};

#endif // ContinuousBuild
