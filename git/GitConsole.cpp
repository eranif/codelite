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
#include "GitResetDlg.h"
#include "bitmap_loader.h"
#include "clBitmap.h"
#include "clCommandProcessor.h"
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
#include <algorithm>
#include <wx/datetime.h>
#include <wx/icon.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#include "clThemeUpdater.h"

#define GIT_MESSAGE(...) AddText(wxString::Format(__VA_ARGS__));
#define GIT_MESSAGE1(...)

#define GIT_ITEM_DATA(viewItem) reinterpret_cast<GitClientData*>(m_dvListCtrl->GetItemData(viewItem))

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

    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetPath() const { return m_path; }
    eGitFile GetKind() const { return m_kind; }
};

static wxVariant MakeFileBitmapLabel(const wxString& filename)
{
    BitmapLoader* bitmaps = clGetManager()->GetStdIcons();
    clDataViewTextBitmap tb(filename,
                            bitmaps->GetMimeImageId(FileExtManager::GetType(filename, FileExtManager::TypeText)));
    wxVariant v;
    v << tb;
    return v;
}

// ---------------------------------------------------------------------
void PopulateToolbarOverflow(clToolBar* toolbar)
{
    static const char* labels[] = { wxTRANSLATE("Create local branch"),
                                    wxTRANSLATE("Switch to local branch"),
                                    wxTRANSLATE("Switch to remote branch"),
                                    "",
                                    wxTRANSLATE("Refresh"),
                                    wxTRANSLATE("Apply Patch"),
                                    "",
                                    wxTRANSLATE("Start gitk"),
                                    wxTRANSLATE("Garbage collect"),
                                    "",
                                    wxTRANSLATE("Plugin settings"),
                                    wxTRANSLATE("Set repository path"),
                                    wxTRANSLATE("Clone a git repository") };
    static const char* bitmapnames[] = { "file_new", "split",          "remote-folder", "", "file_reload", "patch",
                                         "",         "debugger_start", "clean",         "", "cog",         "folder",
                                         "copy" };
    static const int IDs[] = { XRCID("git_create_branch"),
                               XRCID("git_switch_branch"),
                               XRCID("git_switch_to_remote_branch"),
                               0,
                               XRCID("git_refresh"),
                               XRCID("git_apply_patch"),
                               0,
                               XRCID("git_start_gitk"),
                               XRCID("git_garbage_collection"),
                               0,
                               XRCID("git_settings"),
                               XRCID("git_set_repository"),
                               XRCID("git_clone") };
    size_t IDsize = sizeof(IDs) / sizeof(int);
    wxCHECK_RET(sizeof(labels) / sizeof(char*) == IDsize, "Mismatched arrays");
    wxCHECK_RET(sizeof(bitmapnames) / sizeof(char*) == IDsize, "Mismatched arrays");

    wxAuiToolBarItem item, separator;
    item.SetKind(wxITEM_NORMAL);
    separator.SetKind(wxITEM_SEPARATOR);

    for(size_t n = 0; n < IDsize; ++n) {
        if(IDs[n] != 0) {
            toolbar->AddTool(IDs[n], labels[n], clGetManager()->GetStdIcons()->LoadBitmap(bitmapnames[n]));
        } else {
            toolbar->AddSeparator();
        }
    }
}

// ---------------------------------------------------------------------

