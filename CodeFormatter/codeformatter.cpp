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
#include "codeformatter.h"

#include "JSON.h"
#include "asyncprocess.h"
#include "clEditorConfig.h"
#include "clEditorStateLocker.h"
#include "clFilesCollector.h"
#include "clKeyboardManager.h"
#include "clSTCLineKeeper.h"
#include "clWorkspaceManager.h"
#include "codeformatterdlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "formatoptions.h"
#include "globals.h"
#include "macros.h"
#include "phpoptions.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "workspace.h"

#include <algorithm>
#include <thread>
#include <wx/app.h> //wxInitialize/wxUnInitialize
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/xrc/xmlres.h>

static int ID_TOOL_SOURCE_CODE_FORMATTER = ::wxNewId();
FormatOptions CodeFormatter::m_options;

#define CHECK_FORMAT_ENGINE_RETURN(e) \
    if(e == kFormatEngineNone) {      \
        return;                       \
    }

#define CHECK_FORMAT_ENGINE_CONTINUE(e) \
    if(e == kFormatEngineNone) {        \
        continue;                       \
    }

extern "C" char* STDCALL AStyleMain(const char* pSourceIn, const char* pOptions,
                                    void(STDCALL* fpError)(int, const char*), char*(STDCALL* fpAlloc)(unsigned long));

//------------------------------------------------------------------------
// Astyle functions required by AStyleLib
// Error handler for the Artistic Style formatter
void STDCALL ASErrorHandler(int errorNumber, const char* errorMessage)
{
    wxString errStr;
    errStr << _U(errorMessage) << " (error " << errorNumber << ")";
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
    info.SetAuthor("Eran Ifrah");
    info.SetName("Source Code Formatter");
    info.SetDescription(_("Source Code Formatter (Supports C/C++/Obj-C/JavaScript/PHP files)"));
    info.SetVersion("v2.0");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeFormatter::CodeFormatter(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Source Code Formatter");
    m_shortName = _("Source Code Formatter");

    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormatProject, this, ID_TOOL_SOURCE_CODE_FORMATTER);
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormatFiles, this, XRCID("format_files"));

    EventNotifier::Get()->Bind(wxEVT_FORMAT_STRING, &CodeFormatter::OnFormatString, this);
    EventNotifier::Get()->Bind(wxEVT_FORMAT_FILE, &CodeFormatter::OnFormatFile, this);
    EventNotifier::Get()->Bind(wxEVT_BEFORE_EDITOR_SAVE, clCommandEventHandler(CodeFormatter::OnBeforeFileSave), this);
    EventNotifier::Get()->Bind(wxEVT_PHP_SETTINGS_CHANGED, &CodeFormatter::OnPhpSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &CodeFormatter::OnContextMenu, this);

    clKeyboardManager::Get()->AddAccelerator(
        _("Source Code Formatter"),
        { { "format_source", _("Format Current Source"), "Ctrl-I" }, { "formatter_options", _("Options...") } });

    m_optionsPhp.Load();
    if(!m_mgr->GetConfigTool()->ReadObject("FormatterOptions", &m_options)) {
        m_options.AutodetectSettings();
    }
}

CodeFormatter::~CodeFormatter() {}

void CodeFormatter::CreateToolBar(clToolBar* toolbar)
{
    // support both toolbars icon size
    int size = m_mgr->GetToolbarIconSize();

    clBitmapList* images = toolbar->GetBitmapsCreateIfNeeded();
    toolbar->AddSpacer();
    toolbar->AddTool(XRCID("format_source"), _("Format Source"), images->Add("format"), _("Format Source Code"));
    toolbar->AddTool(XRCID("formatter_options"), _("Format Options"), images->Add("cog"),
                     _("Source Code Formatter Options..."));
    // Connect the events to us
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormat, this, XRCID("format_source"));
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &CodeFormatter::OnFormatOptions, this, XRCID("formatter_options"));
    m_mgr->GetTheApp()->Bind(wxEVT_UPDATE_UI, &CodeFormatter::OnFormatOptionsUI, this, XRCID("formatter_options"));
    m_mgr->GetTheApp()->Bind(wxEVT_UPDATE_UI, &CodeFormatter::OnFormatUI, this, XRCID("format_source"));
}

