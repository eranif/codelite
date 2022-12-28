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

#include "ColoursAndFontsManager.h"
#include "GitResetDlg.h"
#include "bitmap_loader.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
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

    void SetPath(const wxString& path) { this->m_path = path; }
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
void PopulateToolbarOverflow(clToolBar* toolbar)
{
    std::vector<ToolBarItem> items = { { wxTRANSLATE("Create local branch"), XRCID("git_create_branch"), "file_new" },
                                       { wxTRANSLATE("Switch to local branch"), XRCID("git_switch_branch"), "split" },
                                       { wxTRANSLATE("Switch to remote branch"), XRCID("git_switch_to_remote_branch"),
                                         "remote-folder" },
                                       { wxEmptyString, wxID_SEPARATOR, wxEmptyString },
                                       { wxTRANSLATE("Refresh"), XRCID("git_refresh"), "file_reload" },
                                       { wxTRANSLATE("Apply Patch"), XRCID("git_apply_patch"), "patch" },
                                       { wxEmptyString, wxID_SEPARATOR, wxEmptyString },
                                       { wxTRANSLATE("Start gitk"), XRCID("git_start_gitk"), "debugger_start" },
                                       { wxTRANSLATE("Garbage collect"), XRCID("git_garbage_collection"), "clean" },
                                       { wxEmptyString, wxID_SEPARATOR, wxEmptyString },
                                       { wxTRANSLATE("Plugin settings"), XRCID("git_settings"), "cog" },
                                       { wxTRANSLATE("Clone a git repository"), XRCID("git_clone"), "copy" } };

    auto images = toolbar->GetBitmapsCreateIfNeeded();
    for(auto item : items) {
        if(item.id == wxID_SEPARATOR) {
            toolbar->AddSeparator();
        } else {
            toolbar->AddTool(item.id, wxGetTranslation(item.label), images->Add(item.bmp));
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
    m_errorPatterns = { { "fatal:" },
                        { "error:" },
                        { "tell me who you are" },
                        { "hook failure" },
                        { "not a git repository" },
                        { "No commit message given, aborting" } };

    m_successPatterns =
        // Informative messages, will be coloured with green
        { { "up to date" }, { "up-to-date" } };

    // yellow
    m_warningPatterns = { { "the authenticity of host" }, { "can't be established" }, { "key fingerprint" } };

    m_modifiedBmp = m_bitmapLoader->LoadBitmap("modified");
    m_untrackedBmp = m_bitmapLoader->LoadBitmap("info");
    m_folderBmp = m_bitmapLoader->LoadBitmap("folder-yellow");
    m_newBmp = m_bitmapLoader->LoadBitmap("plus");
    m_deleteBmp = m_bitmapLoader->LoadBitmap("minus");

    EventNotifier::Get()->Connect(wxEVT_GIT_CONFIG_CHANGED, wxCommandEventHandler(GitConsole::OnConfigurationChanged),
                                  NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &GitConsole::OnWorkspaceClosed, this);
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);

    // Toolbar
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(XRCID("git_clear_log"), _("Clear Git Log"), images->Add("clear"), _("Clear Git Log"));
    m_toolbar->AddTool(XRCID("git_stop_process"), _("Terminate Git Process"), images->Add("execute_stop"),
                       _("Terminate Git Process"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_console_add_file"), _("Add File"), images->Add("plus"), _("Add File"));
    m_toolbar->AddTool(XRCID("git_console_reset_file"), _("Reset File"), images->Add("undo"), _("Reset File"));

    m_toolbar->AddTool(XRCID("git_reset_repository"), _("Reset"), images->Add("clean"), _("Reset repository"));
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(XRCID("git_pull"), _("Pull"), images->Add("pull"), _("Pull remote changes"), wxITEM_DROPDOWN);

    m_toolbar->AddTool(XRCID("git_commit"), _("Commit"), images->Add("git-commit"), _("Commit local changes"));
    m_toolbar->AddTool(XRCID("git_push"), _("Push"), images->Add("up"), _("Push local changes"));
    m_toolbar->AddTool(XRCID("git_rebase"), _("Rebase"), images->Add("merge"), _("Rebase"), wxITEM_DROPDOWN);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_commit_diff"), _("Diffs"), images->Add("diff"), _("Show current diffs"));
    m_toolbar->AddTool(XRCID("git_browse_commit_list"), _("Log"), images->Add("tasks"), _("Browse commit history"));
    m_toolbar->AddTool(XRCID("git_blame"), _("Blame"), images->Add("finger"), _("Git blame"));

#ifdef __WXMSW__
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("git_msysgit"), _("Open MSYS Git"), images->Add("console"),
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
    m_gauge->Hide();
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

    // force font/colours udpate
    clCommandEvent dummy;
    OnSysColoursChanged(dummy);
}

GitConsole::~GitConsole()
{
    EventNotifier::Get()->Disconnect(wxEVT_GIT_CONFIG_CHANGED,
                                     wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &GitConsole::OnWorkspaceClosed, this);
    m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &GitConsole::OnGitPullDropdown, this, XRCID("git_pull"));
    m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &GitConsole::OnGitRebaseDropdown, this, XRCID("git_rebase"));
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &GitConsole::OnSysColoursChanged, this);
}

