//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : GitConsole.cpp
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

#include "GitConsole.h"

#include "AsyncProcess/clCommandProcessor.h"
#include "ColoursAndFontsManager.h"
#include "GitReleaseNotesGenerationDlg.h"
#include "GitResetDlg.h"
#include "MarkdownStyler.hpp"
#include "StdToWX.h"
#include "StringUtils.h"
#include "ai/LLMManager.hpp"
#include "ai/ProgressToken.hpp"
#include "ai/ResponseCollector.hpp"
#include "aui/clAuiToolBarArt.h"
#include "aui/cl_aui_tool_stickness.h"
#include "bitmap_loader.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clSTCHelper.hpp"
#include "clSideBarCtrl.hpp"
#include "clStrings.h"
#include "clTempFile.hpp"
#include "clToolBar.h"
#include "cl_config.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "git.h"
#include "gitentry.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "wxTerminalCtrl/wxTerminalOutputCtrl.hpp"

#include <algorithm>
#include <wx/datetime.h>
#include <wx/icon.h>
#include <wx/settings.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

#define GIT_MESSAGE(...) AddText(wxString::Format(__VA_ARGS__));
#define GIT_MESSAGE1(...)
#define GIT_ITEM_DATA(viewItem) reinterpret_cast<GitClientData*>(m_dvListCtrl->GetItemData(viewItem))

namespace
{
enum class eGitFile {
    kUnknown = -1,
    kNewFile,
    kDeletedFile,
    kModifiedFile,
    kUntrackedFile,
    kRenamedFile,
};

class GitClientData
{
    wxString m_path;
    eGitFile m_kind;

public:
    GitClientData(const wxString& path, eGitFile kind)
        : m_path(path)
        , m_kind(kind)
    {
    }
    virtual ~GitClientData() = default;

    const wxString& GetPath() const { return m_path; }
    eGitFile GetKind() const { return m_kind; }
};

wxVariant MakeFileBitmapLabel(const wxString& filename)
{
    BitmapLoader* bitmaps = clGetManager()->GetStdIcons();
    clDataViewTextBitmap tb(
        filename, bitmaps->GetMimeImageId(FileExtManager::GetType(filename, FileExtManager::TypeText)));
    wxVariant v;
    v << tb;
    return v;
}

struct ToolBarItem {
    wxString label;
    int id;
    wxString bmp;
};

struct GitFileEntry {
    wxString path;
    wxString fullname;
    wxString prefix; // entry type (modified, new, etc)

    GitFileEntry(const wxString& rawpath, const wxString& prefix, const wxString& repopath)
        : path(rawpath)
        , prefix(prefix)
    {
        fullname = rawpath.AfterLast('/');
    }
};

IEditor* CreateOrOpenFile(const wxString& path, bool first_time)
{
    // Open a temporary file for the release notes and load it into CodeLite.
    wxFileName fn{path};

    if (first_time) {
        if (!fn.FileExists()) {
            // Create
            fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            if (!FileUtils::WriteFileContent(fn, wxEmptyString)) {
                return nullptr;
            }
        }

        // OpenFile(..) will return the already opened file and it does not create a new one.
        auto editor = clGetManager()->OpenFile(fn.GetFullPath());
        CHECK_PTR_RET_NULL(editor);

        editor->GetCtrl()->SetReadOnly(true);
        editor->GetCtrl()->SetWrapMode(wxSTC_WRAP_WORD);
        return editor;
    } else {
        auto editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET_NULL(editor);

        if (editor->GetRemotePathOrLocal() != path) {
            return nullptr;
        }
        editor->GetCtrl()->SetWrapMode(wxSTC_WRAP_WORD);
        return editor;
    }
}

wxString GenerateRandomFile()
{
    clTempFile tmpfile{"txt"};
    return tmpfile.GetFullPath();
}

bool AppendTokenToFile(IEditor* editor, const std::string& token, bool is_done)
{
    editor->GetCtrl()->SetInsertionPointEnd();
    editor->GetCtrl()->SetReadOnly(false);
    editor->AppendText(wxString::FromUTF8(token));
    if (!is_done) {
        editor->GetCtrl()->SetReadOnly(true);
    }
    editor->GetCtrl()->ClearSelections();
    editor->GetCtrl()->EnsureCaretVisible();
    clSTCHelper::SetCaretAt(editor->GetCtrl(), editor->GetCtrl()->GetLastPosition());
    return true;
}
} // namespace

// ---------------------------------------------------------------------

