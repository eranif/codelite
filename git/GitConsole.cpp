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
#define GIT_MESSAGE1(...) \
    if(IsVerbose()) { AddText(wxString::Format(__VA_ARGS__)); }

class GitClientData : public wxClientData
{
    wxString m_path;

public:
    GitClientData(const wxString& path)
        : m_path(path)
    {
    }
    virtual ~GitClientData() {}

    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetPath() const { return m_path; }
};

// we use a custom column randerer so can have a better control over the font
class GitMyTextRenderer : public wxDataViewCustomRenderer
{
    wxDataViewListCtrl* m_listctrl;
    wxVariant m_value;
    wxFont m_font;

public:
    GitMyTextRenderer(wxDataViewListCtrl* listctrl)
        : m_listctrl(listctrl)
    {
        m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_font.SetFamily(wxFONTFAMILY_TELETYPE);

        wxBitmap bmp(1, 1);
        wxMemoryDC dc;
        dc.SelectObject(bmp);
        int xx, yy;
        wxFont f = m_font;
        dc.GetTextExtent("Tp", &xx, &yy, NULL, NULL, &f);
        m_listctrl->SetRowHeight(yy);
    }

    virtual ~GitMyTextRenderer() {}

    virtual wxSize GetSize() const
    {
        int xx, yy;
        wxBitmap bmp(1, 1);
        wxMemoryDC dc;
        dc.SelectObject(bmp);

        wxString s = m_value.GetString();
        wxFont f = m_font;
        dc.GetTextExtent(s, &xx, &yy, NULL, NULL, &f);
        return wxSize(xx, yy);
    }

    virtual bool SetValue(const wxVariant& value)
    {
        m_value = value;
        return true;
    }

    virtual bool GetValue(wxVariant& value) const
    {
        value = m_value;
        return true;
    }

    virtual bool Render(wxRect cell, wxDC* dc, int state)
    {
        wxVariant v;
        GetValue(v);
        wxString str = v.GetString();
        str.Trim();
        wxPoint pt = cell.GetTopLeft();
        wxFont f = m_font;
        dc->SetFont(f);
        dc->DrawText(str, pt);
        return true;
    }
};

// ---------------------------------------------------------------------
void PopulateAuiToolbarOverflow(
    wxAuiToolBarItemArray& append_items,
    const GitImages& images) // Helper function, partly because there's no convenient wxAuiToolBarItem ctor
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
    static const char* bitmapnames[] = { "gitNewBranch",
                                         "gitSwitchLocalBranch",
                                         "gitSwitchRemoteBranch",
                                         "",
                                         "gitRefresh",
                                         "gitApply",
                                         "",
                                         "gitStart",
                                         "gitTrash",
                                         "",
                                         "gitSettings",
                                         "gitPath",
                                         "gitClone" };
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
            item.SetId(IDs[n]);
            item.SetBitmap(images.Bitmap(bitmapnames[n]));
            item.SetLabel(labels[n]);
            append_items.Add(item);
        } else {
            append_items.Add(separator);
        }
    }
}

// ---------------------------------------------------------------------

