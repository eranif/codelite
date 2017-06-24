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
#include "asyncprocess.h"
#include "clEditorConfig.h"
#include "clEditorStateLocker.h"
#include "clSTCLineKeeper.h"
#include "codeformatter.h"
#include "codeformatterdlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "formatoptions.h"
#include "globals.h"
#include "json_node.h"
#include "macros.h"
#include "phpoptions.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "workspace.h"
#include "wx/ffile.h"
#include "wx/log.h"
#include "wx/menu.h"
#include <wx/app.h> //wxInitialize/wxUnInitialize
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/xrc/xmlres.h>

static int ID_TOOL_SOURCE_CODE_FORMATTER = ::wxNewId();

extern "C" char* STDCALL AStyleMain(const char* pSourceIn,
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

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

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
    m_mgr->GetTheApp()->Connect(XRCID("format_files"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormatFiles), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_BEFORE_EDITOR_SAVE, clCommandEventHandler(CodeFormatter::OnBeforeFileSave), this);
    EventNotifier::Get()->Bind(wxEVT_PHP_SETTINGS_CHANGED, &CodeFormatter::OnPhpSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &CodeFormatter::OnContextMenu, this);

    m_optionsPhp.Load();
    m_mgr->GetConfigTool()->ReadObject("FormatterOptions", &m_options);
}

CodeFormatter::~CodeFormatter()
{
}

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
    if(!editor)
        return;

    DoFormatEditor(editor);
}

int CodeFormatter::FindFormatter(const wxFileName& fileName)
{
    if(FileExtManager::IsCxxFile(fileName)) {
        if(m_options.GetEngine() == kFormatEngineClangFormat) {
            return cFormatEngineClangFormat;
        }
        if (m_options.GetEngine() == kFormatEngineAStyle) {
            return cFormatEngineAStyle;
        }
    }

    if(FileExtManager::IsPHPFile(fileName)) {
        if(m_options.GetPhpEngine() == kPhpFormatEnginePhpCsFixer) {
            return cFormatEnginePhpCsFixer;
        }
        if(m_options.GetPhpEngine() == kPhpFormatEnginePhpcbf) {
            return cFormatEnginePhpcbf;
        }
        if(m_options.GetPhpEngine() == kPhpFormatEngineBuiltin) {
            return cFormatEngineBuildInPhp;
        }
    }

    if(FileExtManager::IsFileType(fileName, FileExtManager::TypeXml) ||
        FileExtManager::IsFileType(fileName, FileExtManager::TypeXRC) ||
        FileExtManager::IsFileType(fileName, FileExtManager::TypeWorkspace) ||
        FileExtManager::IsFileType(fileName, FileExtManager::TypeProject)) {
        return cFormatEngineWxXmlDocument;
    }

    if(FileExtManager::IsJavascriptFile(fileName) || FileExtManager::IsJavaFile(fileName)) {
        return cFormatEngineClangFormat;
    }

    return cFormatEngineNone;
}

void CodeFormatter::DoFormatEditor(IEditor* editor)
{
    wxFileName fileName = editor->GetFileName();

    m_mgr->SetStatusMessage(wxString::Format(wxT("%s: %s..."), _("Formatting"), fileName.GetFullPath().c_str()), 0);

    // Notify about indentation about to start
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_STARTING);
    evt.SetString(fileName.GetFullPath());
    EventNotifier::Get()->ProcessEvent(evt);

    int curpos = editor->GetCurrentPosition();
    wxString content = editor->GetEditorText();
    int engine = FindFormatter(fileName);
    DoFormatWithTempFile(fileName, content, engine);
    OverwriteEditorText(editor, content, curpos);

    // Notify that a file was indented
    wxCommandEvent evtDone(wxEVT_CODEFORMATTER_INDENT_COMPLETED);
    evtDone.SetString(fileName.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(evtDone);

    m_mgr->SetStatusMessage(_("Done"), 0);
}

void CodeFormatter::DoFormatPreview(wxString& content, const wxString& ext, const int& engine)
{
    wxString path;

    clCxxWorkspace* cppworkspace = m_mgr->GetWorkspace();
    if (cppworkspace->IsOpen()) {
        path = cppworkspace->GetActiveProject()->GetProjectPath();
    }

    wxFileName tempFileName(path, "preview." + ext);
    DoFormatWithTempFile(tempFileName, content, engine);
}

void CodeFormatter::DoFormatWithTempFile(const wxFileName& fileName, wxString& content, const int& engine)
{
    wxFileName tempFileName = fileName.GetFullPath() + "-code-formatter-tmp." + fileName.GetExt();
    FileUtils::Deleter fd(tempFileName);

    if(!FileUtils::WriteFileContent(tempFileName, content)) {
        return;
    }

    DoFormatFile(tempFileName, engine);

    FileUtils::ReadFileContent(tempFileName, content);
}

