//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatter.cpp
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
#include "precompiled_header.h"
#include "globals.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "codeformatter.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h> //wxInitialize/wxUnInitialize
#include "wx/ffile.h"
#include "formatoptions.h"
#include "wx/log.h"
#include "codeformatterdlg.h"
#include "wx/menu.h"
#include "file_logger.h"
#include "asyncprocess.h"
#include "json_node.h"
#include <wx/ffile.h>
#include <wx/filename.h>
#include "file_logger.h"
#include "procutils.h"
#include "clSTCLineKeeper.h"
#include "macros.h"
#include <wx/progdlg.h>
#include "fileutils.h"

static int ID_TOOL_SOURCE_CODE_FORMATTER = ::wxNewId();

extern "C" EXPORT char* STDCALL AStyleMain(const char* pSourceIn,
                                           const char* pOptions,
                                           void(STDCALL* fpError)(int, const char*),
                                           char*(STDCALL* fpAlloc)(unsigned long));

//------------------------------------------------------------------------
// Astyle functions required by AStyleLib
// Error handler for the Artistic Style formatter
void STDCALL ASErrorHandler(int errorNumber, const char* errorMessage)
{
    wxString errStr;
    errStr << _U(errorMessage) << wxT(" (error ") << errorNumber << wxT(")");
    CL_DEBUG(errStr.c_str());
}

// Allocate memory for the Artistic Style formatter
char* STDCALL ASMemoryAlloc(unsigned long memoryNeeded)
{
    // error condition is checked after return from AStyleMain
    char* buffer = new char[memoryNeeded];
    return buffer;
}
//------------------------------------------------------------------------
static CodeFormatter* theFormatter = NULL;

// Allocate the code formatter on the heap, it will be freed by
// the application
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(theFormatter == 0) {
        theFormatter = new CodeFormatter(manager);
    }
    return theFormatter;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("CodeFormatter"));
    info.SetDescription(_("Source Code Formatter based on the open source AStyle tool"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeFormatter::CodeFormatter(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Source Code Formatter (AStyle)");
    m_shortName = wxT("CodeFormatter");

    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_STRING, clSourceFormatEventHandler(CodeFormatter::OnFormatString), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_FILE, clSourceFormatEventHandler(CodeFormatter::OnFormatFile), NULL, this);
    m_mgr->GetTheApp()->Connect(ID_TOOL_SOURCE_CODE_FORMATTER,
                                wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeFormatter::OnFormatProject),
                                NULL,
                                this);
}

CodeFormatter::~CodeFormatter()
{
    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_STRING, clSourceFormatEventHandler(CodeFormatter::OnFormatString), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_FILE, clSourceFormatEventHandler(CodeFormatter::OnFormatFile), NULL, this);
}