void CodeFormatter::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("format_source"), _("Format Current Source"), _("Format Current Source"),
                          wxITEM_NORMAL);
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
    if(!editor) {
        return;
    }

    int selStart = wxNOT_FOUND, selEnd = wxNOT_FOUND;
    if(editor->GetSelectionStart() != wxNOT_FOUND && editor->GetSelectionStart() < editor->GetSelectionEnd()) {
        // we got a selection
        selStart = editor->GetSelectionStart();
        selEnd = editor->GetSelectionEnd();
        // Round selection to lines
        selStart = editor->PosFromLine(editor->LineFromPos(selStart));
        selEnd = editor->LineEnd(editor->LineFromPos(selEnd));
    }

    DoFormatEditor(editor, selStart, selEnd);
}

FormatterEngine CodeFormatter::FindFormatter(const wxFileName& fileName)
{
    // C++
    if(FileExtManager::IsCxxFile(fileName)) {
        switch(m_options.GetEngine()) {
        case kCxxFormatEngineClangFormat:
            return kFormatEngineClangFormat;
        case kCxxFormatEngineAStyle:
            return kFormatEngineAStyle;
        case kCxxForamtEngineNone:
            return kFormatEngineNone;
        }
    }

    // PHP
    if(FileExtManager::IsPHPFile(fileName)) {
        switch(m_options.GetPhpEngine()) {
        case kPhpFormatEnginePhpCsFixer:
            return kFormatEnginePhpCsFixer;
        case kPhpFormatEnginePhpcbf:
            return kFormatEnginePhpcbf;
        case kPhpFormatEngineBuiltin:
            return kFormatEngineBuildInPhp;
        case kPhpForamtEngineNone:
            return kFormatEngineNone;
        }
    }

    // XML
    if(FileExtManager::IsFileType(fileName, FileExtManager::TypeXml) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeXRC) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeWorkspace) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeProject)) {
        switch(m_options.GetXmlEngine()) {
        case kXmlForamtEngineNone:
            return kFormatEngineNone;
        case kXmlFormatEngineBuiltin:
            return kFormatEngineWxXmlDocument;
        }
    }

    // JavaScript / TypeScript
    if(FileExtManager::IsJavascriptFile(fileName) || FileExtManager::IsJavaFile(fileName)) {
        switch(m_options.GetJavaScriptEngine()) {
        case kJSForamtEngineNone:
            return kFormatEngineNone;
        case kJSFormatEngineClangFormat:
            return kFormatEngineClangFormat;
        }
    }

    // Rust
    if(FileExtManager::IsFileType(fileName, FileExtManager::TypeRust)) {
        switch(m_options.GetRustEngine()) {
        case kRustForamtEngineNone:
            return kFormatEngineNone;
        case kRustFormatEngineRustfmt:
            return kFormatEngineRust;
        }
    }

    // JSON file types
    if(FileExtManager::IsFileType(fileName, FileExtManager::TypeJSON) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeWxCrafter) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeWorkspaceFileSystem) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeWorkspaceNodeJS) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeWorkspaceDocker) ||
       FileExtManager::IsFileType(fileName, FileExtManager::TypeWorkspacePHP)) {
        switch(m_options.GetJsonEngine()) {
        case kJSONForamtEngineNone:
            return kFormatEngineNone;
        case kJSONFormatEngineBuiltin:
            return kFormatEngineJSON;
        }
    }
    return kFormatEngineNone;
}

bool CodeFormatter::CanFormatSelection(const FormatterEngine& engine)
{
    if(engine == kFormatEngineClangFormat || engine == kFormatEngineAStyle) {
        return true;
    }

    return false;
}

bool CodeFormatter::CanFormatString(const FormatterEngine& engine)
{
    if(engine == kFormatEngineAStyle || engine == kFormatEngineClangFormat || engine == kFormatEngineBuildInPhp) {
        return true;
    }

    return false;
}