GitConsole::GitConsole(wxWindow* parent, GitPlugin* git)
    : GitConsoleBase(parent)
    , m_git(git)
{
    // set the font to fit the C++ lexer default font
    LexerConf::Ptr_t lexCpp = EditorConfigST::Get()->GetLexer("text");
    if(lexCpp) { lexCpp->Apply(m_stcLog); }
    m_bitmapLoader = clGetManager()->GetStdIcons();

    GitImages m_images;
    m_images.SetBitmapResolution(clBitmap::ShouldLoadHiResImages() ? "@2x" : "");

#ifdef __WXMSW__
    m_dvFiles->SetIndent(clGetScaledSize(16));
#endif

    m_bitmaps = m_bitmapLoader->MakeStandardMimeMap();
    m_modifiedBmp = m_bitmapLoader->LoadBitmap("warning");
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

    m_splitter->SetSashPosition(data.GetGitConsoleSashPos());

    // Toolbar
    // clGetManager()->Get
    BitmapLoader* bmps = clGetManager()->GetStdIcons();

    m_auibar->AddTool(XRCID("git_clear_log"), _("Clear Git Log"), bmps->LoadBitmap("clear"), _("Clear Git Log"));
    m_auibar->AddTool(XRCID("git_stop_process"), _("Terminate Git Process"), bmps->LoadBitmap("execute_stop"),
                      _("Terminate Git Process"));
    m_auibar->AddSeparator();
    m_auibar->AddTool(XRCID("git_console_add_file"), _("Add File"), bmps->LoadBitmap("plus"), _("Add File"));
    m_auibar->AddTool(XRCID("git_console_reset_file"), _("Reset File"), bmps->LoadBitmap("undo"), _("Reset File"));

    m_auibar->AddTool(XRCID("git_reset_repository"), _("Reset"), m_images.Bitmap("gitResetRepo"),
                      _("Reset repository"));
    m_auibar->AddSeparator();

    m_auibar->AddTool(XRCID("git_pull"), _("Pull"), m_images.Bitmap("gitPull"), _("Pull remote changes"));
    m_auibar->SetToolDropDown(XRCID("git_pull"), true);
    m_auibar->AddTool(XRCID("git_commit"), _("Commit"), m_images.Bitmap("gitCommitLocal"), _("Commit local changes"));
    m_auibar->AddTool(XRCID("git_push"), _("Push"), m_images.Bitmap("gitPush"), _("Push local changes"));
    m_auibar->AddTool(XRCID("git_rebase"), _("Rebase"), m_images.Bitmap("gitRebase"), _("Rebase"));
    m_auibar->SetToolDropDown(XRCID("git_rebase"), true);
    m_auibar->AddSeparator();
    m_auibar->AddTool(XRCID("git_commit_diff"), _("Diffs"), m_images.Bitmap("gitDiffs"), _("Show current diffs"));
    m_auibar->AddTool(XRCID("git_browse_commit_list"), _("Log"), m_images.Bitmap("gitCommitedFiles"),
                      _("Browse commit history"));
    m_auibar->AddTool(XRCID("git_blame"), _("Blame"), m_images.Bitmap("gitBlame"), _("Git blame"));

#ifdef __WXMSW__
    m_auibar->AddSeparator();
    m_auibar->AddTool(XRCID("git_msysgit"), _("Open MSYS Git"), m_images.Bitmap("msysgit"),
                      _("Open MSYS Git at the current file location"));
#endif

    // Bind the events
    Bind(wxEVT_MENU, &GitConsole::OnClearGitLog, this, XRCID("git_clear_log"));
    Bind(wxEVT_UPDATE_UI, &GitConsole::OnClearGitLogUI, this, XRCID("git_clear_log"));
    Bind(wxEVT_MENU, &GitConsole::OnAddFile, this, XRCID("git_console_add_file"));
    Bind(wxEVT_MENU, &GitConsole::OnResetFile, this, XRCID("git_console_reset_file"));
    Bind(wxEVT_MENU, &GitConsole::OnStopGitProcess, this, XRCID("git_stop_process"));
    Bind(wxEVT_UPDATE_UI, &GitConsole::OnStopGitProcessUI, this, XRCID("git_stop_process"));

    wxAuiToolBarItemArray append_items;
    PopulateAuiToolbarOverflow(append_items, m_images);
    m_auibar->AddSeparator();
    for(size_t i = 0; i < append_items.GetCount(); ++i) {
        const wxAuiToolBarItem& item = append_items.Item(i);
        m_auibar->AddTool(item.GetId(), item.GetLabel(), item.GetBitmap(), item.GetLabel(), (wxItemKind)item.GetKind());
    }
    m_auibar->Realize();

    Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(GitConsole::OnGitPullDropdown), this,
         XRCID("git_pull"));
    Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(GitConsole::OnGitRebaseDropdown), this,
         XRCID("git_rebase"));

    // Adjust the h-scrollbar of git log
    ::clRecalculateSTCHScrollBar(m_stcLog);
}

