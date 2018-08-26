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

enum FormatterEngine {
    kFormatEngineNone,
    kFormatEngineAStyle,
    kFormatEngineClangFormat,
    kFormatEngineBuildInPhp,
    kFormatEnginePhpCsFixer,
    kFormatEnginePhpcbf,
    kFormatEngineWxXmlDocument,
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
    void DoFormatFile(const wxFileName& fileName, const FormatterEngine& engine);
    void DoFormatFileAsString(const wxFileName& fileName, const FormatterEngine& engine);
    void DoFormatString(wxString& content, const wxFileName& fileName, const FormatterEngine& engine, int& cursorPosition);
    void DoFormatSelection(IEditor* editor,
                           wxString& content,
                           const FormatterEngine& engine,
                           int& cursorPosition,
                           const int& selStart,
                           const int& selEnd);
    void DoFormatEditor(IEditor* editor, int selStart = wxNOT_FOUND, int selEnd = wxNOT_FOUND);
    void DoFormatWithTempFile(const wxFileName& fileName, wxString& content, const FormatterEngine& engine);
    void OverwriteEditorText(IEditor*& editor,
        const wxString& content,
        const int& cursorPosition = 0,
        const int& selStart = wxNOT_FOUND,
        const int& selEnd = wxNOT_FOUND);
    FormatterEngine FindFormatter(const wxFileName& fileName);
    bool CanFormatSelection(const FormatterEngine& engine);
    bool CanFormatString(const FormatterEngine& engine);
    bool CanFormatFile(const FormatterEngine& engine);

    void DoFormatWithPhpcbf(const wxFileName& fileName);
    void DoFormatWithBuildInPhp(wxString& content);
    void DoFormatWithPhpCsFixer(const wxFileName& fileName);
    void DoFormatWithClang(const wxFileName& fileName);
    void DoFormatWithClang(wxString& content,
        const wxFileName& fileName,
        int& cursorPosition,
        const int& selStart = wxNOT_FOUND,
        const int& selEnd = wxNOT_FOUND);
    void DoFormatWithAstyle(wxString& content, const bool& appendEOL = true);
    void DoFormatWithWxXmlDocument(const wxFileName& fileName);

    void OnPhpSettingsChanged(clCommandEvent& event);

public:
    wxString RunCommand(const wxString& command);

    /**
     * @brief format list of files
     */
    void BatchFormat(const std::vector<wxFileName>& files, bool silent = true);
    void OnContextMenu(clContextMenuEvent& event);

    CodeFormatter(IManager* manager);
    virtual ~CodeFormatter();
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    IManager* GetManager();
    // event handlers
    void OnFormat(wxCommandEvent& e);
    void OnFormatOptions(wxCommandEvent& e);
    void OnFormatUI(wxUpdateUIEvent& e);
    void OnFormatOptionsUI(wxUpdateUIEvent& e);
    void DoFormatPreview(wxString& content, const wxString& ext, const FormatterEngine& engine);

    void OnFormatString(clSourceFormatEvent& e);
    void OnFormatFile(clSourceFormatEvent& e);
    void OnFormatFiles(wxCommandEvent& e);
    void OnFormatProject(wxCommandEvent& e);
    void OnBeforeFileSave(clCommandEvent& e);
};

#endif // CODEFORMATTER_H