void GitConsole::OnClearGitLog(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dvListCtrlLog->DeleteAllItems();
}

void GitConsole::OnStopGitProcess(wxCommandEvent& event)
{
    if(m_git->GetProcess()) {
        m_git->GetProcess()->Terminate();
    }

    if(m_git->GetFolderProcess()) {
        m_git->GetFolderProcess()->Terminate();
    }
}

void GitConsole::OnStopGitProcessUI(wxUpdateUIEvent& event)
{
    event.Enable(m_git->GetProcess() || m_git->GetFolderProcess());
}

void GitConsole::OnClearGitLogUI(wxUpdateUIEvent& event) { event.Enable(!m_dvListCtrlLog->IsEmpty()); }

bool GitConsole::IsPatternFound(const wxString& buffer, const std::unordered_set<wxString>& m) const
{
    wxString lc = buffer.Lower();
    for(const auto& s : m) {
        if(lc.Contains(s)) {
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
    for(const auto& line : lines) {
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
    m_isVerbose = (data.GetFlags() & GitEntry::Git_Verbose_Log);
}

void GitConsole::UpdateTreeView(const wxString& output)
{
    Clear();
    wxVector<wxVariant> cols;
    wxArrayString files = ::wxStringTokenize(output, "\n\r", wxTOKEN_STRTOK);

    // parse the output
    std::vector<GitFileEntry> lines;
    lines.reserve(files.size());

    for(wxString& filename : files) {

        filename.Trim().Trim(false);
        filename.Replace("\t", " ");
        filename.Replace("\\", "/");

        wxString prefix = filename.BeforeFirst(' ');
        filename = filename.AfterFirst(' ');

        filename.Trim().Trim(false);
        if(filename.EndsWith("/"))
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

    for(const auto& d : lines) {
        wxString filename = d.fullname;
        wxChar chX = d.prefix[0];

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

        if(kind == eGitFile::kUntrackedFile) {
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
    if(!hasSelection) {
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
    if(filesToRevert.IsEmpty() && filesToRemove.IsEmpty()) {
        return;
    }

    GitResetDlg dlg(EventNotifier::Get()->TopFrame(), filesToRevert, filesToRemove);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    filesToRevert = dlg.GetItemsToRevert();
    filesToRemove = dlg.GetItemsToRemove();

    if(!filesToRevert.IsEmpty()) {
        m_git->ResetFiles(filesToRevert);
    }

    if(!filesToRemove.IsEmpty()) {
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
        if(gcd) {
            files.push_back(gcd->GetPath());
        }
    }

    if(files.IsEmpty()) {
        e.Skip();
        return;
    }

    // open the files
    for(size_t i = 0; i < files.GetCount(); ++i) {
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
    m_gauge->SetValue(wxMin(current, m_gauge->GetRange()));
    // m_staticTextGauge->SetLabel(trimmedMessage.Trim().Trim(false));
}

bool GitConsole::IsProgressShown() const { return m_gauge->IsShown(); }

void GitConsole::PulseProgress() { m_gauge->Pulse(); }

bool GitConsole::IsDirty() const { return (m_dvListCtrl->GetItemCount() > 0); }

void GitConsole::OnStclogStcChange(wxStyledTextEvent& event) { event.Skip(); }

void GitConsole::Clear()
{
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
        GitClientData* cd = reinterpret_cast<GitClientData*>(d);
        if(cd) {
            wxDELETE(cd);
        }
    });

    m_dvListCtrlUnversioned->DeleteAllItems([](wxUIntPtr d) {
        GitClientData* cd = reinterpret_cast<GitClientData*>(d);
        if(cd) {
            wxDELETE(cd);
        }
    });
}

void GitConsole::OnUpdateUI(wxUpdateUIEvent& event) { event.Enable(!m_git->GetRepositoryPath().IsEmpty()); }

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
    if(m_dvListCtrlUnversioned->GetSelectedItemsCount() == 0) {
        return wxArrayString();
    }
    wxArrayString paths;
    wxDataViewItemArray items;
    int count = m_dvListCtrlUnversioned->GetSelections(items);
    for(int i = 0; i < count; i++) {
        wxDataViewItem item = items.Item(i);
        if(item.IsOk() == false) {
            continue;
        }

        GitClientData* cd = reinterpret_cast<GitClientData*>(m_dvListCtrlUnversioned->GetItemData(item));
        if(cd && cd->GetKind() == eGitFile::kUntrackedFile) {
            paths.Add(cd->GetPath());
        }
    }
    return paths;
}

void GitConsole::OnOpenUnversionedFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxArrayString paths = GetSelectedUnversionedFiles();
    if(paths.IsEmpty()) {
        return;
    }

    for(const wxString& filepath : paths) {
        if(!filepath.EndsWith("/")) {
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
    if(prompt_str.StartsWith(home_dir)) {
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
    tmp.Trim();

    auto& builder = m_dvListCtrlLog->GetBuilder();
    builder.Clear();

    if(HasAnsiEscapeSequences(tmp)) {
        builder.Add(tmp, eAsciiColours::NORMAL_TEXT);
    } else {
        if(IsPatternFound(tmp, m_errorPatterns)) {
            builder.Add(tmp, eAsciiColours::RED);
        } else if(IsPatternFound(tmp, m_warningPatterns)) {
            builder.Add(tmp, eAsciiColours::YELLOW);
        } else if(IsPatternFound(tmp, m_successPatterns)) {
            builder.Add(tmp, eAsciiColours::GREEN);
        } else {
            builder.Add(tmp, eAsciiColours::NORMAL_TEXT);
        }
    }
    m_dvListCtrlLog->AddLine(builder.GetString(), text_ends_with_cr);
}

void GitConsole::PrintPrompt()
{
    auto& builder = m_dvListCtrlLog->GetBuilder();
    builder.Clear();
    builder.Add(GetPrompt(), eAsciiColours::GREEN, true);
    m_dvListCtrlLog->AddLine(builder.GetString(), false);
    builder.Clear();
}

void GitConsole::OnSysColoursChanged(clCommandEvent& event)
{
    event.Skip();
    auto font = ColoursAndFontsManager::Get().GetFixedFont();
    m_dvListCtrl->SetDefaultFont(font);
    m_dvListCtrlLog->SetDefaultFont(font);
    m_dvListCtrlUnversioned->SetDefaultFont(font);
}