void CodeFormatter::DoFormatFile(const wxFileName& fileName, const int& engine)
{
    clDEBUG() << "Formatting file: " << fileName << clEndl;

    if(engine == cFormatEngineAStyle) {
        DoFormatWithAstyle(fileName);
    } else if (engine == cFormatEngineClangFormat) {
        DoFormatWithClang(fileName);
    } else if (engine == cFormatEngineBuildInPhp) {
        DoFormatWithBuildInPhp(fileName);
    } else if (engine == cFormatEnginePhpCsFixer) {
        DoFormatWithPhpCsFixer(fileName);
    } else if (engine == cFormatEnginePhpcbf) {
        DoFormatWithPhpcbf(fileName);
    } else if (engine == cFormatEngineWxXmlDocument) {
        DoFormatWithWxXmlDocument(fileName);
    }

    clDEBUG() << "File formatted: " << fileName << clEndl;
}

bool CodeFormatter::DoFormatWithPhpCsFixer(const wxFileName& fileName)
{
    wxFileName phar(m_options.GetPHPCSFixerPhar());
    if(!IsPharConfigValid(phar)) {
        return false;
    }

    wxString command = m_options.GetPhpFixerCommand(fileName);
    return DoFormatExternally(fileName, command);
}

bool CodeFormatter::DoFormatWithPhpcbf(const wxFileName& fileName)
{
    wxFileName phar(m_options.GetPhpcbfPhar());
    if(!IsPharConfigValid(phar)) {
        return false;
    }

    wxString command = m_options.GetPhpcbfCommand(fileName);
    return DoFormatExternally(fileName, command);
}

bool CodeFormatter::DoFormatExternally(const wxFileName& fileName, const wxString& command)
{
    clDEBUG() << "CodeFormatter running: " << command << clEndl;
    ProcUtils::SafeExecuteCommand(command);
    return true;
}

bool CodeFormatter::DoFormatWithBuildInPhp(const wxFileName& fileName)
{
    // Construct the formatting options
    PHPFormatterOptions phpOptions;
    phpOptions.flags = m_options.GetPHPFormatterOptions();
    if(m_mgr->GetEditorSettings()->GetIndentUsesTabs()) {
        phpOptions.flags |= kPFF_UseTabs;
    }
    phpOptions.indentSize = m_mgr->GetEditorSettings()->GetTabWidth();
    phpOptions.eol = m_mgr->GetEditorSettings()->GetEOLAsString();

    wxString content;
    if(!FileUtils::ReadFileContent(fileName, content)) {
        return false;
    }

    // Create the formatter buffer
    PHPFormatterBuffer buffer(content, phpOptions);

    // Format the source
    buffer.format();

    if(!FileUtils::WriteFileContent(fileName, buffer.GetBuffer())) {
        return false;
    }

    return true;
}

bool CodeFormatter::DoFormatWithClang(const wxFileName& fileName)
{
    if(m_options.GetClangFormatExe().IsEmpty()) {
        return false;
    }

    wxString command = m_options.ClangFormatCommand(fileName);
    return DoFormatExternally(fileName, command);
}

bool CodeFormatter::DoFormatWithAstyle(const wxFileName& fileName)
{
    wxString content, options;

    if(!FileUtils::ReadFileContent(fileName, content)) {
        return false;
    }

    options << m_options.AstyleOptionsAsString();

    // determine indentation method and amount
    bool useTabs = m_mgr->GetEditorSettings()->GetIndentUsesTabs();
    int tabWidth = m_mgr->GetEditorSettings()->GetTabWidth();
    int indentWidth = m_mgr->GetEditorSettings()->GetIndentWidth();
    options << (useTabs && tabWidth == indentWidth ? wxT(" -t") : wxT(" -s")) << indentWidth;

    char* textOut = AStyleMain(_C(content), _C(options), ASErrorHandler, ASMemoryAlloc);
    content.clear();
    if(textOut) {
        content = _U(textOut);
        content.Trim();
        delete[] textOut;
    }
    if(content.IsEmpty()) {
        return false;
    }

    content << DoGetGlobalEOLString();

    if(!FileUtils::WriteFileContent(fileName, content)) {
        return false;
    }
}

bool CodeFormatter::DoFormatWithWxXmlDocument(const wxFileName& fileName)
{
    wxString filePaht = fileName.GetFullPath();
    wxXmlDocument doc;
    if(!doc.Load(filePaht) || !doc.Save(filePaht, m_mgr->GetEditorSettings()->GetIndentWidth())) {
        clWARNING() << "Failed to format XML file: " << fileName << clEndl;
        return false;
    }

    return true;
}