GitConsole::GitConsole(wxWindow* parent, GitPlugin* git)
    : GitConsoleBase(parent)
    , m_git(git)
{
    // set the font to fit the C++ lexer default font
    m_bitmapLoader = clGetManager()->GetStdIcons();
    m_dvListCtrl->SetSortedColumn(1);
    m_dvListCtrl->SetRendererType(eRendererType::RENDERER_DIRECT2D);
    m_dvListCtrlUnversioned->SetSortedColumn(1);
    m_dvListCtrlUnversioned->SetRendererType(eRendererType::RENDERER_DIRECT2D);

    // Error messages will be coloured with red
    m_errorPatterns = {{"fatal:"},
                       {"error:"},
                       {"tell me who you are"},
                       {"hook failure"},
                       {"not a git repository"},
                       {"No commit message given, aborting"}};

    m_successPatterns =
        // Informative messages, will be coloured with green
        {{"up to date"}, {"up-to-date"}};

    // yellow
    m_warningPatterns = {{"the authenticity of host"}, {"can't be established"}, {"key fingerprint"}};

    m_modifiedBmp = m_bitmapLoader->LoadBitmap("modified");
    m_untrackedBmp = m_bitmapLoader->LoadBitmap("info");
    m_folderBmp = m_bitmapLoader->LoadBitmap("folder-yellow");
    m_newBmp = m_bitmapLoader->LoadBitmap("plus");
    m_deleteBmp = m_bitmapLoader->LoadBitmap("minus");

    EventNotifier::Get()->Connect(
        wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &GitConsole::OnWorkspaceClosed, this);
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::VerboseLog);

    // Toolbar
    auto images = clGetManager()->GetStdIcons();
    m_toolbar->SetArtProvider(new clAuiToolBarArt());
    m_toolbar->AddTool(XRCID("git_refresh"), _("Refresh"), images->LoadBitmap("file_reload"), _("Refresh"));
    m_toolbar->AddTool(XRCID("git_clear_log"), _("Clear Git Log"), images->LoadBitmap("clear"), _("Clear Git Log"));
    m_toolbar->AddTool(XRCID("git_stop_process"),
                       _("Terminate Git Process"),
                       images->LoadBitmap("execute_stop"),
                       _("Terminate Git Process"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_console_add_file"), _("Add File"), images->LoadBitmap("plus"), _("Add File"));
    m_toolbar->AddTool(XRCID("git_console_reset_file"), _("Reset File"), images->LoadBitmap("undo"), _("Reset File"));

    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_commit"), _("Commit"), images->LoadBitmap("git-commit"), _("Commit local changes"));
    m_toolbar->AddTool(XRCID("git_push"), _("Push"), images->LoadBitmap("up"), _("Push local changes"));

    m_toolbar->AddTool(XRCID("git_pull"), _("Pull"), images->LoadBitmap("pull"), _("Pull remote changes"));
    m_toolbar->SetToolDropDown(XRCID("git_pull"), true);
    m_toolbar->AddTool(XRCID("git_rebase"), _("Rebase"), images->LoadBitmap("merge"), _("Rebase"));
    m_toolbar->SetToolDropDown(XRCID("git_rebase"), true);

    m_toolbar->AddSeparator();
    m_toolbar->AddTool(
        XRCID("git_browse_commit_list"), _("Log"), images->LoadBitmap("tasks"), _("Browse commit history"));
    m_toolbar->AddTool(XRCID("git_reset_repository"), _("Reset repository"), images->LoadBitmap("clean"));

    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_ai_powered_features"), _("AI-Powered features"), images->LoadBitmap("wand"));
    m_toolbar->SetToolDropDown(XRCID("git_ai_powered_features"), true);

    // Bind the events
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnClearGitLog, this, XRCID("git_clear_log"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnClearGitLogUI, this, XRCID("git_clear_log"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnAddUnversionedFiles, this, XRCID("git_console_add_file"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnStopGitProcess, this, XRCID("git_stop_process"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnAddUnversionedFilesUI, this, XRCID("git_console_add_file"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnResetFileUI, this, XRCID("git_console_reset_file"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnStopGitProcessUI, this, XRCID("git_stop_process"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnAIAvailableUI, this, XRCID("git_ai_powered_features"));

    clAuiToolBarArt::Finalise(m_toolbar);

    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitPullDropdown, this, XRCID("git_pull"));
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitRebaseDropdown, this, XRCID("git_rebase"));
    m_toolbar->Bind(
        wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitAIDropDown, this, XRCID("git_ai_powered_features"));
    m_statusBar = new IndicatorPanel(this, _("Ready"));
    GetSizer()->Add(m_statusBar, wxSizerFlags(0).Expand());
    GetSizer()->Fit(this);

    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    m_dvListCtrlUnversioned->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, [this](clCommandEvent& event) {
        event.Skip();
        m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
        m_dvListCtrlUnversioned->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
        m_dvListCtrl->Refresh();
        m_dvListCtrlUnversioned->Refresh();
    });

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &GitConsole::OnSysColoursChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SIDEBAR_SELECTION_CHANGED, &GitConsole::OnOutputViewTabChanged, this);

    m_log_view = new wxTerminalOutputCtrl(m_panel_log);
    wxFont font{wxNullFont};
    auto lexer = ColoursAndFontsManager::Get().GetLexer("terminal");
    if (lexer) {
        auto font = lexer->GetFontForStyle(0, this);
        if (font.IsOk()) {
            font.SetFractionalPointSize(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFractionalPointSize());
            m_log_view->SetTextFont(font);
        }
    }
    m_panel_log->GetSizer()->Add(m_log_view, wxSizerFlags(1).Expand());

    // force font/colours update
    clCommandEvent dummy;
    OnSysColoursChanged(dummy);
}

GitConsole::~GitConsole()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &GitConsole::OnWorkspaceClosed, this);
    m_toolbar->Unbind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitPullDropdown, this, XRCID("git_pull"));
    m_toolbar->Unbind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitRebaseDropdown, this, XRCID("git_rebase"));
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &GitConsole::OnSysColoursChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_OUTPUT_VIEW_TAB_CHANGED, &GitConsole::OnOutputViewTabChanged, this);
}