GitConsole::GitConsole(wxWindow* parent, GitPlugin* git)
    : GitConsoleBase(parent)
    , m_git(git)
{
    // set the font to fit the C++ lexer default font
    m_styler.reset(new clGenericSTCStyler(m_stcLog));
    m_bitmapLoader = clGetManager()->GetStdIcons();
    
    clThemeUpdater::Get().RegisterWindow(m_splitter733);
    // Build the styles for git output

    // Error messages will be coloured with red
    {
        wxArrayString words;
        words.Add("fatal:");
        words.Add("error:");
        words.Add("tell me who you are");
        words.Add("hook failure");
        words.Add("not a git repository");
        words.Add("No commit message given, aborting");
        m_styler->AddStyle(words, clGenericSTCStyler::kError);
    }

    {
        // Informative messages, will be coloured with green
        wxArrayString words;
        words.Add("up to date");
        words.Add("up-to-date");
        m_styler->AddStyle(words, clGenericSTCStyler::kInfo);
    }
    {
        // warning messages
        wxArrayString words;
        words.Add("the authenticity of host");
        words.Add("can't be established");
        words.Add("key fingerprint");
        m_styler->AddStyle(words, clGenericSTCStyler::kWarning);
    }
    m_styler->ApplyStyles();

    m_modifiedBmp = m_bitmapLoader->LoadBitmap("modified");
    m_untrackedBmp = m_bitmapLoader->LoadBitmap("info");
    m_folderBmp = m_bitmapLoader->LoadBitmap("folder-yellow");
    m_newBmp = m_bitmapLoader->LoadBitmap("plus");
    m_deleteBmp = m_bitmapLoader->LoadBitmap("minus");

    EventNotifier::Get()->Connect(wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(GitConsole::OnWorkspaceClosed), NULL,
                                  this);
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);

    // Toolbar
    m_toolbar->AddTool(XRCID("git_clear_log"), _("Clear Git Log"), m_bitmapLoader->LoadBitmap("clear"),
                       _("Clear Git Log"));
    m_toolbar->AddTool(XRCID("git_stop_process"), _("Terminate Git Process"),
                       m_bitmapLoader->LoadBitmap("execute_stop"), _("Terminate Git Process"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_console_add_file"), _("Add File"), m_bitmapLoader->LoadBitmap("plus"), _("Add File"));
    m_toolbar->AddTool(XRCID("git_console_reset_file"), _("Reset File"), m_bitmapLoader->LoadBitmap("undo"),
                       _("Reset File"));

    m_toolbar->AddTool(XRCID("git_reset_repository"), _("Reset"), m_bitmapLoader->LoadBitmap("clean"),
                       _("Reset repository"));
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(XRCID("git_pull"), _("Pull"), m_bitmapLoader->LoadBitmap("pull"), _("Pull remote changes"),
                       wxITEM_DROPDOWN);

    m_toolbar->AddTool(XRCID("git_commit"), _("Commit"), m_bitmapLoader->LoadBitmap("git-commit"),
                       _("Commit local changes"));
    m_toolbar->AddTool(XRCID("git_push"), _("Push"), m_bitmapLoader->LoadBitmap("up"), _("Push local changes"));
    m_toolbar->AddTool(XRCID("git_rebase"), _("Rebase"), m_bitmapLoader->LoadBitmap("merge"), _("Rebase"),
                       wxITEM_DROPDOWN);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_commit_diff"), _("Diffs"), m_bitmapLoader->LoadBitmap("diff"),
                       _("Show current diffs"));
    m_toolbar->AddTool(XRCID("git_browse_commit_list"), _("Log"), m_bitmapLoader->LoadBitmap("tasks"),
                       _("Browse commit history"));
    m_toolbar->AddTool(XRCID("git_blame"), _("Blame"), m_bitmapLoader->LoadBitmap("finger"), _("Git blame"));

#ifdef __WXMSW__
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_msysgit"), _("Open MSYS Git"), m_bitmapLoader->LoadBitmap("console"),
                       _("Open MSYS Git at the current file location"));
#endif

    // Bind the events
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnClearGitLog, this, XRCID("git_clear_log"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnClearGitLogUI, this, XRCID("git_clear_log"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    m_toolbar->Bind(wxEVT_MENU, &GitConsole::OnStopGitProcess, this, XRCID("git_stop_process"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &GitConsole::OnStopGitProcessUI, this, XRCID("git_stop_process"));

    PopulateToolbarOverflow(m_toolbar);
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &GitConsole::OnGitPullDropdown, this, XRCID("git_pull"));
    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &GitConsole::OnGitRebaseDropdown, this, XRCID("git_rebase"));
    m_stcLog->SetWrapMode(wxSTC_WRAP_WORD);
    m_gauge->Hide();
    GetSizer()->Fit(this);

    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    m_dvListCtrlUnversioned->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
}