bool CodeFormatter::CanFormatFile(const FormatterEngine& engine)
{
    if(engine == kFormatEngineClangFormat || engine == kFormatEnginePhpCsFixer || engine == kFormatEnginePhpcbf ||
       engine == kFormatEngineWxXmlDocument || engine == kFormatEngineRust || engine == kFormatEngineJSON) {
        return true;
    }

    return false;
}

void CodeFormatter::DoFormatEditor(IEditor* editor, int selStart, int selEnd)
{
    const wxFileName& fileName = editor->GetFileName();

    // find a suitable formatter
    FormatterEngine engine = FindFormatter(fileName);
    CHECK_FORMAT_ENGINE_RETURN(engine);

    m_mgr->SetStatusMessage(wxString() << _("Formatting: ") << fileName.GetFullPath(), 0);

    // Notify about indentation about to start
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_STARTING);
    evt.SetString(fileName.GetFullPath());
    EventNotifier::Get()->ProcessEvent(evt);

    int cursorPosition = editor->GetCurrentPosition();

    if(engine == kFormatEngineRust) {
        // special handling for rust
        DoFormatWithRustfmt(editor, fileName);
    } else {
        wxString content;
        if(selStart != wxNOT_FOUND && CanFormatSelection(engine)) {
            content = editor->GetTextRange(selStart, selEnd);
            DoFormatSelection(editor, content, engine, cursorPosition, selStart, selEnd);
        } else {
            content = editor->GetEditorText();
            DoFormatString(content, fileName, engine, cursorPosition);
            selStart = wxNOT_FOUND;
            selEnd = wxNOT_FOUND;
        }
        OverwriteEditorText(editor, content, cursorPosition, selStart, selEnd);
    }

    // Notify that a file was indented
    wxCommandEvent evtDone(wxEVT_CODEFORMATTER_INDENT_COMPLETED);
    evtDone.SetString(fileName.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(evtDone);

    m_mgr->SetStatusMessage(_("Done"), 0);
}

void CodeFormatter::DoFormatPreview(wxString& content, const wxString& ext, const FormatterEngine& engine)
{
    wxFileName tempFileName = m_options.GetPreviewFileName();
    tempFileName.SetExt(ext);
    int cursorPosition = wxNOT_FOUND;
    DoFormatString(content, tempFileName, engine, cursorPosition);
}

void CodeFormatter::DoFormatString(wxString& content, const wxFileName& fileName, const FormatterEngine& engine,
                                   int& cursorPosition)
{
    if(!CanFormatString(engine)) {
        DoFormatWithTempFile(fileName, content, engine);
        return;
    }

    if(engine == kFormatEngineAStyle) {
        DoFormatWithAstyle(content);
    } else if(engine == kFormatEngineBuildInPhp) {
        DoFormatWithBuildInPhp(content);
    } else if(engine == kFormatEngineClangFormat) {
        DoFormatWithClang(content, fileName, cursorPosition);
    }
}

void CodeFormatter::DoFormatWithTempFile(const wxFileName& fileName, wxString& content, const FormatterEngine& engine)
{
    wxFileName tempFileName = fileName.GetFullPath() + "-code-formatter-tmp." + fileName.GetExt();
    FileUtils::Deleter fd(tempFileName);

    if(!FileUtils::WriteFileContent(tempFileName, content)) {
        clWARNING() << "CodeFormatter: Failed to save file: " << tempFileName << endl;
        return;
    }

    DoFormatFile(tempFileName, engine);

    if(!FileUtils::ReadFileContent(tempFileName, content)) {
        clWARNING() << "CodeFormatter: Failed to load file: " << tempFileName << endl;
    }
}

void CodeFormatter::DoFormatFile(const wxFileName& fileName, const FormatterEngine& engine)
{
    clDEBUG() << "CodeFormatter formatting file: " << fileName << endl;

    if(!CanFormatFile(engine)) {
        DoFormatFileAsString(fileName, engine);
    } else if(engine == kFormatEngineClangFormat) {
        DoFormatWithClang(fileName);
    } else if(engine == kFormatEnginePhpCsFixer) {
        DoFormatWithPhpCsFixer(fileName);
    } else if(engine == kFormatEnginePhpcbf) {
        DoFormatWithPhpcbf(fileName);
    } else if(engine == kFormatEngineWxXmlDocument) {
        DoFormatWithWxXmlDocument(fileName);
    } else if(engine == kFormatEngineJSON) {
        DoFormatWithcJSON(fileName);
    }
    clDEBUG() << "CodeFormatter file formatted: " << fileName << endl;
}