void GitConsole::OnClearGitLog(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_log_view->Clear();
}

void GitConsole::OnStopGitProcess(wxCommandEvent& event)
{
    if (m_git->GetProcess()) {
        m_git->GetProcess()->Terminate();
    }

    if (m_git->GetFolderProcess()) {
        m_git->GetFolderProcess()->Terminate();
    }
}

void GitConsole::OnStopGitProcessUI(wxUpdateUIEvent& event)
{
    event.Enable(m_git->GetProcess() || m_git->GetFolderProcess());
}

void GitConsole::OnClearGitLogUI(wxUpdateUIEvent& event) { event.Enable(!m_log_view->IsEmpty()); }

bool GitConsole::IsPatternFound(const wxString& buffer, const std::unordered_set<wxString>& m) const
{
    wxString lc = buffer.Lower();
    for (const auto& s : m) {
        if (lc.Contains(s)) {
            return true;
        }
    }
    return false;
}

bool GitConsole::HasAnsiEscapeSequences(const wxString& buffer) const
{
    wxChar ch = 0x1B;
    return buffer.find(ch) != wxString::npos;
}

void GitConsole::AddText(const wxString& text)
{
    auto lines = ::wxStringTokenize(text, "\n", wxTOKEN_STRTOK);
    for (const auto& line : lines) {
        AddLine(line);
    }
}

bool GitConsole::IsVerbose() const { return m_isVerbose; }

void GitConsole::OnConfigurationChanged(wxCommandEvent& e)
{
    e.Skip();
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::VerboseLog);
}

void GitConsole::UpdateTreeView(const wxString& output)
{
    Clear();
    wxVector<wxVariant> cols;
    wxArrayString files = ::wxStringTokenize(output, "\n\r", wxTOKEN_STRTOK);

    // parse the output
    std::vector<GitFileEntry> lines;
    lines.reserve(files.size());

    for (wxString& filename : files) {

        filename.Trim().Trim(false);
        filename.Replace("\t", " ");
        filename.Replace("\\", "/");

        wxString prefix = filename.BeforeFirst(' ');
        filename = filename.AfterFirst(' ');

        filename.Trim().Trim(false);
        if (filename.EndsWith("/"))
            // a directory
            continue;

        GitFileEntry entry(filename, prefix, m_git->GetRepositoryPath());
        lines.push_back(entry);
    }

    auto sort_cb = [](const GitFileEntry& a, const GitFileEntry& b) {
        const wxString& fpa = a.path;
        const wxString& fpb = b.path;
        return fpa.CmpNoCase(fpb) < 0;
    };
    std::sort(lines.begin(), lines.end(), std::move(sort_cb));

    for (const auto& d : lines) {
        wxString filename = d.fullname;
        wxChar chX = d.prefix[0];

        wxBitmap statusBmp;
        eGitFile kind = eGitFile::kUntrackedFile;
        switch (chX) {
        case 'M':
            statusBmp = m_modifiedBmp;
            kind = eGitFile::kModifiedFile;
            break;
        case 'A':
            statusBmp = m_newBmp;
            kind = eGitFile::kNewFile;
            break;
        case 'D':
            statusBmp = m_deleteBmp;
            kind = eGitFile::kDeletedFile;
            break;
        case 'R':
            statusBmp = m_modifiedBmp;
            kind = eGitFile::kRenamedFile;
            break;
        default:
            statusBmp = m_untrackedBmp;
            kind = eGitFile::kUntrackedFile;
            break;
        }

        if (kind == eGitFile::kUntrackedFile) {
            // untracked
            cols.clear();
            cols.push_back(MakeFileBitmapLabel(d.path));
            m_dvListCtrlUnversioned->AppendItem(cols, (wxUIntPtr) new GitClientData(d.path, kind));
        } else {
            // modified
            cols.clear();
            cols.push_back(wxString() << chX);
            cols.push_back(MakeFileBitmapLabel(d.path));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new GitClientData(d.path, kind));
        }
    }
}