GitConsole::~GitConsole()
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    data.SetGitConsoleSashPos(m_splitter->GetSashPosition());
    conf.WriteItem(&data);

    EventNotifier::Get()->Disconnect(wxEVT_GIT_CONFIG_CHANGED,
                                     wxCommandEventHandler(GitConsole::OnConfigurationChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(GitConsole::OnWorkspaceClosed), NULL,
                                     this);
    Unbind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(GitConsole::OnGitPullDropdown), this,
           XRCID("git_pull"));
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
    wxWindowUpdateLocker locker(m_stcLog);
    wxString tmp = text;
    tmp.Replace("\r\n", "\n");
    if(!tmp.EndsWith("\n")) { tmp.Append("\n"); }
    wxString curtext = m_stcLog->GetText();
    curtext << tmp;
    m_stcLog->SetText(curtext);
    m_stcLog->ScrollToEnd();
}

void GitConsole::AddRawText(const wxString& text) { AddText(text); }

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
    m_dvFilesModel->Clear();
    wxVector<wxVariant> cols;

    cols.clear();
    cols.push_back(MakeIconText(_("Modified"), m_modifiedBmp));
    m_itemModified = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("Modified"));

    cols.clear();
    cols.push_back(MakeIconText(_("New Files"), m_newBmp));
    m_itemNew = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("New Files"));

    cols.clear();
    cols.push_back(MakeIconText(_("Deleted Files"), m_deleteBmp));
    m_itemDeleted = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("Deleted Files"));

    cols.clear();
    cols.push_back(MakeIconText(_("Untracked"), m_untrackedBmp));
    m_itemUntracked = m_dvFilesModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData("Untracked"));

    wxArrayString files = ::wxStringTokenize(output, "\n\r", wxTOKEN_STRTOK);
    std::sort(files.begin(), files.end());

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

        wxBitmap bmp;
        if(filename.EndsWith("/")) {
            bmp = m_folderBmp;
        } else if(m_bitmaps.count(FileExtManager::GetType(filename))) {
            bmp = m_bitmaps[FileExtManager::GetType(filename)];
        } else {
            bmp = m_bitmaps[FileExtManager::TypeText];
        }

        cols.clear();
        cols.push_back(MakeIconText(filename, bmp));

        wxChar chX = prefix[0];
        wxChar chY = 0;
        if(prefix.length() > 1) { chY = prefix[1]; }

        if(chX == 'M') {
            m_dvFilesModel->AppendItem(m_itemModified, cols, new GitClientData(filenameFullpath));

        } else if(chX == 'A') {
            m_dvFilesModel->AppendItem(m_itemNew, cols, new GitClientData(filenameFullpath));

        } else if(chX == 'D') {
            // Delete from index
            m_dvFilesModel->AppendItem(m_itemDeleted, cols, new GitClientData(filenameFullpath));

        } else if(chX == 'R') {
            // Renamed in index
            // for now, we will treat renamed file as modified file
            m_dvFilesModel->AppendItem(m_itemModified, cols, new GitClientData(filenameFullpath));

        } else {
            m_dvFilesModel->AppendItem(m_itemUntracked, cols, new GitClientData(filenameFullpath));
        }
    }

#ifndef __WXMAC__
    if(!m_dvFilesModel->HasChildren(m_itemModified)) {
        m_dvFilesModel->DeleteItem(m_itemModified);
        m_itemModified = wxDataViewItem();
    } else {
        m_dvFiles->Expand(m_itemModified);
    }

    if(!m_dvFilesModel->HasChildren(m_itemUntracked)) {
        m_dvFilesModel->DeleteItem(m_itemUntracked);
        m_itemUntracked = wxDataViewItem();
    }

    if(!m_dvFilesModel->HasChildren(m_itemNew)) {
        m_dvFilesModel->DeleteItem(m_itemNew);
        m_itemNew = wxDataViewItem();
    } else {
        m_dvFiles->Expand(m_itemNew);
    }

    if(!m_dvFilesModel->HasChildren(m_itemDeleted)) {
        m_dvFilesModel->DeleteItem(m_itemDeleted);
        m_itemDeleted = wxDataViewItem();
    } else {
        m_dvFiles->Expand(m_itemDeleted);
    }
#endif
}

