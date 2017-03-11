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
#include "clClangFormatLocator.h"
#include "clEditorStateLocker.h"
#include <wx/sstream.h>
#include "clEditorConfig.h"

static int ID_TOOL_SOURCE_CODE_FORMATTER = ::wxNewId();

extern "C" char* STDCALL AStyleMain(const char* pSourceIn, const char* pOptions,
    void(STDCALL* fpError)(int, const char*), char*(STDCALL* fpAlloc)(unsigned long));

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
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(theFormatter == 0) {
        theFormatter = new CodeFormatter(manager);
    }
    return theFormatter;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Source Code Formatter"));
    info.SetDescription(_("Source Code Formatter (Supports C/C++/Obj-C/JavaScript/PHP files)"));
    info.SetVersion(wxT("v2.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeFormatter::CodeFormatter(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Source Code Formatter");
    m_shortName = _("Source Code Formatter");

    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_STRING, clSourceFormatEventHandler(CodeFormatter::OnFormatString), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_FILE, clSourceFormatEventHandler(CodeFormatter::OnFormatFile), NULL, this);
    m_mgr->GetTheApp()->Connect(ID_TOOL_SOURCE_CODE_FORMATTER, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormatProject), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_BEFORE_EDITOR_SAVE, clCommandEventHandler(CodeFormatter::OnBeforeFileSave), this);

    // Migrate settings if needed
    FormatOptions fmtroptions;
    m_mgr->GetConfigTool()->ReadObject("FormatterOptions", &fmtroptions);
    if(fmtroptions.GetEngine() == kFormatEngineClangFormat) {
        // check to see that the selected clang executable exists
        wxFileName clangFomatExe(fmtroptions.GetClangFormatExe());
        if(fmtroptions.GetClangFormatExe().IsEmpty() || !clangFomatExe.Exists()) {
            // No valid clang executable found, try to locate one
            clClangFormatLocator locator;
            wxString clangFormatPath;
            if(locator.Locate(clangFormatPath)) {
                fmtroptions.SetClangFormatExe(clangFormatPath);
            } else {
                // Change the active engine to AStyle
                fmtroptions.SetEngine(kFormatEngineAStyle);
                fmtroptions.SetClangFormatExe(""); // Clear the non existed executable
            }
        }
    }
    // Save the options
    EditorConfigST::Get()->WriteObject("FormatterOptions", &fmtroptions);
}

CodeFormatter::~CodeFormatter() {}

clToolBar* CodeFormatter::CreateToolBar(wxWindow* parent)
{
    clToolBar* tb(NULL);
    if(m_mgr->AllowToolbar()) {
        // support both toolbars icon size
        int size = m_mgr->GetToolbarIconSize();

        tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE_PLUGIN);
        tb->SetToolBitmapSize(wxSize(size, size));

        BitmapLoader* bmpLoader = m_mgr->GetStdIcons();
        tb->AddTool(
            XRCID("format_source"), _("Format Source"), bmpLoader->LoadBitmap("format", size), _("Format Source Code"));
        tb->AddTool(XRCID("formatter_options"), _("Format Options"), bmpLoader->LoadBitmap("cog", size),
            _("Source Code Formatter Options..."));
        tb->Realize();
    }

    // Connect the events to us
    m_mgr->GetTheApp()->Connect(XRCID("format_source"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormat), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("formatter_options"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormatOptions), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("format_source"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(CodeFormatter::OnFormatUI), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("formatter_options"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(CodeFormatter::OnFormatOptionsUI), NULL, (wxEvtHandler*)this);
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

    clDEBUG() << "Formatting file: '" << editor->GetFileName() << "'" << clEndl;

    // Notify about indentation about to start
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_STARTING);
    evt.SetString(editor->GetFileName().GetFullPath());
    EventNotifier::Get()->ProcessEvent(evt);

    m_mgr->SetStatusMessage(
        wxString::Format(wxT("%s: %s..."), _("Formatting"), editor->GetFileName().GetFullPath().c_str()), 0);
    DoFormatFile(editor);
    m_mgr->SetStatusMessage(_("Done"), 0);

    clDEBUG() << "Formatting file: '" << editor->GetFileName() << "'...is done" << clEndl;
}