void GitConsole::OnContextMenu(wxDataViewEvent& event)
{
    bool hasSelection = (m_dvListCtrl->GetSelectedItemsCount() > 0);
    if (!hasSelection) {
        return;
    }

    wxMenu menu;
    menu.Append(XRCID("git_console_open_file"), _("Open File"));
    menu.AppendSeparator();
    menu.Append(XRCID("git_console_add_file"), _("Add unstaged file"));
    menu.AppendSeparator();
    menu.Append(XRCID("git_console_reset_file"), _("Reset file"));

    menu.Bind(wxEVT_MENU, &GitConsole::OnOpenFile, this, XRCID("git_console_open_file"));
    menu.Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    menu.Bind(wxEVT_MENU, &GitConsole::OnAddUnstagedFiles, this, XRCID("git_console_add_file"));
    m_dvListCtrl->PopupMenu(&menu);
}

void GitConsole::OnAddUnstagedFiles(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);

    wxArrayString files;
    files.reserve(items.GetCount());

    for (size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = GIT_ITEM_DATA(items.Item(i));
        if (gcd) {
            files.push_back(gcd->GetPath());
        }
    }

    if (files.empty()) {
        event.Skip();
        return;
    }

    // open the files
    for (const wxString& filename : files) {
        GIT_MESSAGE("Adding file: %s", filename);
    }
    m_git->AddFiles(files);
}

void GitConsole::OnResetFile(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);
    wxArrayString filesToRevert, filesToRemove;

    for (size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = GIT_ITEM_DATA(items.Item(i));
        if (gcd) {
            if (gcd->GetKind() == eGitFile::kNewFile) {
                filesToRemove.push_back(gcd->GetPath());

            } else if ((gcd->GetKind() == eGitFile::kModifiedFile) || (gcd->GetKind() == eGitFile::kRenamedFile) ||
                       (gcd->GetKind() == eGitFile::kDeletedFile)) {
                filesToRevert.push_back(gcd->GetPath());
            }
        }
    }
    if (filesToRevert.IsEmpty() && filesToRemove.IsEmpty()) {
        return;
    }

    GitResetDlg dlg(EventNotifier::Get()->TopFrame(), filesToRevert, filesToRemove);
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    filesToRevert = dlg.GetItemsToRevert();
    filesToRemove = dlg.GetItemsToRemove();

    if (!filesToRevert.IsEmpty()) {
        m_git->ResetFiles(filesToRevert);
    }

    if (!filesToRemove.IsEmpty()) {
        m_git->UndoAddFiles(filesToRemove);
    }
}

void GitConsole::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    Clear();

    wxCommandEvent dummy;
    OnClearGitLog(dummy);
}

void GitConsole::OnItemSelectedUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelectedItemsCount()); }

void GitConsole::OnFileActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    GitClientData* gcd = GIT_ITEM_DATA(event.GetItem());
    CHECK_PTR_RET(gcd);

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    wxString difftool = data.GetDifftool();
    if (difftool.empty()) {
        const wxArrayString wx_options =
            StdToWX::ToArrayString({"built-in", "vimdiff", "vimdiff1", "vimdiff2", "vimdiff3", "winmerge"});
        difftool = ::wxGetSingleChoice(_("Choose a tool to use:"), "CodeLite", wx_options, 0);
        if (difftool.empty()) {
            // user hit cancel
            return;
        }

        data.SetDifftool(difftool);
        data.Save();

        wxString message;
        message << _("Your diff tool is set to: ") << difftool << "\n"
                << _("You can change this from the menu bar: Plugins > Git > GIT plugin settings > Tools");
        ::wxMessageBox(message);
    }

    if (difftool == "built-in") {
        wxArrayString files;
        files.push_back(gcd->GetPath());
        m_git->ShowDiff(files);
    } else {
        m_git->ShowExternalDiff(gcd->GetPath(), difftool);
    }
}

