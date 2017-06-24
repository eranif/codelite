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

#include "cl_command_event.h"
#include "fileextmanager.h"
#include "formatoptions.h"
#include "plugin.h"

enum CodeFormatterEngine {
    cFormatEngineNone,
    cFormatEngineAStyle,
    cFormatEngineClangFormat,
    cFormatEngineBuildInPhp,
    cFormatEnginePhpCsFixer,
    cFormatEnginePhpcbf,
    cFormatEngineWxXmlDocument,
};

class CodeFormatter : public IPlugin
{
    FormatOptions m_options;
    PhpOptions m_optionsPhp;

protected:
    wxString m_selectedFolder;

    int DoGetGlobalEOL() const;
    wxString DoGetGlobalEOLString() const;

private:
    void DoFormatFile(const wxFileName& fileName, const int& engine);
    void DoFormatEditor(IEditor* editor);
    void DoFormatWithTempFile(const wxFileName& fileName, wxString& content, const int& engine);
    bool IsPharConfigValid(const wxFileName& phar);
    void OverwriteEditorText(IEditor*& editor, const wxString& content, const int& curpos = 0);
    int FindFormatter(const wxFileName& fileName);

    bool DoFormatWithPhpcbf(const wxFileName& fileName);
    bool DoFormatWithBuildInPhp(const wxFileName& fileName);
    bool DoFormatWithPhpCsFixer(const wxFileName& fileName);
    bool DoFormatWithClang(const wxFileName& fileName);
    bool DoFormatWithAstyle(const wxFileName& fileName);
    bool DoFormatWithWxXmlDocument(const wxFileName& fileName);

    void OnPhpSettingsChanged(clCommandEvent& event);

public:
    bool DoFormatExternally(const wxFileName& fileName, const wxString& command);

    /**
     * @brief format list of files
     */
    void BatchFormat(const std::vector<wxFileName>& files);
    void OnContextMenu(clContextMenuEvent& event);

    CodeFormatter(IManager* manager);
    virtual ~CodeFormatter();
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    IManager* GetManager();
    // event handlers
    void OnFormat(wxCommandEvent& e);
    void OnFormatOptions(wxCommandEvent& e);
    void OnFormatUI(wxUpdateUIEvent& e);
    void OnFormatOptionsUI(wxUpdateUIEvent& e);
    void DoFormatPreview(wxString& content, const wxString& ext, const int& engine);

    void OnFormatString(clSourceFormatEvent& e);
    void OnFormatFile(clSourceFormatEvent& e);
    void OnFormatFiles(wxCommandEvent& e);
    void OnFormatProject(wxCommandEvent& e);
    void OnBeforeFileSave(clCommandEvent& e);
};

#endif // CODEFORMATTER_H
