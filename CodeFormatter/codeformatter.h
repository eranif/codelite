//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatter.h
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
#ifndef CODEFORMATTER_H
#define CODEFORMATTER_H

#include "CodeFormatterManager.hpp"
#include "CodeLiteRemoteHelper.hpp"
#include "cl_command_event.h"
#include "fileextmanager.h"
#include "plugin.h"

#include <memory>

class CodeFormatter : public IPlugin
{
    CodeFormatterManager m_manager;
    std::shared_ptr<CodeLiteRemoteHelper> m_remoteHelper;

protected:
    wxString m_selectedFolder;

private:
    std::shared_ptr<GenericFormatter> FindFormatter(const wxString& filepath,
                                                    const wxString& content = wxEmptyString) const;
    bool DoFormatFile(const wxString& fileName, bool is_remote_format);
    bool DoFormatString(const wxString& content, const wxString& fileName, wxString* output);
    bool DoFormatEditor(IEditor* editor);
    void OnScanFilesCompleted(const std::vector<wxString>& files);
    void ReloadCurrentEditor();
    void OnWorkspaceLoaded(clWorkspaceEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnFileSaved(clCommandEvent& e);

    void OnFormatCompleted(clSourceFormatEvent& event);
    void OnInplaceFormatCompleted(clSourceFormatEvent& event);

public:
    /**
     * @brief format list of files
     */
    void BatchFormat(const std::vector<wxString>& files, bool silent = true);
    void OnContextMenu(clContextMenuEvent& event);

    CodeFormatter(IManager* manager);
    virtual ~CodeFormatter();
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    IManager* GetManager();
    // event handlers
    void OnFormatEditor(wxCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnFormatEditorUI(wxUpdateUIEvent& e);
    void OnSettingsUI(wxUpdateUIEvent& e);

    void OnFormatString(clSourceFormatEvent& e);
    void OnFormatFile(clSourceFormatEvent& e);
    void OnFormatFiles(wxCommandEvent& e);
    void OnFormatProject(wxCommandEvent& e);
};

#endif // CODEFORMATTER_H