void GitConsole::OnOpenFile(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);
    wxArrayString files;
    for (size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = GIT_ITEM_DATA(items.Item(i));
        if (gcd) {
            files.push_back(gcd->GetPath());
        }
    }

    if (files.IsEmpty()) {
        e.Skip();
        return;
    }

    // open the files
    for (size_t i = 0; i < files.GetCount(); ++i) {
        GIT_MESSAGE("Opening file: %s", files.Item(i).c_str());
        m_git->OpenFile(files.Item(i));
    }
}

void GitConsole::OnApplyPatch(wxCommandEvent& event) { wxPostEvent(m_git, event); }

struct GitCommandData : public wxObject {
    GitCommandData(const wxArrayString a, const wxString n, int i)
        : arr(a)
        , name(n)
        , id(i)
    {
    }
    wxArrayString arr; // Holds the possible command-strings
    wxString name;     // Holds the name of the command e.g. "git_pull"
    int id;            // Holds the id of the command e.g. XRCID("git_pull")
};

void GitConsole::OnGitAIDropDown(wxAuiToolBarEvent& event)
{
    if (event.IsDropDownClicked()) {
        wxMenu menu;
        menu.Append(XRCID("git_ai_generate_release_notes"), _("Generate Release Notes..."));
        menu.Append(XRCID("git_ai_code_review"), _("Do Code Review..."));

        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& e) {
                wxUnusedVar(e);
                CallAfter(&GitConsole::GenerateReleaseNotes);
            },
            XRCID("git_ai_generate_release_notes"));
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& e) {
                wxUnusedVar(e);
                CallAfter(&GitConsole::DoCodeReview);
            },
            XRCID("git_ai_code_review"));

        clAuiToolStickness stickness{m_toolbar, event.GetId()};
        // line up our menu with the button
        wxRect rect = m_toolbar->GetToolRect(event.GetId());
        wxPoint pt = m_toolbar->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);

        PopupMenu(&menu, pt);
    }
}

void GitConsole::DoOnDropdown(const wxString& commandName, int id, const wxAuiToolBarEvent& event)
{
    if (event.IsDropDownClicked()) {
        GitEntry data;
        {
            clConfig conf("git.conf");
            conf.ReadItem(&data);
            // Force conf out of scope, else its dtor clobbers the GitConsole::OnDropDownMenuEvent Save()
        }

        GitCommandsEntries& ce = data.GetGitCommandsEntries(commandName);
        vGitLabelCommands_t entries = ce.GetCommands();
        int lastUsed = ce.GetLastUsedCommandIndex();

        wxArrayString arr;
        wxMenu menu;
        for (size_t n = 0; n < entries.size(); ++n) {
            wxMenuItem* item = menu.AppendRadioItem(n, entries.at(n).label);
            item->Check(n == (size_t)lastUsed);
            arr.Add(entries.at(n).command);
        }
        menu.Bind(wxEVT_MENU,
                  wxCommandEventHandler(GitConsole::OnDropDownMenuEvent),
                  this,
                  0,
                  arr.GetCount(),
                  new GitCommandData(arr, commandName, id));

        m_toolbar->SetToolSticky(id, true);

        // line up our menu with the button
        wxRect rect = m_toolbar->GetToolRect(id);
        wxPoint pt = m_toolbar->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);

        PopupMenu(&menu, pt);

        // make sure the button is "un-stuck"
        m_toolbar->SetToolSticky(id, false);

        menu.Unbind(wxEVT_MENU,
                    wxCommandEventHandler(GitConsole::OnDropDownMenuEvent),
                    this,
                    0,
                    arr.GetCount(),
                    new GitCommandData(arr, commandName, id));
    } else {
        // button clicked.
        if (event.GetId() == XRCID("git_pull")) {
            // do pull
            wxCommandEvent e;
            m_git->OnPull(e);
        } else if (event.GetId() == XRCID("git_rebase")) {
            // do rebase
            wxCommandEvent e;
            m_git->OnRebase(e);
        }
    }
}

void GitConsole::OnDropDownMenuEvent(wxCommandEvent& event)
{
    int id = event.GetId();
    GitCommandData* userdata = static_cast<GitCommandData*>(event.GetEventUserData());
    wxCHECK_RET((int)userdata->arr.GetCount() > event.GetId(), "Out-of-range ID");
    event.SetString(userdata->arr.Item(id));
    event.SetId(userdata->id);

    wxPostEvent(m_git, event); // We've now populated the event object with useful data, so get GitPlugin to process it

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    GitCommandsEntries& ce = data.GetGitCommandsEntries(userdata->name);
    ce.SetLastUsedCommandIndex(id);
    conf.WriteItem(&data);
    conf.Save();
}

void GitConsole::HideProgress() { m_statusBar->Stop(_("Ready")); }

void GitConsole::ShowProgress(const wxString& message) { m_statusBar->Start(message); }