void CodeFormatter::DoFormatSelection(IEditor* editor, wxString& content, const FormatterEngine& engine,
                                      int& cursorPosition, const int& selStart, const int& selEnd)
{
    if(engine == kFormatEngineAStyle) {
        DoFormatWithAstyle(content, false);
        content = editor->FormatTextKeepIndent(content, selStart,
                                               Format_Text_Indent_Prev_Line | Format_Text_Save_Empty_Lines);
    } else if(engine == kFormatEngineClangFormat) {
        content = editor->GetEditorText();
        wxFileName fileName = editor->GetFileName();
        DoFormatWithClang(content, fileName, cursorPosition, selStart, selEnd);
    }
}

void CodeFormatter::DoFormatWithPhpCsFixer(const wxFileName& fileName)
{
    wxString command;
    if(!m_options.GetPhpFixerCommand(fileName, command)) {
        return;
    }
    RunCommand(command);
}

void CodeFormatter::DoFormatWithPhpcbf(const wxFileName& fileName)
{
    wxString command;
    if(!m_options.GetPhpcbfCommand(fileName, command)) {
        return;
    }
    RunCommand(command);
}

wxString CodeFormatter::RunCommand(const wxString& command)
{
    clDEBUG() << "CodeFormatter running: " << command << endl;

    IProcess::Ptr_t process(::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    if(!process) {
        return "Failed to execute:\n" + command;
    }

    wxString output;
    process->WaitForTerminate(output);
    return output;
}

void CodeFormatter::DoFormatWithBuildInPhp(wxString& content)
{
    // Construct the formatting options
    PHPFormatterOptions phpOptions;
    phpOptions.flags = m_options.GetPHPFormatterOptions();
    if(m_mgr->GetEditorSettings()->GetIndentUsesTabs()) {
        phpOptions.flags |= kPFF_UseTabs;
    }
    phpOptions.indentSize = m_mgr->GetEditorSettings()->GetTabWidth();
    phpOptions.eol = m_mgr->GetEditorSettings()->GetEOLAsString();

    // Create the formatter buffer
    PHPFormatterBuffer buffer(content, phpOptions);

    // Format the source
    buffer.format();

    content = buffer.GetBuffer();
}

void CodeFormatter::DoFormatWithClang(const wxFileName& fileName)
{
    if(m_options.GetClangFormatExe().IsEmpty()) {
        clWARNING() << "CodeFormatter: Missing clang_format exec" << endl;
        return;
    }

    wxString command = m_options.ClangFormatCommand(fileName);
    RunCommand(command);
}

void CodeFormatter::DoFormatWithRustfmt(IEditor* editor, const wxFileName& fileName)
{
    if(m_options.GetRustCommand().IsEmpty()) {
        clWARNING() << "CodeFormatter: Missing rustfmt command" << endl;
        return;
    }

    // the problem with rustfmt is that we can not use a different file name
    // but we might have modified changes in the editor
    // to workaround this, we need to do the following:
    // - save the editor content
    // - format the file
    // - overwrite the buffer with the modified version on the file system
    bool undo = false;
    if(editor->IsEditorModified()) {
        if(!editor->Save()) {
            ::wxMessageBox(_("Failed to save file:\n") + fileName.GetFullPath(), _("Source Code Formatter"),
                           wxOK | wxICON_ERROR);
            return;
        }
        undo = true;
    }

    wxString command = m_options.RustfmtCommand(fileName);
    clDEBUG() << "Running:" << command << endl;
    RunCommand(command);

    // revert the save we did earlier
    if(undo) {
        // restore the file to its state before we saved it
        editor->GetCtrl()->Undo();
    }

    // read the formatted content
    wxString fixedContent;
    if(!FileUtils::ReadFileContent(fileName, fixedContent)) {
        return;
    }

    OverwriteEditorText(editor, fixedContent, editor->GetCurrentPosition());
}

void CodeFormatter::DoFormatWithClang(wxString& content, const wxFileName& fileName, int& cursorPosition,
                                      const int& selStart, const int& selEnd)
{
    if(m_options.GetClangFormatExe().IsEmpty()) {
        clWARNING() << "CodeFormatter: Missing clang_format exec" << endl;
        return;
    }

    int tailLength;
    if(selStart != wxNOT_FOUND) {
        tailLength = content.length() - selEnd;
    }

    wxFileName tempFileName = fileName.GetFullPath() + "-code-formatter-tmp." + fileName.GetExt();
    FileUtils::Deleter fd(tempFileName);
    if(!FileUtils::WriteFileContent(tempFileName, content)) {
        clWARNING() << "CodeFormatter: Failed to save file: " << tempFileName << endl;
        return;
    }

    wxString command =
        m_options.ClangFormatCommand(tempFileName, fileName.GetFullName(), cursorPosition, selStart, selEnd);

    content = RunCommand(command);

    // The first line contains the cursor position
    if(cursorPosition != wxNOT_FOUND) {
        wxString metadata = content.BeforeFirst('\n');
        JSON root(metadata);
        cursorPosition = root.toElement().namedObject("cursor").toInt(wxNOT_FOUND);
        content = content.AfterFirst('\n');
    }

    if(selStart != wxNOT_FOUND) {
        content = content.Mid(selStart, content.length() - tailLength - selStart);
    }
}

void CodeFormatter::DoFormatWithAstyle(wxString& content, const bool& appendEOL)
{
    wxString options = m_options.AstyleOptionsAsString();

    // determine indentation method and amount
    bool useTabs = m_mgr->GetEditorSettings()->GetIndentUsesTabs();
    int tabWidth = m_mgr->GetEditorSettings()->GetTabWidth();
    int indentWidth = m_mgr->GetEditorSettings()->GetIndentWidth();
    options << (useTabs && tabWidth == indentWidth ? " -t" : " -s") << indentWidth;

    char* textOut = AStyleMain(_C(content), _C(options), ASErrorHandler, ASMemoryAlloc);
    content.clear();
    if(textOut) {
        content = _U(textOut);
        content.Trim();
        delete[] textOut;
    }
    if(content.IsEmpty() || !appendEOL) {
        return;
    }

    content << DoGetGlobalEOLString();
}

void CodeFormatter::DoFormatFileAsString(const wxFileName& fileName, const FormatterEngine& engine)
{
    wxString content;
    if(!FileUtils::ReadFileContent(fileName, content)) {
        clWARNING() << "CodeFormatter: Failed to load file: " << fileName << endl;
        return;
    }

    int cursorPosition = wxNOT_FOUND;
    DoFormatString(content, fileName, engine, cursorPosition);
    if(content.IsEmpty()) {
        return;
    }

    if(!FileUtils::WriteFileContent(fileName, content)) {
        clWARNING() << "CodeFormatter: Failed to save file: " << fileName << endl;
        return;
    }
}

void CodeFormatter::DoFormatWithcJSON(const wxFileName& fileName)
{
    JSON json(fileName);
    if(!json.isOk()) {
        clWARNING() << "CodeFormatter: Failed to load JSON file: " << fileName << endl;
        return;
    }

    // prettify the content and save it
    wxString pretty = json.toElement().format();
    if(!FileUtils::WriteFileContent(fileName, pretty)) {
        clWARNING() << "Failed to save file:" << fileName << endl;
        return;
    }
}

void CodeFormatter::DoFormatWithWxXmlDocument(const wxFileName& fileName)
{
    wxString filePaht = fileName.GetFullPath();
    wxXmlDocument doc;
    if(!doc.Load(filePaht) || !doc.Save(filePaht, m_mgr->GetEditorSettings()->GetIndentWidth())) {
        clWARNING() << "CodeFormatter: Failed to format XML file: " << fileName << endl;
        return;
    }
}

void CodeFormatter::OverwriteEditorText(IEditor*& editor, const wxString& content, const int& cursorPosition,
                                        const int& selStart, const int& selEnd)
{
    wxString editroContent;
    if(selStart == wxNOT_FOUND) {
        editroContent = editor->GetEditorText();
    } else {
        editroContent = editor->GetTextRange(selStart, selEnd);
    }

    if(content.IsEmpty() || editroContent.IsSameAs(content)) {
        return;
    }

    clEditorStateLocker lk(editor->GetCtrl());
    editor->GetCtrl()->BeginUndoAction();

    if(selStart == wxNOT_FOUND) {
        editor->SetEditorText(content);
    } else {
        editor->SelectText(selStart, selEnd - selStart);
        editor->ReplaceSelection(content);
    }

    editor->SetCaretAt(cursorPosition);
    editor->GetCtrl()->EndUndoAction();
}

void CodeFormatter::OnFormatOptions(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxString cppSampleFile, phpSampleFile, cppSample, phpSample;
    cppSampleFile << m_mgr->GetStartupDirectory() << "/astyle.sample";
    phpSampleFile << m_mgr->GetStartupDirectory() << "/php.sample";
    ReadFileWithConversion(cppSampleFile, cppSample);
    ReadFileWithConversion(phpSampleFile, phpSample);

    CodeFormatterDlg dlg(NULL, m_mgr, this, m_options, cppSample, phpSample);
    dlg.ShowModal();

    // Reload options in case changes where cancled
    m_mgr->GetConfigTool()->ReadObject("FormatterOptions", &m_options);
}

void CodeFormatter::OnFormatUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->GetActiveEditor() != NULL);
}