void GitConsole::OnContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("git_console_open_file"), _("Open File"));
    menu.AppendSeparator();
    menu.Append(XRCID("git_console_add_file"), _("Add file"));
    menu.Append(XRCID("git_console_reset_file"), _("Reset file"));
    menu.AppendSeparator();
    menu.Append(XRCID("git_console_close_view"), _("Close View"));

    menu.Bind(wxEVT_MENU, &GitConsole::OnOpenFile, this, XRCID("git_console_open_file"));
    menu.Bind(wxEVT_MENU, &GitConsole::OnCloseView, this, XRCID("git_console_close_view"));
    m_dvFiles->PopupMenu(&menu);
}

void GitConsole::OnAddFile(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvFiles->GetSelections(items);
    wxArrayString files;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = dynamic_cast<GitClientData*>(m_dvFilesModel->GetClientObject(items.Item(i)));
        if(gcd) {
            wxString path = gcd->GetPath();
            path.Trim().Trim(false);
            path.Replace("\\", "/");
            files.push_back(path);
        }
    }

    if(!files.IsEmpty()) { m_git->AddFiles(files); }
}

void GitConsole::OnResetFile(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvFiles->GetSelections(items);
    wxArrayString filesToRevert, filesToRemove;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxString parentNodeName;
        wxDataViewItem parent = m_dvFilesModel->GetParent(items.Item(i));
        if(parent.IsOk()) {
            wxStringClientData* gcd = dynamic_cast<wxStringClientData*>(m_dvFilesModel->GetClientObject(parent));
            parentNodeName = gcd->GetData();
        }

        GitClientData* gcd = dynamic_cast<GitClientData*>(m_dvFilesModel->GetClientObject(items.Item(i)));
        if(gcd) {
            if(parentNodeName == "New Files") {
                filesToRemove.push_back(gcd->GetPath());

            } else if(parentNodeName == "Modified") {
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
    m_dvFilesModel->Clear();
    OnClearGitLog(e);
}

void GitConsole::OnItemSelectedUI(wxUpdateUIEvent& event) { event.Enable(m_dvFiles->GetSelectedItemsCount()); }

void GitConsole::OnFileActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    wxArrayString files;
    GitClientData* gcd = dynamic_cast<GitClientData*>(m_dvFilesModel->GetClientObject(event.GetItem()));
    if(gcd) {
        GIT_MESSAGE("Showing diff for: %s", gcd->GetPath().c_str());
        files.push_back(gcd->GetPath());
        m_git->ShowDiff(files);
    }
}

void GitConsole::OnOpenFile(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_dvFiles->GetSelections(items);
    wxArrayString files;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        GitClientData* gcd = dynamic_cast<GitClientData*>(m_dvFilesModel->GetClientObject(items.Item(i)));
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

void GitConsole::DoOnDropdown(wxAuiToolBarEvent& e, const wxString& commandName, int id)
{

    if(!e.IsDropDownClicked()) {
        e.Skip();
        return;
    }

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
    menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitConsole::OnDropDownMenuEvent), this, 0,
              arr.GetCount(), new GitCommandData(arr, commandName, id));

    wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(e.GetEventObject());
    if(auibar) {
        clAuiToolStickness ts(auibar, e.GetToolId());
        wxRect rect = auibar->GetToolRect(e.GetId());
        wxPoint pt = auibar->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);
        PopupMenu(&menu, pt);
    }
    menu.Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GitConsole::OnDropDownMenuEvent), this, 0,
                arr.GetCount(), new GitCommandData(arr, commandName, id));
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

bool GitConsole::IsDirty() const
{
    bool hasDeleted = m_itemDeleted.IsOk() && m_dvFilesModel->HasChildren(m_itemDeleted);
    bool hasModified = m_itemModified.IsOk() && m_dvFilesModel->HasChildren(m_itemModified);
    bool hasNew = m_itemNew.IsOk() && m_dvFilesModel->HasChildren(m_itemNew);

    return hasDeleted || hasModified || hasNew;
}
void GitConsole::OnStclogStcChange(wxStyledTextEvent& event)
{
    event.Skip();
    ::clRecalculateSTCHScrollBar(m_stcLog);
}

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
