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

class CodeFormatter : public IPlugin
{
    FormatOptions m_options;
    PhpOptions m_optionsPhp;

protected:
    void DoFormatFile(IEditor* editor);

    int DoGetGlobalEOL() const;
    wxString DoGetGlobalEOLString() const;

private:
    bool IsPharConfigValid(const wxFileName& phar, const wxString& name);
    bool DoClangFormat(const wxFileName& filename,
        wxString& formattedOutput,
        int& cursorPosition,
        int startOffset,
        int length,
        const wxFileName& originalFileName);

    void DoFormatWithPhpcbf(IEditor* editor);
    void DoFormatWithBuildInPhp(IEditor* editor);
    void DoFormatWithPhpCsFixer(IEditor* editor);
    void DoFormatWithClang(IEditor* editor);
    void DoFormatWithAstyle(IEditor* editor);
    void DoFormatXmlSource(IEditor* editor);
    void OverwriteEditorText(IEditor*& editor, const wxString& text, int curpos = 0);

    void OnPhpSettingsChanged(clCommandEvent& event);
    void DoFormatWithPhar(IEditor* editor, const wxFileName& phar, const wxString& name, wxString& command);

public:
    /**
     * @brief format a file using clang-foramt tool. Optioanlly, you can format a portion of
     * the file by passing startOffset and length.
     * @param filename The file to parse
     * @param formattedOutput formatted output string
     * @param cursorPosition [input/output] the initial cursor position before the parsing started. the function will
     * update it
     * to its proper location after the parsing is compeleted
     * @param startOffset start of chunk to format
     * @param length chunk length
     */
    bool ClangFormatFile(const wxFileName& filename,
        wxString& formattedOutput,
        int& cursorPosition,
        int startOffset = wxNOT_FOUND,
        int length = wxNOT_FOUND);
    /**
     * @brief same as the above, but work on a buffer instead
     */
    bool ClangFormatBuffer(const wxString& content,
        const wxFileName& filename,
        wxString& formattedOutput,
        int& cursorPosition,
        int startOffset = wxNOT_FOUND,
        int length = wxNOT_FOUND);

    /**
     * @brief same as the above, but work on a buffer instead
     */
    bool ClangPreviewFormat(const wxString& content, wxString& formattedOutput);

    /**
     * @brief format a PHP content
     */
    bool PhpFormat(wxString& content);
    bool DoFormatExternally(wxString& content, wxString command, wxString filePath = "");

    /**
     * @brief format list of files
     */
    bool BatchFormat(const std::vector<wxFileName>& files);

    /**
     * @brief batch format of files using clang-format tool
     */
    bool ClangBatchFormat(const std::vector<wxFileName>& files);

    /**
     * @brief batch format of files using astyle tool
     */
    bool AStyleBatchFOrmat(const std::vector<wxFileName>& files);

public:
    CodeFormatter(IManager* manager);
    virtual ~CodeFormatter();
    void AstyleFormat(const wxString& input, wxString& output, const wxString& options);
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

    void OnFormatString(clSourceFormatEvent& e);
    void OnFormatFile(clSourceFormatEvent& e);
    void OnFormatProject(wxCommandEvent& e);
    void OnBeforeFileSave(clCommandEvent& e);
};

#endif // CODEFORMATTER_H