clToolBar* CodeFormatter::CreateToolBar(wxWindow* parent)
{
    clToolBar* tb(NULL);
    if(m_mgr->AllowToolbar()) {
        // support both toolbars icon size
        int size = m_mgr->GetToolbarIconSize();

        tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
        tb->SetToolBitmapSize(wxSize(size, size));

        BitmapLoader* bmpLoader = m_mgr->GetStdIcons();
        if(size == 24) {
            tb->AddTool(XRCID("format_source"),
                        _("Format Source"),
                        bmpLoader->LoadBitmap(wxT("toolbars/24/codeformatter/code-format")),
                        _("Format Source Code"));
            tb->AddTool(XRCID("formatter_options"),
                        _("Format Options"),
                        bmpLoader->LoadBitmap(wxT("toolbars/24/codeformatter/code-format-options")),
                        _("Source Code Formatter Options..."));
        } else {
            // 16
            tb->AddTool(XRCID("format_source"),
                        _("Format Source"),
                        bmpLoader->LoadBitmap(wxT("toolbars/16/codeformatter/code-format")),
                        _("Format Source Code"));
            tb->AddTool(XRCID("formatter_options"),
                        _("Format Options"),
                        bmpLoader->LoadBitmap(wxT("toolbars/16/codeformatter/code-format-options")),
                        _("Source Code Formatter Options..."));
        }

#if defined(__WXMAC__)
        tb->AddSeparator();
#endif
        tb->Realize();
    }

    // Connect the events to us
    m_mgr->GetTheApp()->Connect(XRCID("format_source"),
                                wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeFormatter::OnFormat),
                                NULL,
                                (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("formatter_options"),
                                wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeFormatter::OnFormatOptions),
                                NULL,
                                (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("format_source"),
                                wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(CodeFormatter::OnFormatUI),
                                NULL,
                                (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("formatter_options"),
                                wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(CodeFormatter::OnFormatOptionsUI),
                                NULL,
                                (wxEvtHandler*)this);
    return tb;
}

void CodeFormatter::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(
        menu, XRCID("format_source"), _("Format Current Source"), _("Format Current Source"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("formatter_options"), _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Source Code Formatter"), menu);
}

void CodeFormatter::OnFormat(wxCommandEvent& e)
{
    IEditor* editor(NULL);
    wxString fileToFormat = e.GetString();

    // If we got a file name in the event, use it instead of the active editor
    if(fileToFormat.IsEmpty() == false) {
        editor = m_mgr->FindEditor(fileToFormat);

    } else {
        editor = m_mgr->GetActiveEditor();
    }

    // get the editor that requires formatting
    if(!editor) return;

    // Notify about indentation about to start
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_STARTING);
    evt.SetString(editor->GetFileName().GetFullPath());
    EventNotifier::Get()->ProcessEvent(evt);

    m_mgr->SetStatusMessage(
        wxString::Format(wxT("%s: %s..."), _("Formatting"), editor->GetFileName().GetFullPath().c_str()), 0);
    DoFormatFile(editor);
    m_mgr->SetStatusMessage(_("Done"), 0);
}

void CodeFormatter::DoFormatFile(IEditor* editor)
{
    int curpos = editor->GetCurrentPosition();

    // execute the formatter
    FormatOptions fmtroptions;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);

    // We allow ClangFormat to work only when the source file is known to be
    // a C/C++ source file or JavaScript (these are the types of files that clang-format can handle properly)
    if(fmtroptions.GetEngine() == kFormatEngineClangFormat &&
       (FileExtManager::IsCxxFile(editor->GetFileName()) || FileExtManager::IsJavascriptFile(editor->GetFileName()))) {

        int from = wxNOT_FOUND, length = wxNOT_FOUND;
        wxString formattedOutput;
        if(editor->GetSelectionStart() != wxNOT_FOUND) {
            // we got a selection, only format it
            from = editor->GetSelectionStart();
            length = editor->GetSelectionEnd() - from;
            if(length <= 0) {
                from = wxNOT_FOUND;
                length = wxNOT_FOUND;
            }
        }

        // Make sure we format the editor string and _not_ the file (there might be some newly added lines
        // the could be missing ...)
        if(!ClangFormat(editor->GetSTC()->GetText(), formattedOutput, curpos, from, length)) {
            ::wxMessageBox(_("Source code formatting error!"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        clSTCLineKeeper lk(editor);
        editor->SetEditorText(formattedOutput);
        editor->SetCaretAt(curpos);

    } else {
        // AStyle
        wxString options = fmtroptions.AstyleOptionsAsString();

        // determine indentation method and amount
        bool useTabs = m_mgr->GetEditorSettings()->GetIndentUsesTabs();
        int tabWidth = m_mgr->GetEditorSettings()->GetTabWidth();
        int indentWidth = m_mgr->GetEditorSettings()->GetIndentWidth();
        options << (useTabs && tabWidth == indentWidth ? wxT(" -t") : wxT(" -s")) << indentWidth;

        wxString output;
        wxString inputString;
        bool formatSelectionOnly(editor->GetSelection().IsEmpty() == false);

        if(formatSelectionOnly) {
            // get the lines contained in the selection
            int selStart = editor->GetSelectionStart();
            int selEnd = editor->GetSelectionEnd();
            int lineNumber = editor->LineFromPos(selStart);

            selStart = editor->PosFromLine(lineNumber);
            selEnd = editor->LineEnd(editor->LineFromPos(selEnd));

            editor->SelectText(selStart, selEnd - selStart);
            inputString = editor->GetSelection();

        } else {
            inputString = editor->GetEditorText();
        }

        AstyleFormat(inputString, options, output);
        if(output.IsEmpty() == false) {

            // append new-line
            wxString eol;
            if(editor->GetEOL() == 0) { // CRLF
                eol = wxT("\r\n");
            } else if(editor->GetEOL() == 1) { // CR
                eol = wxT("\r");
            } else {
                eol = wxT("\n");
            }

            if(!formatSelectionOnly) output << eol;

            if(formatSelectionOnly) {
                // format the text (add the indentation)
                output = editor->FormatTextKeepIndent(
                    output, editor->GetSelectionStart(), Format_Text_Indent_Prev_Line | Format_Text_Save_Empty_Lines);
                editor->ReplaceSelection(output);

            } else {
                clSTCLineKeeper lk(editor);
                editor->SetEditorText(output);
                editor->SetCaretAt(curpos);
            }
        }
    }

    // Notify that a file was indented
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_COMPLETED);
    evt.SetString(editor->GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(evt);
}

void CodeFormatter::AstyleFormat(const wxString& input, const wxString& options, wxString& output)
{
    char* textOut = AStyleMain(_C(input), _C(options), ASErrorHandler, ASMemoryAlloc);
    if(textOut) {
        output = _U(textOut);
        output.Trim();
        delete[] textOut;
    }
}

void CodeFormatter::OnFormatOptions(wxCommandEvent& e)
{
    wxUnusedVar(e);
    // load options from settings file
    FormatOptions fmtroptions;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);

    wxString sampleFile;
    wxString content;
    sampleFile << m_mgr->GetStartupDirectory() << wxT("/astyle.sample");
    ReadFileWithConversion(sampleFile, content);

    CodeFormatterDlg dlg(NULL, m_mgr, this, fmtroptions, content);
    dlg.ShowModal();
}

void CodeFormatter::OnFormatUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->GetActiveEditor() != NULL);
}

