//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2017 Anders Jenbo
// File name            : phplint.h
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

#ifndef __phplint__
#define __phplint__

#include "lintoptions.h"
#include "phpoptions.h"
#include "plugin.h"

class PHPLint : public IPlugin
{
    std::list<wxString> m_queue;
    wxString m_output;
    IProcess* m_process;
    LintOptions m_settings;
    PhpOptions m_settingsPhp;

private:
    void DispatchCommand(const wxString& command);
    void ProcessPhpError(const wxString& lintOutput);
    void ProcessXML(const wxString& lintOutput);
    bool IsWarning(wxXmlNode* violation, const wxString& linter);
    void MarkError(wxString& errorMessage, const wxString& strLine, IEditor*& editor, bool isWarning = false);
    void RunLint();
    void QueuePhpcsCommand(const wxString& phpPath, const wxString& file);
    void QueuePhpmdCommand(const wxString& phpPath, const wxString& file);
    void QueuePhpstanCommand(const wxString& phpPath, const wxString& file);
    void OnPhpSettingsChanged(clCommandEvent& event);

protected:
    void DoProcessQueue();
    void DoCheckFile(const wxFileName& filename);

public:
    PHPLint(IManager* manager);
    ~PHPLint();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();

    void OnCheck(wxCommandEvent& e);
    void OnLintingDone(const wxString& lintOutput);
    void OnLoadFile(clCommandEvent& e);
    void OnSaveFile(clCommandEvent& e);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnMenuCommand(wxCommandEvent& e);
    void OnMenuRunLint(wxCommandEvent& e);
};

#endif // PHPLint