void GitConsole::UpdateProgress([[maybe_unused]] unsigned long current, const wxString& message)
{
    m_statusBar->SetMessage(message);
}

bool GitConsole::IsProgressShown() const { return m_statusBar->IsRunning(); }

bool GitConsole::IsDirty() const { return (m_dvListCtrl->GetItemCount() > 0); }

void GitConsole::OnStclogStcChange(wxStyledTextEvent& event) { event.Skip(); }

void GitConsole::Clear()
{
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
        GitClientData* cd = reinterpret_cast<GitClientData*>(d);
        if (cd) {
            wxDELETE(cd);
        }
    });

    m_dvListCtrlUnversioned->DeleteAllItems([](wxUIntPtr d) {
        GitClientData* cd = reinterpret_cast<GitClientData*>(d);
        if (cd) {
            wxDELETE(cd);
        }
    });
}

void GitConsole::OnUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_git->IsGitEnabled()); }

void GitConsole::OnUnversionedFileActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    GitClientData* cd = reinterpret_cast<GitClientData*>(m_dvListCtrlUnversioned->GetItemData(event.GetItem()));
    CHECK_PTR_RET(cd);
    clGetManager()->OpenFile(cd->GetPath());
}

void GitConsole::OnUnversionedFileContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_OPEN);
    menu.Append(wxID_ADD);
    menu.Bind(wxEVT_MENU, &GitConsole::OnOpenUnversionedFiles, this, wxID_OPEN);
    menu.Bind(wxEVT_MENU, &GitConsole::OnAddUnversionedFiles, this, wxID_ADD);
    m_dvListCtrlUnversioned->PopupMenu(&menu);
}

wxArrayString GitConsole::GetSelectedModifiedFiles() const
{
    if (m_dvListCtrl->GetSelectedItemsCount() == 0) {
        return {};
    }

    wxArrayString paths;
    wxDataViewItemArray items;
    int count = m_dvListCtrl->GetSelections(items);
    paths.reserve(count);
    for (int i = 0; i < count; i++) {
        wxDataViewItem item = items.Item(i);
        if (item.IsOk() == false) {
            continue;
        }

        GitClientData* cd = reinterpret_cast<GitClientData*>(m_dvListCtrl->GetItemData(item));
        if (cd && cd->GetKind() == eGitFile::kModifiedFile) {
            paths.Add(cd->GetPath());
        }
    }
    return paths;
}

wxArrayString GitConsole::GetSelectedUnversionedFiles() const
{
    if (m_dvListCtrlUnversioned->GetSelectedItemsCount() == 0) {
        return wxArrayString();
    }
    wxArrayString paths;
    wxDataViewItemArray items;
    int count = m_dvListCtrlUnversioned->GetSelections(items);
    paths.reserve(count);
    for (int i = 0; i < count; i++) {
        wxDataViewItem item = items.Item(i);
        if (item.IsOk() == false) {
            continue;
        }

        GitClientData* cd = reinterpret_cast<GitClientData*>(m_dvListCtrlUnversioned->GetItemData(item));
        if (cd && cd->GetKind() == eGitFile::kUntrackedFile) {
            paths.Add(cd->GetPath());
        }
    }
    return paths;
}

void GitConsole::OnOpenUnversionedFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxArrayString paths = GetSelectedUnversionedFiles();
    if (paths.IsEmpty()) {
        return;
    }

    for (const wxString& filepath : paths) {
        if (!filepath.EndsWith("/")) {
            m_git->OpenFile(filepath);
        }
    }
}

void GitConsole::OnAddUnversionedFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_git->AddFiles(GetSelectedUnversionedFiles());
}

void GitConsole::ShowLog()
{
    // Change the selection to the "Log" view
    // m_notebookLog->SetSelection(m_notebookLog->GetPageIndex(_("Log")));
}

wxString GitConsole::GetPrompt() const
{
    wxString prompt_str = m_git->GetRepositoryPath();
#ifndef __WXMSW__
    wxString home_dir = ::wxGetHomeDir();
    if (prompt_str.StartsWith(home_dir)) {
        prompt_str.Replace(home_dir, "~", false);
    }
#endif
    return prompt_str;
}

void GitConsole::AddLine(const wxString& line)
{
    wxString tmp = line;
    bool text_ends_with_cr = line.EndsWith("\r");
    tmp.Replace("\r", wxEmptyString);
    tmp.Trim().Append("\n");

    wxStringView sv{tmp.c_str(), tmp.length()};
    m_log_view->StyleAndAppend(sv, nullptr);
}

void GitConsole::PrintPrompt() { AddLine(GetPrompt() + "\n"); }

void GitConsole::OnSysColoursChanged(clCommandEvent& event) { event.Skip(); }

