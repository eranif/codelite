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
#include "GitResetDlg.h"
#include "StdToWX.h"
#include "StringUtils.h"
#include "aui/clAuiToolBarArt.h"
#include "bitmap_loader.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clSideBarCtrl.hpp"
#include "clStrings.h"
#include "clToolBar.h"
#include "cl_aui_tool_stickness.h"
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
    virtual ~GitClientData() {}

    const wxString& GetPath() const { return m_path; }
    eGitFile GetKind() const { return m_kind; }
};

wxVariant MakeFileBitmapLabel(const wxString& filename)
{
    BitmapLoader* bitmaps = clGetManager()->GetStdIcons();
    clDataViewTextBitmap tb(filename,
                            bitmaps->GetMimeImageId(FileExtManager::GetType(filename, FileExtManager::TypeText)));
    wxVariant v;
    v << tb;
    return v;
}

struct ToolBarItem {
    wxString label;
    int id;
    wxString bmp;
};
// ---------------------------------------------------------------------
void PopulateToolbarOverflow(wxAuiToolBar* toolbar)
{
    std::vector<ToolBarItem> items = {
        {wxTRANSLATE("Create local branch"), XRCID("git_create_branch"), "file_new"},
        {wxTRANSLATE("Switch to local branch"), XRCID("git_switch_branch"), "split"},
        {wxTRANSLATE("Switch to remote branch"), XRCID("git_switch_to_remote_branch"), "remote-folder"},
        {wxEmptyString, wxID_SEPARATOR, wxEmptyString},
        {wxTRANSLATE("Apply Patch"), XRCID("git_apply_patch"), "patch"},
        {wxEmptyString, wxID_SEPARATOR, wxEmptyString},
        {wxTRANSLATE("Start gitk"), XRCID("git_start_gitk"), "debugger_start"},
        {wxTRANSLATE("Garbage collect"), XRCID("git_garbage_collection"), "clean"},
        {wxEmptyString, wxID_SEPARATOR, wxEmptyString},
        {wxTRANSLATE("Plugin settings"), XRCID("git_settings"), "cog"},
        {wxTRANSLATE("Clone a git repository"), XRCID("git_clone"), "copy"}};

    auto images = clGetManager()->GetStdIcons();
    for (auto item : items) {
        if (item.id == wxID_SEPARATOR) {
            toolbar->AddSeparator();
        } else {
            toolbar->AddTool(item.id, wxGetTranslation(item.label), images->LoadBitmap(item.bmp));
        }
    }
}

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

    m_toolbar->AddTool(
        XRCID("git_browse_commit_list"), _("Log"), images->LoadBitmap("tasks"), _("Browse commit history"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_rebase"), _("Rebase"), images->LoadBitmap("merge"), _("Rebase"));
    m_toolbar->SetToolDropDown(XRCID("git_rebase"), true);

    m_toolbar->AddTool(XRCID("git_reset_repository"), _("Reset"), images->LoadBitmap("clean"), _("Reset repository"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_commit_diff"), _("Diffs"), images->LoadBitmap("diff"), _("Show current diffs"));
    m_toolbar->AddTool(XRCID("git_blame"), _("Blame"), images->LoadBitmap("finger"), _("Git blame"));

#ifdef __WXMSW__
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_msysgit"),
                       _("Open MSYS Git"),
                       images->LoadBitmap("console"),
                       _("Open MSYS Git at the current file location"));
#endif

    // Bind the events
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnClearGitLog, this, XRCID("git_clear_log"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnClearGitLogUI, this, XRCID("git_clear_log"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnAddUnversionedFiles, this, XRCID("git_console_add_file"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnStopGitProcess, this, XRCID("git_stop_process"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnAddUnversionedFilesUI, this, XRCID("git_console_add_file"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnResetFileUI, this, XRCID("git_console_reset_file"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnStopGitProcessUI, this, XRCID("git_stop_process"));

    PopulateToolbarOverflow(m_toolbar);

    for (size_t i = 0; i < m_toolbar->GetToolCount(); ++i) {
        auto tool = m_toolbar->FindToolByIndex(i);
        if (tool->GetId() == wxID_SEPARATOR) {
            continue;
        }

        auto normal_bmp = tool->GetBitmap();
        wxBitmap disable_bmp = normal_bmp;
        if (DrawingUtils::IsThemeDark()) {
            disable_bmp = disable_bmp.ConvertToDisabled(0);
        } else {
            disable_bmp = disable_bmp.ConvertToDisabled(255);
        }
        tool->SetDisabledBitmap(disable_bmp);
    }

    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitPullDropdown, this, XRCID("git_pull"));
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &GitConsole::OnGitRebaseDropdown, this, XRCID("git_rebase"));
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
    menu.Append(XRCID("git_console_reset_file"), _("Reset file"));

    menu.Bind(wxEVT_MENU, &GitConsole::OnOpenFile, this, XRCID("git_console_open_file"));
    menu.Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    m_dvListCtrl->PopupMenu(&menu);
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