void CodeFormatter::OnFormatOptionsUI(wxUpdateUIEvent& e) { e.Enable(true); }

void CodeFormatter::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(type);
    wxUnusedVar(menu);
    if(type == MenuTypeFileView_Project) {
        // Project context menu
        menu->Prepend(ID_TOOL_SOURCE_CODE_FORMATTER, _("Source Code Formatter"));
    }
}

void CodeFormatter::UnPlug()
{
    m_mgr->GetTheApp()->Disconnect(XRCID("format_source"),
                                   wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CodeFormatter::OnFormat),
                                   NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("formatter_options"),
                                   wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CodeFormatter::OnFormatOptions),
                                   NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("format_source"),
                                   wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(CodeFormatter::OnFormatUI),
                                   NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("formatter_options"),
                                   wxEVT_UPDATE_UI,
                                   wxUpdateUIEventHandler(CodeFormatter::OnFormatOptionsUI),
                                   NULL,
                                   (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(ID_TOOL_SOURCE_CODE_FORMATTER,
                                   wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(CodeFormatter::OnFormatProject),
                                   NULL,
                                   this);
}

IManager* CodeFormatter::GetManager() { return m_mgr; }

void CodeFormatter::OnFormatString(clSourceFormatEvent& e)
{
    wxString str = e.GetInputString();
    if(str.IsEmpty()) {
        e.SetFormattedString(str);
        return;
    }

    // execute the formatter
    FormatOptions fmtroptions;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);

    wxString output;
    if(fmtroptions.GetEngine() == kFormatEngineAStyle) {
        wxString options = fmtroptions.AstyleOptionsAsString();

        // determine indentation method and amount
        bool useTabs = m_mgr->GetEditorSettings()->GetIndentUsesTabs();
        int tabWidth = m_mgr->GetEditorSettings()->GetTabWidth();
        int indentWidth = m_mgr->GetEditorSettings()->GetIndentWidth();
        options << (useTabs && tabWidth == indentWidth ? wxT(" -t") : wxT(" -s")) << indentWidth;

        AstyleFormat(str, options, output);
        output << DoGetGlobalEOLString();

    } else if(fmtroptions.GetEngine() == kFormatEngineClangFormat) {
        ClangPreviewFormat(str, output, fmtroptions);

    } else {
        // ??
    }
    e.SetFormattedString(output);
}

int CodeFormatter::DoGetGlobalEOL() const
{
    OptionsConfigPtr options = m_mgr->GetEditorSettings();
    if(options->GetEolMode() == wxT("Unix (LF)")) {
        return 2;
    } else if(options->GetEolMode() == wxT("Mac (CR)")) {
        return 1;
    } else if(options->GetEolMode() == wxT("Windows (CRLF)")) {
        return 0;
    } else {
// set the EOL by the hosting OS
#if defined(__WXMAC__)
        return 2;
#elif defined(__WXGTK__)
        return 2;
#else
        return 0;
#endif
    }
}
wxString CodeFormatter::DoGetGlobalEOLString() const
{
    switch(DoGetGlobalEOL()) {
    case 0:
        return wxT("\r\n");
    case 1:
        return wxT("\r");
    case 2:
    default:
        return wxT("\n");
    }
}