void CodeFormatter::OnFormatOptionsUI(wxUpdateUIEvent& e) { e.Enable(true); }

void CodeFormatter::OnContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    event.GetMenu()->Append(XRCID("format_files"), _("Source Code Formatter"));
    m_selectedFolder = event.GetPath();
}

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
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormat, this, XRCID("format_source"));
    m_mgr->GetTheApp()->Unbind(wxEVT_UPDATE_UI, &CodeFormatter::OnFormatUI, this, XRCID("format_source"));
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormatOptions, this, XRCID("formatter_options"));
    m_mgr->GetTheApp()->Unbind(wxEVT_UPDATE_UI, &CodeFormatter::OnFormatOptionsUI, this, XRCID("formatter_options"));

    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormatProject, this, ID_TOOL_SOURCE_CODE_FORMATTER);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &CodeFormatter::OnFormatFiles, this, XRCID("format_files"));

    EventNotifier::Get()->Unbind(wxEVT_FORMAT_STRING, &CodeFormatter::OnFormatString, this);
    EventNotifier::Get()->Unbind(wxEVT_FORMAT_FILE, &CodeFormatter::OnFormatFile, this);
    EventNotifier::Get()->Unbind(wxEVT_BEFORE_EDITOR_SAVE, &CodeFormatter::OnBeforeFileSave, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_SETTINGS_CHANGED, &CodeFormatter::OnPhpSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &CodeFormatter::OnContextMenu, this);
}