void GitConsole::OnAddUnversionedFilesUI(wxUpdateUIEvent& event)
{
    bool has_unversioned_files = !GetSelectedUnversionedFiles().empty();
    event.Enable(m_git->IsGitEnabled() && has_unversioned_files);
}

void GitConsole::OnResetFileUI(wxUpdateUIEvent& event)
{
    bool has_modified_files = !GetSelectedModifiedFiles().empty();
    event.Enable(m_git->IsGitEnabled() && has_modified_files);
}

void GitConsole::OnOutputViewTabChanged(clCommandEvent& event)
{
    event.Skip();
    // Avoid auto refreshing the view on a remote workspace, this could lead to non responsive UI
    if (m_git->m_isRemoteWorkspace || !clGetManager()->IsPaneShown(PANE_OUTPUT, GIT_TAB_NAME)) {
        return;
    }
    if (m_git && event.GetString() == GIT_TAB_NAME) {
        m_git->DoRefreshView(false);
    }
}

void GitConsole::OnAIAvailableUI(wxUpdateUIEvent& event)
{
    event.Enable(m_git->IsGitEnabled() && llm::Manager::GetInstance().IsAvailable() &&
                 !llm::Manager::GetInstance().IsBusy());
}

void GitConsole::DoCodeReview()
{
    wxString prompt = llm::Manager::GetInstance().GetConfig().GetPrompt(llm::PromptKind::kGitChangesCodeReview);

    wxString output;
    if (!m_git->DoExecuteCommandSync("diff --ignore-all-space", &output, m_git->GetRepositoryPath())) {
        wxMessageBox(_("Failed to fetch git diff!"), "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
        return;
    }

    prompt.Replace("{{context}}", output);

    // Construct a token for cancellation purposes.
    std::shared_ptr<llm::CancellationToken> cancellation_token = std::make_shared<llm::CancellationToken>(10000);
    auto collector = new llm::ResponseCollector();
    m_statusBar->Start(_("Generating Code Review..."));

    std::shared_ptr<bool> first_token = std::make_shared<bool>(true);

    wxString review_file = GenerateRandomFile();
    collector->SetStreamCallback(
        [=, this](const std::string& message, bool is_done, [[maybe_unused]] bool is_thinking) {
            if (cancellation_token->IsMaxTokenReached()) {
                m_statusBar->Stop(message);
                return;
            }

            if (!message.empty()) {
                auto editor = CreateOrOpenFile(review_file, *first_token);
                *first_token = false;
                CHECK_COND_RET(editor);
                AppendTokenToFile(editor, message, is_done);
            }

            if (is_done) {
                m_statusBar->Stop("Ready");
                auto editor = CreateOrOpenFile(review_file, *first_token);
                if (editor) {
                    editor->GetCtrl()->SetSavePoint();
                    MarkdownStyler styler(editor->GetCtrl());
                    styler.StyleText(true);
                }
            }
        });

    auto function_disabler = std::make_shared<llm::FunctionsDisabler>();
    auto state_change_cb = [this, function_disabler](llm::ChatState state) {
        if (!wxThread::IsMain()) {
            clWARNING() << "StateChangingCB called for non main thread!" << endl;
            return;
        }
        switch (state) {
        case llm::ChatState::kThinking:
            m_statusBar->SetMessage(_("Thinking..."));
            break;
        case llm::ChatState::kWorking:
            m_statusBar->SetMessage(_("Working..."));
            break;
        case llm::ChatState::kReady:
            // Re-enable all functions again.
            m_statusBar->Stop(_("Ready."));
            break;
        }
    };
    collector->SetStateChangingCB(std::move(state_change_cb));

    llm::ChatOptions chat_options{llm::ChatOptions::kNoHistory};
    llm::Manager::GetInstance().EnableFunctionByName("Read_file_from_the_file_system", true);
    llm::Manager::GetInstance().Chat(collector, prompt, cancellation_token, chat_options);
}

void GitConsole::GenerateReleaseNotes()
{
    // We need 2 commits to fetch the diff.
    GitReleaseNotesGenerationDlg dlg{EventNotifier::Get()->TopFrame()};
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString first_commit = dlg.GetTextCtrlFirstCommit()->GetValue();
    wxString second_commit = dlg.GetTextCtrlSecondCommit()->GetValue();

    int max_tokens = dlg.GetSpinCtrlLimitTokens()->GetValue();
    bool oneline_commit_format = dlg.GetCheckBoxOneLine()->GetValue();

    constexpr size_t kChunkSize = 8 * 1024;
    wxArrayString history;

    {
        IndicatorPanelLocker lk{m_statusBar, _("Fetching git log..."), _("Ready")};
        auto res = m_git->FetchLogBetweenCommits(first_commit, second_commit, oneline_commit_format, kChunkSize);
        if (!res.ok()) {
            wxMessageBox(wxString() << _("Failed to fetch git log.\n") << res.error_message(),
                         "CodeLite",
                         wxICON_ERROR | wxOK | wxCENTRE);
            return;
        }
        history = res.value();
    }

    if (history.empty()) {
        return;
    }

    wxString prompt_template =
        llm::Manager::GetInstance().GetConfig().GetPrompt(llm::PromptKind::kReleaseNotesGenerate);

    // Construct a token for cancellation purposes.
    std::shared_ptr<llm::CancellationToken> cancellation_token = std::make_shared<llm::CancellationToken>(max_tokens);
    auto collector = new llm::ResponseCollector();

    m_statusBar->Start(_("Generating release notes..."));
    bool multiple_prompts = history.size() > 1;

    collector->SetStateChangingCB([this](llm::ChatState state) {
        if (!wxThread::IsMain()) {
            clWARNING() << "StateChangingCB called for non main thread!" << endl;
            return;
        }
        switch (state) {
        case llm::ChatState::kThinking:
            m_statusBar->SetMessage(_("Thinking..."));
            break;
        case llm::ChatState::kWorking:
            m_statusBar->SetMessage(_("Working..."));
            break;
        case llm::ChatState::kReady:
            m_statusBar->Stop(_("Ready."));
            break;
        }
    });

    std::shared_ptr<std::string> complete_response = std::make_shared<std::string>();
    std::shared_ptr<bool> first_token = std::make_shared<bool>(true);

    wxString release_notes_file = GenerateRandomFile();
    collector->SetStreamCallback(
        [=, this](const std::string& message, bool is_done, [[maybe_unused]] bool is_thinking) {
            if (cancellation_token->IsMaxTokenReached()) {
                m_statusBar->Stop(message);
                return;
            }

            if (!message.empty()) {
                if (!multiple_prompts) {
                    auto editor = CreateOrOpenFile(release_notes_file, *first_token);
                    *first_token = false;
                    CHECK_COND_RET(editor);
                    AppendTokenToFile(editor, message, is_done);
                } else {
                    complete_response->append(message);
                }
            }

            if (is_done) {
                if (multiple_prompts) {
                    CallAfter(&GitConsole::FinaliseReleaseNotes, wxString::FromUTF8(*complete_response));
                }
                m_statusBar->Stop("Ready");

                auto editor = CreateOrOpenFile(release_notes_file, *first_token);
                if (editor) {
                    editor->GetCtrl()->SetSavePoint();
                    MarkdownStyler styler(editor->GetCtrl());
                    styler.StyleText(true);
                }
            }
        });

    llm::ChatOptions chat_options{llm::ChatOptions::kNoTools};
    llm::AddFlagSet(chat_options, llm::ChatOptions::kNoHistory);

    //  Prepare the prompts
    wxArrayString prompts;
    for (const auto& h : history) {
        wxString p = prompt_template;
        p.Replace("{{context}}", h);
        prompts.Add(p);
    }
    llm::Manager::GetInstance().Chat(collector, prompts, cancellation_token, chat_options);
}

void GitConsole::FinaliseReleaseNotes(const wxString& complete_reponse)
{
    wxString prompt = llm::Manager::GetInstance().GetConfig().GetPrompt(llm::PromptKind::kReleaseNotesMerge);
    prompt.Replace("{{context}}", complete_reponse);

    m_statusBar->SetMessage(_("Finalising notes..."));
    auto collector = new llm::ResponseCollector();

    // Open a temporary file for the release notes and load it into CodeLite.
    auto editor = CreateOrOpenFile(GenerateRandomFile(), true);
    CHECK_COND_RET(editor);
    wxString release_notes_file = editor->GetRemotePathOrLocal();

    collector->SetStreamCallback(
        [=, this](const std::string& message, bool is_done, [[maybe_unused]] bool is_thinking) {
            auto editor = CreateOrOpenFile(release_notes_file, false);
            CHECK_PTR_RET(editor);
            if (!AppendTokenToFile(editor, message, is_done)) {
                return;
            }

            if (is_done) {
                m_statusBar->Stop("Ready");
                editor->GetCtrl()->SetSavePoint();
                MarkdownStyler styler(editor->GetCtrl());
                styler.StyleText(true);
            }
        });

    llm::ChatOptions chat_options{llm::ChatOptions::kDefault};
    llm::AddFlagSet(chat_options, llm::ChatOptions::kNoTools);
    llm::AddFlagSet(chat_options, llm::ChatOptions::kNoHistory);
    llm::Manager::GetInstance().Chat(collector, prompt, nullptr, chat_options);
}