void CodeFormatter::OnFormatFile(clSourceFormatEvent& e) { wxUnusedVar(e); }

bool CodeFormatter::ClangFormat(const wxString& content,
                                wxString& formattedOutput,
                                int& cursorPosition,
                                int startOffset,
                                int length)
{
    // Write the content into a temporary file
    wxFileName fn(wxStandardPaths::Get().GetTempDir(), "code-formatter-tmp.cpp");
    wxFFile fp(fn.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(content, wxConvUTF8);
        fp.Close();
    }

    FormatOptions options;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &options);
    bool res = DoClangFormat(fn, formattedOutput, cursorPosition, startOffset, length, options);
    {
        // Delete the temporary file
        wxLogNull nl;
        ::wxRemoveFile(fn.GetFullPath());
    }
    return res;
}

bool CodeFormatter::ClangFormat(const wxFileName& filename,
                                wxString& formattedOutput,
                                int& cursorPosition,
                                int startOffset,
                                int length)
{
    FormatOptions options;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &options);
    return DoClangFormat(filename, formattedOutput, cursorPosition, startOffset, length, options);
}

bool CodeFormatter::ClangPreviewFormat(const wxString& content, wxString& formattedOutput, const FormatOptions& options)
{
    int startOffset, length, cursorPosition;
    startOffset = length = cursorPosition = wxNOT_FOUND;

    wxFileName fn(wxStandardPaths::Get().GetTempDir(), "code-formatter-tmp.cpp");
    wxFFile fp(fn.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(content, wxConvUTF8);
        fp.Close();
    }
    bool res = DoClangFormat(fn, formattedOutput, cursorPosition, startOffset, length, options);
    {
        // Delete the temporary file
        wxLogNull nl;
        ::wxRemoveFile(fn.GetFullPath());
    }
    return res;
}

bool CodeFormatter::DoClangFormat(const wxFileName& filename,
                                  wxString& formattedOutput,
                                  int& cursorPosition,
                                  int startOffset,
                                  int length,
                                  const FormatOptions& options)
{
    // clang-format
    // Build the command line to run

    if(options.GetClangFormatExe().IsEmpty()) {
        return false;
    }

    wxString command, file;

    command << options.GetClangFormatExe();
    file = filename.GetFullPath();
    ::WrapWithQuotes(command);
    ::WrapWithQuotes(file);

    command << options.ClangFormatOptionsAsString();
    if(cursorPosition != wxNOT_FOUND) {
        command << " -cursor=" << cursorPosition;
    }

    if(startOffset != wxNOT_FOUND && length != wxNOT_FOUND) {
        command << " -offset=" << startOffset << " -length=" << length;
    }
    command << " " << file;

    // Wrap the command in the local shell
    ::WrapInShell(command);

    // Log the command
    CL_DEBUG("CodeForamtter: running:\n%s\n", command);

    // Execute clang-format and reand the output
    formattedOutput.Clear();
    IProcess::Ptr_t clangFormatProc(
        ::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    CHECK_PTR_RET_FALSE(clangFormatProc);
    clangFormatProc->WaitForTerminate(formattedOutput);
    CL_DEBUG("clang-format returned with:\n%s\n", formattedOutput);

    if(formattedOutput.IsEmpty()) {
        // crash?
        return false;
    }

    // The first line contains the cursor position
    if(cursorPosition != wxNOT_FOUND) {
        wxString metadata = formattedOutput.BeforeFirst('\n');
        JSONRoot root(metadata);
        cursorPosition = root.toElement().namedObject("cursor").toInt(wxNOT_FOUND);
        formattedOutput = formattedOutput.AfterFirst('\n');
    }
    return true;
}

void CodeFormatter::OnFormatProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    TreeItemInfo selectedItem = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(selectedItem.m_itemType != ProjectItem::TypeProject) {
        return;
    }

    ProjectPtr pProj = WorkspaceST::Get()->GetProject(selectedItem.m_text);
    CHECK_PTR_RET(pProj);

    Project::FileInfoVector_t allFiles;
    pProj->GetFilesMetadata(allFiles);

    std::vector<wxFileName> filesToFormat;
    // Filter non C++/JavaScript files
    for(size_t i = 0; i < allFiles.size(); ++i) {
        wxFileName fn(allFiles.at(i).GetFilename());
        if(fn.GetFullName() == "sqlite3.c") {
            // skip this famous a quite large file ...
            continue;
        }

        const wxString& filename = allFiles.at(i).GetFilename();
        if(FileExtManager::IsCxxFile(filename) || FileExtManager::IsJavascriptFile(filename)) {
            filesToFormat.push_back(allFiles.at(i).GetFilename());
        }
    }

    if(filesToFormat.empty()) {
        ::wxMessageBox(_("Nothing to be done here"));
        return;
    }

    wxString msg;
    msg << _("You are about to beautify ") << filesToFormat.size() << _(" files\nContinue?");
    if(wxYES != ::wxMessageBox(msg, _("Source Code Formatter"), wxYES_NO | wxCANCEL | wxCENTER)) {
        return;
    }

    // Format the files
    BatchFormat(filesToFormat);
}