IManager* CodeFormatter::GetManager() { return m_mgr; }

void CodeFormatter::OnFormatString(clSourceFormatEvent& e)
{
    wxString content = e.GetInputString();
    if(content.IsEmpty()) {
        e.SetFormattedString(content);
        return;
    }

    FormatterEngine engine = FindFormatter(e.GetFileName());
    CHECK_FORMAT_ENGINE_RETURN(engine);

    int cursorPosition = wxNOT_FOUND;
    DoFormatString(content, e.GetFileName(), engine, cursorPosition);
    e.SetFormattedString(content);
}

int CodeFormatter::DoGetGlobalEOL() const
{
    OptionsConfigPtr options = m_mgr->GetEditorSettings();
    if(options->GetEolMode() == "Unix (LF)") {
        return 2;
    } else if(options->GetEolMode() == "Mac (CR)") {
        return 1;
    } else if(options->GetEolMode() == "Windows (CRLF)") {
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
        return "\r\n";
    case 1:
        return "\r";
    case 2:
    default:
        return "\n";
    }
}

void CodeFormatter::OnFormatFile(clSourceFormatEvent& e)
{
    const wxFileName& fn = e.GetFileName();
    FormatterEngine engine = FindFormatter(fn);
    CHECK_FORMAT_ENGINE_RETURN(engine);

    // TODO skip files based on size, 4.5MB as the default
    std::vector<wxFileName> filesToFormat;
    filesToFormat.push_back(fn);
    BatchFormat(filesToFormat, true);
}