GitConsole::~GitConsole()
{
    clThemeUpdater::Get().UnRegisterWindow(m_splitter733);
    EventNotifier::Get()->Disconnect(wxEVT_GIT_CONFIG_CHANGED,
                                     wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(GitConsole::OnWorkspaceClosed), NULL,
                                     this);
    m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &GitConsole::OnGitPullDropdown, this, XRCID("git_pull"));
    m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &GitConsole::OnGitRebaseDropdown, this, XRCID("git_rebase"));
}

void GitConsole::OnClearGitLog(wxCommandEvent& event) { m_stcLog->ClearAll(); }

void GitConsole::OnStopGitProcess(wxCommandEvent& event)
{
    if(m_git->GetProcess()) { m_git->GetProcess()->Terminate(); }

    if(m_git->GetFolderProcess()) { m_git->GetFolderProcess()->Terminate(); }
}

void GitConsole::OnStopGitProcessUI(wxUpdateUIEvent& event)
{
    event.Enable(m_git->GetProcess() || m_git->GetFolderProcess());
}

void GitConsole::OnClearGitLogUI(wxUpdateUIEvent& event) { event.Enable(!m_stcLog->IsEmpty()); }

void GitConsole::AddText(const wxString& text)
{
    m_stcLog->SetInsertionPoint(m_stcLog->GetLength());
#ifdef __WXMSW__
    wxString tmp = text;
    tmp.Replace("\r\n", "\n");
    m_stcLog->AddText(tmp);
    if(!tmp.EndsWith("\n")) { m_stcLog->AddText("\n"); }
#else
    m_stcLog->AddText(text);
    if(!text.EndsWith("\n")) { m_stcLog->AddText("\n"); }
#endif
    m_stcLog->ScrollToEnd();
}

void GitConsole::AddRawText(const wxString& text)
{
    // Add text without manipulate its content
    m_stcLog->SetInsertionPoint(m_stcLog->GetLength());
    m_stcLog->AddText(text);
    m_stcLog->ScrollToEnd();
}

bool GitConsole::IsVerbose() const { return m_isVerbose; }

void GitConsole::OnConfigurationChanged(wxCommandEvent& e)
{
    e.Skip();
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);
}

void GitConsole::UpdateTreeView(const wxString& output)
{
    Clear();
    wxVector<wxVariant> cols;
    wxArrayString files = ::wxStringTokenize(output, "\n\r", wxTOKEN_STRTOK);
    std::sort(files.begin(), files.end());

    BitmapLoader* bitmaps = clGetManager()->GetStdIcons();
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxString filename = files.Item(i);
        filename.Trim().Trim(false);
        filename.Replace("\t", " ");
        wxString prefix = filename.BeforeFirst(' ');
        filename = filename.AfterFirst(' ');
        wxString filenameFullpath = filename;

        filename.Trim().Trim(false);

        wxFileName fn(filename);
        if(fn.IsRelative()) {
            fn.MakeAbsolute(m_git->GetRepositoryDirectory());
            if(fn.FileExists()) { filenameFullpath = fn.GetFullPath(); }
        }

        if(filename.EndsWith("/")) { continue; }
        wxChar chX = prefix[0];
        wxChar chY = 0;
        if(prefix.length() > 1) { chY = prefix[1]; }

        wxBitmap statusBmp;
        eGitFile kind = eGitFile::kUntrackedFile;
        switch(chX) {
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

        if(kind != eGitFile::kUntrackedFile) {
            cols.clear();
            cols.push_back(wxString() << chX);
            cols.push_back(MakeFileBitmapLabel(filename));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new GitClientData(filenameFullpath, kind));
        } else {
            cols.clear();
            cols.push_back(MakeFileBitmapLabel(fn.GetFullName()));
            cols.push_back(fn.GetFullPath());
            m_dvListCtrlUnversioned->AppendItem(cols, (wxUIntPtr) new GitClientData(filenameFullpath, kind));
        }
    }
    m_notebookChanges->SetPageText(1, wxString() << _("Unversioned files (") << m_dvListCtrlUnversioned->GetItemCount()
                                                 << ")");
}