void CodeFormatter::OverwriteEditorText(IEditor*& editor, const wxString& content, const int& curpos)
{
    if(content.IsEmpty() || editor->GetEditorText().IsSameAs(content)) {
        return;
    }

    clEditorStateLocker lk(editor->GetCtrl());
    editor->GetCtrl()->BeginUndoAction();
    editor->SetEditorText(content);
    editor->SetCaretAt(curpos);
    editor->GetCtrl()->EndUndoAction();
}

void CodeFormatter::OnFormatOptions(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxString cppSampleFile, phpSampleFile, cppSample, phpSample;
    cppSampleFile << m_mgr->GetStartupDirectory() << wxT("/astyle.sample");
    phpSampleFile << m_mgr->GetStartupDirectory() << wxT("/php.sample");
    ReadFileWithConversion(cppSampleFile, cppSample);
    ReadFileWithConversion(phpSampleFile, phpSample);

    CodeFormatterDlg dlg(NULL, m_mgr, this, m_options, cppSample, phpSample);
    dlg.ShowModal();

    m_mgr->GetConfigTool()->ReadObject("FormatterOptions", &m_options);
}

void CodeFormatter::OnFormatUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->GetActiveEditor() != NULL);
}

void CodeFormatter::OnFormatOptionsUI(wxUpdateUIEvent& e)
{
    e.Enable(true);
}

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
    m_mgr->GetTheApp()->Disconnect(XRCID("format_files"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(CodeFormatter::OnFormatFiles), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_FORMAT_STRING, clSourceFormatEventHandler(CodeFormatter::OnFormatString), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_FORMAT_FILE, clSourceFormatEventHandler(CodeFormatter::OnFormatFile), NULL, this);
    EventNotifier::Get()->Unbind(
        wxEVT_BEFORE_EDITOR_SAVE, clCommandEventHandler(CodeFormatter::OnBeforeFileSave), this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_SETTINGS_CHANGED, &CodeFormatter::OnPhpSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &CodeFormatter::OnContextMenu, this);
}

IManager* CodeFormatter::GetManager()
{
    return m_mgr;
}

void CodeFormatter::OnFormatString(clSourceFormatEvent& e)
{
    wxString content = e.GetInputString();
    if(content.IsEmpty()) {
        e.SetFormattedString(content);
        return;
    }
    int engine = FindFormatter(e.GetFileName());
    DoFormatWithTempFile(e.GetFileName(), content, engine);
    e.SetFormattedString(content);
}

bool CodeFormatter::IsPharConfigValid(const wxFileName& phar)
{
    wxFileName php(m_optionsPhp.GetPhpExe());
    if(!php.Exists() || !phar.Exists()) {
        return false;
    }

    return true;
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

void CodeFormatter::OnFormatFile(clSourceFormatEvent& e)
{
    wxUnusedVar(e);
}

void CodeFormatter::OnFormatFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxArrayString files;
    wxDir::GetAllFiles(m_selectedFolder, &files);

    if(files.IsEmpty())
        return;

    std::vector<wxFileName> filesToFormat;

    for(size_t i = 0; i < files.GetCount(); ++i) {
        int engine = FindFormatter(files.Item(i));
        if (engine == cFormatEngineNone) {
            continue;
        }

        filesToFormat.push_back(files.Item(i));
    }

    BatchFormat(filesToFormat);
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

    Project::FileInfoVector_t allFiles;
    pProj->GetFilesMetadata(allFiles);

    std::vector<wxFileName> filesToFormat;

    for(size_t i = 0; i < allFiles.size(); ++i) {
        int engine = FindFormatter(allFiles.at(i).GetFilename());
        if (engine == cFormatEngineNone) {
            continue;
        }

        // TODO skip files based on size, 4.5MB as the default
        filesToFormat.push_back(allFiles.at(i).GetFilename());
    }

    BatchFormat(filesToFormat);
}

void CodeFormatter::BatchFormat(const std::vector<wxFileName>& files)
{
    if(files.empty()) {
        ::wxMessageBox(_("Project contains no files"));
        return;
    }

    wxString msg;
    msg << _("You are about to beautify ") << files.size() << _(" files\nContinue?");
    if(wxYES != ::wxMessageBox(msg, _("Source Code Formatter"), wxYES_NO | wxCANCEL | wxCENTER)) {
        return;
    }

    wxProgressDialog dlg(
        _("Source Code Formatter"), _("Formatting files..."), (int)files.size(), m_mgr->GetTheApp()->GetTopWindow());

    for(size_t i = 0; i < files.size(); ++i) {
        wxString msg;
        msg << "[ " << i << " / " << files.size() << " ] " << files.at(i).GetFullName();
        dlg.Update(i, msg);

        int engine = FindFormatter(files.at(i).GetFullPath());
        DoFormatFile(files.at(i).GetFullPath(), engine);
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