void CodeFormatter::OnFormatFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clGetManager()->SetStatusMessage(_("Code Formatter: scanning for files..."));
    std::thread thr(
        [=](const wxString& rootFolder, CodeFormatter* formatter) {
            clFilesScanner fs;
            std::vector<wxFileName> files;
            fs.Scan(rootFolder, files, "*", "*.o;*.obj;*.dll;*.a;*.exe;*.dylib;*.db", "build-*;.codelite;.git;.svn");

            std::vector<wxFileName> arrfiles;
            arrfiles.reserve(files.size());
            for(const wxFileName& f : files) {
                FormatterEngine engine = FindFormatter(f);
                CHECK_FORMAT_ENGINE_CONTINUE(engine);
                arrfiles.push_back(f);
            }
            formatter->CallAfter(&CodeFormatter::OnScanFilesCompleted, arrfiles);
        },
        m_selectedFolder, this);
    thr.detach();
}

void CodeFormatter::OnFormatProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    TreeItemInfo selectedItem = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(selectedItem.m_itemType != ProjectItem::TypeProject) {
        return;
    }

    ProjectPtr pProj = clCxxWorkspaceST::Get()->GetProject(selectedItem.m_text);
    CHECK_PTR_RET(pProj);

    const Project::FilesMap_t& allFiles = pProj->GetFiles();

    std::vector<wxFileName> filesToFormat;
    filesToFormat.reserve(allFiles.size());

    for(const auto& vt : allFiles) {
        FormatterEngine engine = FindFormatter(vt.second->GetFilename());
        CHECK_FORMAT_ENGINE_CONTINUE(engine);

        // TODO skip files based on size, 4.5MB as the default
        filesToFormat.push_back(vt.second->GetFilename());
    }
    BatchFormat(filesToFormat, false);
}

void CodeFormatter::BatchFormat(const std::vector<wxFileName>& files, bool silent)
{
    if(files.empty()) {
        if(!silent) {
            ::wxMessageBox(_("Project contains no supported files"));
        }
        return;
    }

    wxProgressDialog* dlg = nullptr;
    if(!silent) {
        wxString msg;
        msg << _("You are about to beautify ") << files.size() << _(" files\nContinue?");
        if(wxYES != ::wxMessageBox(msg, _("Source Code Formatter"), wxYES_NO | wxCANCEL | wxCENTER)) {
            return;
        }

        dlg = new wxProgressDialog(_("Source Code Formatter"), _("Formatting files..."), (int)files.size(),
                                   m_mgr->GetTheApp()->GetTopWindow());
    }
    for(size_t i = 0; i < files.size(); ++i) {
        wxString msg;
        msg << "[ " << i << " / " << files.size() << " ] " << files.at(i).GetFullName();
        if(dlg) {
            dlg->Update(i, msg);
        }

        FormatterEngine engine = FindFormatter(files.at(i).GetFullPath());
        CHECK_FORMAT_ENGINE_CONTINUE(engine);

        DoFormatFile(files.at(i).GetFullPath(), engine);
    }

    if(dlg) {
        dlg->Destroy();
    }
    EventNotifier::Get()->PostReloadExternallyModifiedEvent(false);
}

void CodeFormatter::OnBeforeFileSave(clCommandEvent& e)
{
    e.Skip();
    if(!m_options.HasFlag(kCF_AutoFormatOnFileSave)) {
        return;
    }

    IEditor* editor = m_mgr->FindEditor(e.GetFileName());
    if(!editor || m_mgr->GetActiveEditor() != editor) {
        return;
    }

    // we have our editor, format it
    DoFormatEditor(editor);
}

void CodeFormatter::OnPhpSettingsChanged(clCommandEvent& event)
{
    event.Skip();
    m_optionsPhp.Load();
}

void CodeFormatter::OnScanFilesCompleted(const std::vector<wxFileName>& files) { BatchFormat(files, false); }