bool CodeFormatter::BatchFormat(const std::vector<wxFileName>& files)
{
    FormatOptions options;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &options);

    if(options.GetEngine() == kFormatEngineAStyle) {
        AStyleBatchFOrmat(files, options);

    } else if(options.GetEngine() == kFormatEngineClangFormat) {
        ClangBatchFormat(files, options);
    }
}

bool CodeFormatter::ClangBatchFormat(const std::vector<wxFileName>& files, const FormatOptions& options)
{
    if(options.GetClangFormatExe().IsEmpty()) {
        return false;
    }

    wxProgressDialog dlg(
        _("Source Code Formatter"), _("Formatting files..."), (int)files.size(), m_mgr->GetTheApp()->GetTopWindow());

    for(size_t i = 0; i < files.size(); ++i) {
        wxString command, file;
        command << options.GetClangFormatExe();
        ::WrapWithQuotes(command);

        command << " -i "; // inline editing
        command << options.ClangFormatOptionsAsString();
        file = files.at(i).GetFullPath();
        ::WrapWithQuotes(file);
        command << " " << file;

        // Wrap the command in the local shell
        ::WrapInShell(command);

        // Log the command
        CL_DEBUG("CodeForamtter: running:\n%s\n", command);

        wxString msg;
        msg << "[ " << i << " / " << files.size() << " ] " << files.at(i).GetFullName();
        dlg.Update(i, msg);

        // Execute clang-format and reand the output
        IProcess::Ptr_t clangFormatProc(
            ::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
        CHECK_PTR_RET_FALSE(clangFormatProc);

        wxString output;
        clangFormatProc->WaitForTerminate(output);
        CL_DEBUG("clang-format returned with:\n%s\n", output);
    }

    return true;
}

bool CodeFormatter::AStyleBatchFOrmat(const std::vector<wxFileName>& files, const FormatOptions& options)
{
    wxString fmtOptions = options.AstyleOptionsAsString();

    wxProgressDialog dlg(
        _("Source Code Formatter"), _("Formatting files..."), (int)files.size(), m_mgr->GetTheApp()->GetTopWindow());
    for(size_t i = 0; i < files.size(); ++i) {

        wxString content;
        if(!FileUtils::ReadFileContent(files.at(i), content)) {
            CL_WARNING("Failed to read file content. File: %s", files.at(i).GetFullPath());
            continue;
        }

        wxString msg;
        msg << "[ " << i << " / " << files.size() << " ] " << files.at(i).GetFullName();
        dlg.Update(i, msg);

        // determine indentation method and amount
        bool useTabs = m_mgr->GetEditorSettings()->GetIndentUsesTabs();
        int tabWidth = m_mgr->GetEditorSettings()->GetTabWidth();
        int indentWidth = m_mgr->GetEditorSettings()->GetIndentWidth();
        fmtOptions << (useTabs && tabWidth == indentWidth ? wxT(" -t") : wxT(" -s")) << indentWidth;

        wxString output;
        AstyleFormat(content, fmtOptions, output);
        output << DoGetGlobalEOLString();

        // Replace the content of the file
        if(!FileUtils::WriteFileContent(files.at(i), output)) {
            CL_WARNING("Failed to write file content. File: %s", files.at(i).GetFullPath());
        }
    }
}