void GitConsole::OnContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    bool hasSelection = (m_dvListCtrl->GetSelectedItemsCount() > 0);
    if(hasSelection) {
        menu.Append(XRCID("git_console_open_file"), _("Open File"));
        menu.AppendSeparator();
        menu.Append(XRCID("git_console_reset_file"), _("Reset file"));
        menu.AppendSeparator();
    }
    menu.Append(XRCID("git_console_close_view"), _("Close View"));

    if(hasSelection) {
        menu.Bind(wxEVT_MENU, &GitConsole::OnOpenFile, this, XRCID("git_console_open_file"));
        menu.Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    }
    menu.Bind(wxEVT_MENU, &GitConsole::OnCloseView, this, XRCID("git_console_close_view"));
    m_dvListCtrl->PopupMenu(&menu);
}

void GitConsole::OnResetFile(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);
    wxArrayString filesToRevert, filesToRemove;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = GIT_ITEM_DATA(items.Item(i));
        if(gcd) {
            if(gcd->GetKind() == eGitFile::kNewFile) {
                filesToRemove.push_back(gcd->GetPath());

            } else if((gcd->GetKind() == eGitFile::kModifiedFile) || (gcd->GetKind() == eGitFile::kRenamedFile)) {
                filesToRevert.push_back(gcd->GetPath());
            }
        }
    }
    if(filesToRevert.IsEmpty() && filesToRemove.IsEmpty()) { return; }

    GitResetDlg dlg(EventNotifier::Get()->TopFrame(), filesToRevert, filesToRemove);
    if(dlg.ShowModal() != wxID_OK) { return; }

    filesToRevert = dlg.GetItemsToRevert();
    filesToRemove = dlg.GetItemsToRemove();

    if(!filesToRevert.IsEmpty()) { m_git->ResetFiles(filesToRevert); }

    if(!filesToRemove.IsEmpty()) { m_git->UndoAddFiles(filesToRemove); }
}
void GitConsole::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    Clear();
    OnClearGitLog(e);
}

void GitConsole::OnItemSelectedUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelectedItemsCount()); }

void GitConsole::OnFileActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    wxArrayString files;
    GitClientData* gcd = GIT_ITEM_DATA(event.GetItem());
    if(gcd) {
        GIT_MESSAGE("Showing diff for: %s", gcd->GetPath().c_str());
        files.push_back(gcd->GetPath());
        m_git->ShowDiff(files);
    }
}