void CodeFormatter::DoFormatFile(IEditor* editor)
{
    int curpos = editor->GetCurrentPosition();

    // execute the formatter
    FormatOptions fmtroptions;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);
    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        clDEBUG() << "Using PHP formatter" << clEndl;
        if(fmtroptions.GetPhpEngine() == kPhpFormatEngineBuiltin) {

            // use the built-in PHP formatter

            // Construct the formatting options
            PHPFormatterOptions phpOptions;
            phpOptions.flags = fmtroptions.GetPHPFormatterOptions();
            if(m_mgr->GetEditorSettings()->GetIndentUsesTabs()) {
                phpOptions.flags |= kPFF_UseTabs;
            }
            phpOptions.indentSize = m_mgr->GetEditorSettings()->GetTabWidth();
            phpOptions.eol = m_mgr->GetEditorSettings()->GetEOLAsString();
            // Create the formatter buffer
            PHPFormatterBuffer buffer(editor->GetCtrl()->GetText(), phpOptions);

            // Format the source
            buffer.format();

            // Restore line
            if(!buffer.GetBuffer().IsEmpty()) {
                clSTCLineKeeper lk(editor);
                editor->GetCtrl()->BeginUndoAction();
                // Replace the text with the new formatted buffer
                editor->SetEditorText(buffer.GetBuffer());
                editor->GetCtrl()->EndUndoAction();
            }
        } else {
            wxFileName php(fmtroptions.GetPhpExecutable());
            if(!php.Exists()) {
                ::wxMessageBox(_("Can not format file using PHP-CS-Fixer: Missing PHP executable path"),
                    "Code Formatter", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }
            wxFileName phar(fmtroptions.GetPHPCSFixerPhar());
            if(!phar.Exists()) {
                ::wxMessageBox(_("Can not format file using PHP-CS-Fixer: Missing PHAR file"), "Code Formatter",
                    wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            // Run the command, PHP-CS-Fixer works directly on the file
            // so create a copy of the file and format it, then replace the buffers
            // we do this like this so we won't lose our ability to undo the action
            wxString output;
            wxString command, filename, tmpfile;
            filename = editor->GetFileName().GetFullPath();
            tmpfile << filename << ".php-cs-fixer";
            if(!FileUtils::WriteFileContent(tmpfile, editor->GetEditorText())) {
                ::wxMessageBox(_("Can not format file using PHP-CS-Fixer:\nFailed to write temporary file"),
                    "Code Formatter", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            // Ensure that the temporary file is deleted once we are done with it
            FileUtils::Deleter fd(tmpfile);

            ::WrapWithQuotes(tmpfile);
            command << fmtroptions.GetPhpFixerCommand() << " " << tmpfile;
            ::WrapInShell(command);
            IProcess::Ptr_t phpFixer(
                ::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
            CHECK_PTR_RET(phpFixer);
            phpFixer->WaitForTerminate(output);

            output.clear();
            if(!FileUtils::ReadFileContent(tmpfile, output)) {
                ::wxMessageBox(_("Can not format file using PHP-CS-Fixer:\nfailed to read temporary file content"),
                    "Code Formatter", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            // Update the editor
            clEditorStateLocker lk(editor->GetCtrl());
            editor->GetCtrl()->BeginUndoAction();
            editor->SetEditorText(output);
            editor->GetCtrl()->EndUndoAction();
        }

    } else if(FileExtManager::IsFileType(editor->GetFileName(), FileExtManager::TypeXml) ||
        FileExtManager::IsFileType(editor->GetFileName(), FileExtManager::TypeXRC) ||
        FileExtManager::IsFileType(editor->GetFileName(), FileExtManager::TypeWorkspace) ||
        FileExtManager::IsFileType(editor->GetFileName(), FileExtManager::TypeProject)) {
        DoFormatXmlSource(editor);
    } else {
        // We allow ClangFormat to work only when the source file is known to be
        // a C/C++ source file or JavaScript (these are the types of files that clang-format can handle properly)
        if(fmtroptions.GetEngine() == kFormatEngineClangFormat &&
            (FileExtManager::IsCxxFile(editor->GetFileName()) ||
                FileExtManager::IsJavascriptFile(editor->GetFileName()))) {

            clDEBUG() << "Using C++/Clang formatter" << clEndl;
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
            // that could be missing ...)
            if(!ClangFormatBuffer(
                   editor->GetCtrl()->GetText(), editor->GetFileName(), formattedOutput, curpos, from, length)) {
                ::wxMessageBox(_("Source code formatting error!"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            clEditorStateLocker lk(editor->GetCtrl());
            editor->GetCtrl()->BeginUndoAction();
            editor->SetEditorText(formattedOutput);
            editor->SetCaretAt(curpos);
            editor->GetCtrl()->EndUndoAction();

        } else {
            if(!FileExtManager::IsCxxFile(editor->GetFileName())) {
                clDEBUG() << "CodeFormatter: engine is set to ASTYLE. Source is not C/C++, skipped" << clEndl;
                return;
            }
            clDEBUG() << "Using C++/AStyle formatter" << clEndl;
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
            if(!output.IsEmpty()) {

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
                    clEditorStateLocker lk(editor->GetCtrl());
                    // format the text (add the indentation)
                    output = editor->FormatTextKeepIndent(output, editor->GetSelectionStart(),
                        Format_Text_Indent_Prev_Line | Format_Text_Save_Empty_Lines);
                    editor->ReplaceSelection(output);

                } else {
                    clEditorStateLocker lk(editor->GetCtrl());
                    editor->SetEditorText(output);
                }
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
    m_mgr->GetTheApp()->Disconnect(XRCID("format_source"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormat), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("formatter_options"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormatOptions), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("format_source"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(CodeFormatter::OnFormatUI), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("formatter_options"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(CodeFormatter::OnFormatOptionsUI), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(ID_TOOL_SOURCE_CODE_FORMATTER, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormatProject), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_STRING, clSourceFormatEventHandler(CodeFormatter::OnFormatString), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_FORMAT_FILE, clSourceFormatEventHandler(CodeFormatter::OnFormatFile), NULL, this);
    EventNotifier::Get()->Unbind(
        wxEVT_BEFORE_EDITOR_SAVE, clCommandEventHandler(CodeFormatter::OnBeforeFileSave), this);
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
    if(FileExtManager::IsPHPFile(e.GetFileName())) {

        if(fmtroptions.GetPhpEngine() == kPhpFormatEngineBuiltin) {
            // use the built-in PHP formatter
            // Construct the formatting options
            PHPFormatterOptions phpOptions;
            phpOptions.flags = fmtroptions.GetPHPFormatterOptions();
            if(m_mgr->GetEditorSettings()->GetIndentUsesTabs()) {
                phpOptions.flags |= kPFF_UseTabs;
            }
            phpOptions.indentSize = m_mgr->GetEditorSettings()->GetTabWidth();
            phpOptions.eol = m_mgr->GetEditorSettings()->GetEOLAsString();
            // Create the formatter buffer
            PHPFormatterBuffer buffer(e.GetInputString(), phpOptions);

            // Format the source
            buffer.format();

            // set the output
            output = buffer.GetBuffer();
        } else {
            wxFileName php(fmtroptions.GetPhpExecutable());
            if(!php.Exists()) {
                ::wxMessageBox(_("Can not format file using PHP-CS-Fixer: Missing PHP executable path"),
                    "Code Formatter", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }
            wxFileName phar(fmtroptions.GetPHPCSFixerPhar());
            if(!phar.Exists()) {
                ::wxMessageBox(_("Can not format file using PHP-CS-Fixer: Missing PHAR file"), "Code Formatter",
                    wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            // Run the command, PHP-CS-Fixer works directly on the file
            //
            output.Clear();
            IProcess::Ptr_t phpFixer(::CreateSyncProcess(
                fmtroptions.GetPhpFixerCommand(), IProcessCreateDefault | IProcessCreateWithHiddenConsole));
            CHECK_PTR_RET(phpFixer);
            phpFixer->WaitForTerminate(output);
        }
    } else if(fmtroptions.GetEngine() == kFormatEngineAStyle) {
        if(!FileExtManager::IsCxxFile(e.GetFileName())) {
            clDEBUG() << "CodeFormatter: engine is set to ASTYLE. Source is not C/C++, skipped" << clEndl;
            e.Skip();
            return;
        }

        wxString options = fmtroptions.AstyleOptionsAsString();

        // determine indentation method and amount
        bool useTabs = m_mgr->GetEditorSettings()->GetIndentUsesTabs();
        int tabWidth = m_mgr->GetEditorSettings()->GetTabWidth();
        int indentWidth = m_mgr->GetEditorSettings()->GetIndentWidth();
        options << (useTabs && tabWidth == indentWidth ? wxT(" -t") : wxT(" -s")) << indentWidth;

        AstyleFormat(str, options, output);
        output << DoGetGlobalEOLString();

    } else if(fmtroptions.GetEngine() == kFormatEngineClangFormat) {
        if(!FileExtManager::IsCxxFile(e.GetFileName()) && !FileExtManager::IsJavascriptFile(e.GetFileName())) {
            clDEBUG() << "CodeFormatter: engine is set to clang-format. Source is not C/C++/JavaScript, skipped"
                      << clEndl;
            e.Skip();
            return;
        }
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

bool CodeFormatter::ClangFormatBuffer(const wxString& content, const wxFileName& filename, wxString& formattedOutput,
    int& cursorPosition, int startOffset, int length)
{
    // Write the content into a temporary file
    wxFileName fn(filename.GetPath(), ".code-formatter-tmp.cpp");
    fn.SetExt(filename.GetExt());

    // Delete the temporary file
    FileUtils::Deleter fd(fn);
    wxFFile fp(fn.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(content, wxConvUTF8);
        fp.Close();
    }

    FormatOptions options;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &options);
    bool res = DoClangFormat(fn, formattedOutput, cursorPosition, startOffset, length, options, filename);
    {
        // Delete the temporary file
        wxLogNull nl;
        ::wxRemoveFile(fn.GetFullPath());
    }
    return res;
}

bool CodeFormatter::ClangFormatFile(
    const wxFileName& filename, wxString& formattedOutput, int& cursorPosition, int startOffset, int length)
{
    FormatOptions options;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &options);
    return DoClangFormat(filename, formattedOutput, cursorPosition, startOffset, length, options, filename);
}

bool CodeFormatter::ClangPreviewFormat(const wxString& content, wxString& formattedOutput, const FormatOptions& options)
{
    int startOffset, length, cursorPosition;
    startOffset = length = cursorPosition = wxNOT_FOUND;

    wxFileName fn(clStandardPaths::Get().GetTempDir(), ".code-formatter-tmp.cpp");
    FileUtils::Deleter fd(fn);

    wxFFile fp(fn.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(content, wxConvUTF8);
        fp.Close();
    }
    bool res = DoClangFormat(fn, formattedOutput, cursorPosition, startOffset, length, options, fn);
    {
        // Delete the temporary file
        wxLogNull nl;
        ::wxRemoveFile(fn.GetFullPath());
    }
    return res;
}

bool CodeFormatter::DoClangFormat(const wxFileName& filename, wxString& formattedOutput, int& cursorPosition,
    int startOffset, int length, const FormatOptions& options, const wxFileName& originalFileName)
{
    // clang-format
    // Build the command line to run
    if(options.GetClangFormatExe().IsEmpty()) {
        return false;
    }

    wxString command, file;

    clClangFormatLocator locator;
    double version = locator.GetVersion(options.GetClangFormatExe());

    command << options.GetClangFormatExe();
    file = filename.GetFullPath();
    ::WrapWithQuotes(command);
    ::WrapWithQuotes(file);

    command << " -assume-filename=" << originalFileName.GetFullName() << " ";
    command << options.ClangFormatOptionsAsString(filename, version);
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
    clDEBUG() << "CodeForamtter:" << command << clEndl;

    // Execute clang-format and reand the output
    formattedOutput.Clear();

    // Start clang-format with working directory set to the originalFileName path
    // By default, CodeLite will create a temporary file and format it instead of the original file
    // This temporary file is located under %appdata%\Temp folder
    // We want that clang-format working directory will be located under the _original_ file directory
    // (this is where the user might place a .clang-format file)
    IProcess::Ptr_t clangFormatProc(::CreateSyncProcess(
        command, IProcessCreateDefault | IProcessCreateWithHiddenConsole, originalFileName.GetPath()));
    CHECK_PTR_RET_FALSE(clangFormatProc);
    clangFormatProc->WaitForTerminate(formattedOutput);
    clDEBUG1() << "clang-format returned with:\n" << formattedOutput << clEndl;
    clDEBUG() << "Done" << clEndl;
    if(formattedOutput.IsEmpty()) {
        // crash?
        clWARNING() << "DoClangFormat:: an error occurred. Got empty response" << clEndl;
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

    ProjectPtr pProj = clCxxWorkspaceST::Get()->GetProject(selectedItem.m_text);
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

    switch(options.GetEngine()) {
    case kFormatEngineAStyle:
        return AStyleBatchFOrmat(files, options);
    case kFormatEngineClangFormat:
        return ClangBatchFormat(files, options);
    }
    return false;
}

bool CodeFormatter::ClangBatchFormat(const std::vector<wxFileName>& files, const FormatOptions& options)
{
    if(options.GetClangFormatExe().IsEmpty()) {
        return false;
    }

    wxProgressDialog dlg(
        _("Source Code Formatter"), _("Formatting files..."), (int)files.size(), m_mgr->GetTheApp()->GetTopWindow());

    clClangFormatLocator locator;
    double version = locator.GetVersion(options.GetClangFormatExe());

    for(size_t i = 0; i < files.size(); ++i) {
        wxString command, file;
        command << options.GetClangFormatExe();
        ::WrapWithQuotes(command);

        command << " -i "; // inline editing
        command << options.ClangFormatOptionsAsString(files.at(i), version);
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

        // Execute clang-format and read the output
        IProcess::Ptr_t clangFormatProc(::CreateSyncProcess(
            command, IProcessCreateDefault | IProcessCreateWithHiddenConsole, files.at(i).GetPath()));
        CHECK_PTR_RET_FALSE(clangFormatProc);

        wxString output;
        clangFormatProc->WaitForTerminate(output);
        CL_DEBUG("clang-format returned with:\n%s\n", output);
    }

    EventNotifier::Get()->PostReloadExternallyModifiedEvent(false);
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
    return true;
}

bool CodeFormatter::PhpFormat(const wxString& content, wxString& formattedOutput, const FormatOptions& options)
{
    // Construct the formatting options
    PHPFormatterOptions phpOptions;
    phpOptions.flags = options.GetPHPFormatterOptions();
    if(m_mgr->GetEditorSettings()->GetIndentUsesTabs()) {
        phpOptions.flags |= kPFF_UseTabs;
    }
    phpOptions.indentSize = m_mgr->GetEditorSettings()->GetTabWidth();
    phpOptions.eol = m_mgr->GetEditorSettings()->GetEOLAsString();
    // Create the formatter buffer
    PHPFormatterBuffer buffer(content, phpOptions);

    // Format the source
    buffer.format();
    formattedOutput << buffer.GetBuffer();
    return true;
}

void CodeFormatter::OnBeforeFileSave(clCommandEvent& e)
{
    e.Skip();
    FormatOptions fmtroptions;
    m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);
    if(fmtroptions.HasFlag(kCF_AutoFormatOnFileSave)) {
        // format the file before we save it
        IEditor* editor = m_mgr->FindEditor(e.GetFileName());
        if(editor && m_mgr->GetActiveEditor() == editor) {
            // we have our editor, format it
            DoFormatFile(editor);
        }
    }
}

void CodeFormatter::DoFormatXmlSource(IEditor* editor)
{
    wxXmlDocument doc;
    wxStringInputStream ss(editor->GetCtrl()->GetText());
    if(!doc.Load(ss)) {
        clWARNING() << "Failed to format XML file (Load):" << editor->GetFileName() << clEndl;
        return;
    }

    wxString formattedOutput;
    wxStringOutputStream os(&formattedOutput);
    if(!doc.Save(os, m_mgr->GetEditorSettings()->GetIndentWidth())) {
        clWARNING() << "Failed to format XML file (Save):" << editor->GetFileName() << clEndl;
        return;
    }

    clDEBUG() << "CodeForamtter: using standard XML foramtter" << clEndl;
    clEditorStateLocker lk(editor->GetCtrl());
    int curpos = editor->GetCurrentPosition();
    editor->GetCtrl()->BeginUndoAction();
    editor->SetEditorText(formattedOutput);
    editor->SetCaretAt(curpos);

    // Convert SPACEs to TABs?
    if(m_mgr->GetEditorSettings()->GetIndentUsesTabs()) {
        wxCommandEvent evt(wxEVT_MENU, XRCID("convert_indent_to_tabs"));
        wxTheApp->GetTopWindow()->GetEventHandler()->ProcessEvent(evt);
    }

    editor->GetCtrl()->EndUndoAction();
}
