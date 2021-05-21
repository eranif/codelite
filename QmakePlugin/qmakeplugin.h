//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : qmakeplugin.h
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

#ifndef __QMakePlugin__
#define __QMakePlugin__

#include "cl_command_event.h"
#include "plugin.h"
#include "qmakeplugindata.h"

class QMakeTab;
class QmakeConf;
class IProcess;
class QMakePlugin : public IPlugin
{
    std::map<wxString, QMakeTab*> m_pages;
    QmakeConf* m_conf;
    IProcess* m_qmakeProcess;

protected:
    QMakeTab* DoGetQmakeTab(const wxString& config);
    void DoUnHookAllTabs(wxBookCtrlBase* book);
    bool DoGetData(const wxString& project, const wxString& conf, QmakePluginData::BuildConfPluginData& bcpd);
    wxString DoGetBuildCommand(const wxString& project, const wxString& config, bool projectOnly);

public:
    QMakePlugin(IManager* manager);
    ~QMakePlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void HookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName, const wxString& configName);
    virtual void UnHookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName,
                                          const wxString& configName);
    virtual void UnPlug();

    // event handlers
    void OnSaveConfig(clProjectSettingsEvent& event);
    void OnBuildStarting(clBuildEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnGetCleanCommand(clBuildEvent& event);
    void OnGetBuildCommand(clBuildEvent& event);
    void OnGetIsPluginMakefile(clBuildEvent& event);
    void OnNewQmakeBasedProject(wxCommandEvent& event);
    void OnOpenFile(clCommandEvent& event);
    void OnExportMakefile(wxCommandEvent& event);
    void OnQmakeOutput(clProcessEvent& event);
    void OnQmakeTerminated(clProcessEvent& event);
};

#endif // QMakePlugin