void GitConsole::OnOpenFile(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);
    wxArrayString files;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = GIT_ITEM_DATA(items.Item(i));
        if(gcd) { files.push_back(gcd->GetPath()); }
    }

    if(files.IsEmpty()) {
        e.Skip();
        return;
    }

    // open the files
    for(size_t i = 0; i < files.GetCount(); ++i) {
        GIT_MESSAGE("Opening file: %s", files.Item(i).c_str());
        m_git->GetManager()->OpenFile(files.Item(i));
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

void GitConsole::DoOnDropdown(const wxString& commandName, int id)
{
    GitEntry data;
    {
        clConfig conf("git.conf");
        conf.ReadItem(&data);
    } // Force conf out of scope, else its dtor clobbers the GitConsole::OnDropDownMenuEvent Save()
    GitCommandsEntries& ce = data.GetGitCommandsEntries(commandName);
    vGitLabelCommands_t entries = ce.GetCommands();
    int lastUsed = ce.GetLastUsedCommandIndex();

    wxArrayString arr;
    wxMenu menu;
    for(size_t n = 0; n < entries.size(); ++n) {
        wxMenuItem* item = menu.AppendRadioItem(n, entries.at(n).label);
        item->Check(n == (size_t)lastUsed);
        arr.Add(entries.at(n).command);
    }
    menu.Bind(wxEVT_MENU, wxCommandEventHandler(GitConsole::OnDropDownMenuEvent), this, 0, arr.GetCount(),
              new GitCommandData(arr, commandName, id));

    m_toolbar->ShowMenuForButton(id, &menu);
    menu.Unbind(wxEVT_MENU, wxCommandEventHandler(GitConsole::OnDropDownMenuEvent), this, 0, arr.GetCount(),
                new GitCommandData(arr, commandName, id));
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

void GitConsole::HideProgress()
{
    if(m_gauge->IsShown()) {
        m_gauge->SetValue(0);
        m_gauge->Hide();
        GetSizer()->Layout();
    }
}

void GitConsole::ShowProgress(const wxString& message, bool pulse)
{
    if(!m_gauge->IsShown()) {
        m_gauge->Show();
        GetSizer()->Layout();
    }

    if(pulse) {
        m_gauge->Pulse();
        m_gauge->Update();

    } else {
        m_gauge->SetValue(0);
        m_gauge->Update();
    }
}

void GitConsole::UpdateProgress(unsigned long current, const wxString& message)
{
    wxString trimmedMessage = message;
    m_gauge->SetValue(current);
    // m_staticTextGauge->SetLabel(trimmedMessage.Trim().Trim(false));
}

bool GitConsole::IsProgressShown() const { return m_gauge->IsShown(); }

void GitConsole::PulseProgress() { m_gauge->Pulse(); }

bool GitConsole::IsDirty() const { return (m_dvListCtrl->GetItemCount() > 0); }

void GitConsole::OnStclogStcChange(wxStyledTextEvent& event) { event.Skip(); }

void GitConsole::OnCloseView(wxCommandEvent& e)
{
    e.Skip();

    // Write down that we don't want to set this git repo once this workspace
    // is re-loaded
    if(m_git->IsWorkspaceOpened()) {
        clConfig conf("git.conf");
        GitEntry entry;
        if(conf.ReadItem(&entry)) {
            entry.DeleteEntry(m_git->GetWorkspaceFileName().GetName());
            conf.WriteItem(&entry);
        }
    }
    // Close the git view
    m_git->WorkspaceClosed();

    // Clear the source control image
    clStatusBar* sb = clGetManager()->GetStatusBar();
    if(sb) { sb->SetSourceControlBitmap(wxNullBitmap, "", ""); }
    OnWorkspaceClosed(e);
}

void GitConsole::Clear()
{
    {
        int count = m_dvListCtrl->GetItemCount();
        for(int i = 0; i < count; ++i) {
            GitClientData* cd = GIT_ITEM_DATA(m_dvListCtrl->RowToItem(i));
            if(cd) { wxDELETE(cd); }
        }
        m_dvListCtrl->DeleteAllItems();
    }
    {
        int count = m_dvListCtrlUnversioned->GetItemCount();
        for(int i = 0; i < count; ++i) {
            GitClientData* cd = reinterpret_cast<GitClientData*>(
                m_dvListCtrlUnversioned->GetItemData(m_dvListCtrlUnversioned->RowToItem(i)));
            if(cd) { wxDELETE(cd); }
        }
        m_dvListCtrlUnversioned->DeleteAllItems();
    }
}

void GitConsole::OnUpdateUI(wxUpdateUIEvent& event) { event.Enable(!m_git->GetRepositoryDirectory().IsEmpty()); }

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

wxArrayString GitConsole::GetSelectedUnversionedFiles() const
{
    if(m_dvListCtrlUnversioned->GetSelectedItemsCount() == 0) { return wxArrayString(); }
    wxArrayString paths;
    wxDataViewItemArray items;
    int count = m_dvListCtrlUnversioned->GetSelections(items);
    for(int i = 0; i < count; i++) {
        wxDataViewItem item = items.Item(i);
        if(item.IsOk() == false) { continue; }

        GitClientData* cd = reinterpret_cast<GitClientData*>(m_dvListCtrlUnversioned->GetItemData(item));
        if(cd && cd->GetKind() == eGitFile::kUntrackedFile) { paths.Add(cd->GetPath()); }
    }
    return paths;
}

void GitConsole::OnOpenUnversionedFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxArrayString paths = GetSelectedUnversionedFiles();
    if(paths.IsEmpty()) { return; }
    std::for_each(paths.begin(), paths.end(), [&](const wxString& filepath) {
        if(!wxDirExists(filepath)) { clGetManager()->OpenFile(filepath); }
    });
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
