//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitentry.cpp
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

//////////////////////////////////////////////////////////////////////////////

#include "GitLocator.h"
#include "gitentry.h"
#include <wx/ffile.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>

const wxEventType wxEVT_GIT_CONFIG_CHANGED = ::wxNewEventType();

#define GIT_EXE "git"
#define GITK_EXE "gitk"

GitEntry::GitEntry()
    : clConfigItem("git-settings")
    , m_colourTrackedFile(*wxGREEN)
    , m_colourDiffFile(*wxRED)
#ifdef __WXMSW__
    , m_flags(Git_Show_Terminal)
#else
    , m_flags(0)
#endif
    , m_gitDiffDlgSashPos(0)
    , m_gitConsoleSashPos(0)
    , m_gitCommitDlgHSashPos(0)
    , m_gitCommitDlgVSashPos(0)
    , m_gitBlameShowLogControls(true)
    , m_gitBlameShowParentCommit(true)
    , m_gitBlameDlgMainSashPos(0)
    , m_gitBlameDlgHSashPos(0)
    , m_gitBlameDlgVSashPos(0)
{
    GitLocator locator;
    wxFileName gitpath;
    if(locator.GetExecutable(gitpath)) {
        m_pathGIT = gitpath.GetFullPath();
    }
    locator.MSWGetGitShellCommand(m_gitShellCommand);
}

GitEntry::~GitEntry() {}

void GitEntry::FromJSON(const JSONItem& json)
{
    GitLocator locator;
    m_entries = json.namedObject("m_entries").toStringMap();
    wxString track, diff;
    track = json.namedObject("m_colourTrackedFile").toString();
    diff = json.namedObject("m_colourDiffFile").toString();
    m_pathGIT = json.namedObject("m_pathGIT").toString(m_pathGIT);

    // If the current path does not exists, search again
    bool updateGitShell = false;
    if(!wxFileName::FileExists(m_pathGIT)) {
        GitLocator locator;
        wxFileName gitpath;
        if(locator.GetExecutable(gitpath)) {
            updateGitShell = true;
            m_pathGIT = gitpath.GetFullPath();
        }
    }
    m_pathGITK = json.namedObject("m_pathGITK").toString(m_pathGITK);
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_gitDiffDlgSashPos = json.namedObject("m_gitDiffDlgSashPos").toInt(m_gitDiffDlgSashPos);
    m_gitDiffChooseDlgRadioSel1 = json.namedObject("m_gitDiffChooseDlgRadioSel1").toInt(m_gitDiffChooseDlgRadioSel1);
    m_gitDiffChooseDlgRadioSel2 = json.namedObject("m_gitDiffChooseDlgRadioSel2").toInt(m_gitDiffChooseDlgRadioSel2);
    m_gitDiffChooseDlgCBoxValues1 = json.namedObject("m_gitDiffChooseDlgCBoxValues1").toArrayString();
    m_gitDiffChooseDlgCBoxValues2 = json.namedObject("m_gitDiffChooseDlgCBoxValues2").toArrayString();
    m_gitConsoleSashPos = json.namedObject("m_gitConsoleSashPos").toInt(m_gitConsoleSashPos);
    m_gitCommitDlgHSashPos = json.namedObject("m_gitCommitDlgHSashPos").toInt(m_gitCommitDlgHSashPos);
    m_gitCommitDlgVSashPos = json.namedObject("m_gitCommitDlgVSashPos").toInt(m_gitCommitDlgVSashPos);
    m_gitBlameShowLogControls = json.namedObject("gitBlameShowLogControls").toBool(m_gitBlameShowLogControls);
    m_gitBlameShowParentCommit = json.namedObject("m_gitBlameShowParentCommit").toBool(m_gitBlameShowParentCommit);
    m_gitBlameDlgMainSashPos = json.namedObject("m_gitBlameDlgMainSashPos").toInt(m_gitBlameDlgMainSashPos);
    m_gitBlameDlgHSashPos = json.namedObject("m_gitBlameDlgHSashPos").toInt(m_gitBlameDlgHSashPos);
    m_gitBlameDlgVSashPos = json.namedObject("m_gitBlameDlgVSashPos").toInt(m_gitBlameDlgVSashPos);

    wxString defaultGitShell;
    locator.MSWGetGitShellCommand(defaultGitShell);
    if(updateGitShell) {
        m_gitShellCommand = defaultGitShell;
    } else {
        m_gitShellCommand = json.namedObject("m_gitShellCommand").toString(defaultGitShell);
    }

    // override the colour only if it is a valid colour
    if(!track.IsEmpty()) {
        m_colourTrackedFile = track;
    }
    if(!diff.IsEmpty()) {
        m_colourDiffFile = diff;
    }

    m_recentCommits = json.namedObject("m_recentCommits").toArrayString();

    // read the git commands
    JSONItem arrCommands = json.namedObject("Commands");
    for(int i = 0; i < arrCommands.arraySize(); ++i) {
        GitCommandsEntries entry;
        entry.FromJSON(arrCommands.arrayItem(i));
        m_commandsMap.insert(std::make_pair(entry.GetCommandname(), entry));
    }

    // Load the workspace info: each known workspace name & its known projects' repo paths
    JSONItem arrWorkspaces = json.namedObject("Workspaces");
    for(int i = 0; i < arrWorkspaces.arraySize(); ++i) {
        GitWorkspace workspace;
        workspace.FromJSON(arrWorkspaces.arrayItem(i));
        m_workspacesMap.insert(std::make_pair(workspace.GetWorkspaceName(), workspace));
    }
}

JSONItem GitEntry::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("m_entries", m_entries);
    if(m_colourTrackedFile.IsOk()) {
        json.addProperty("m_colourTrackedFile", m_colourTrackedFile.GetAsString(wxC2S_HTML_SYNTAX));
    }

    if(m_colourDiffFile.IsOk()) {
        json.addProperty("m_colourDiffFile", m_colourDiffFile.GetAsString(wxC2S_HTML_SYNTAX));
    }

    json.addProperty("m_pathGIT", m_pathGIT);
    json.addProperty("m_pathGITK", m_pathGITK);
    json.addProperty("m_flags", m_flags);
    json.addProperty("m_gitDiffDlgSashPos", m_gitDiffDlgSashPos);
    json.addProperty("m_gitDiffChooseDlgRadioSel1", m_gitDiffChooseDlgRadioSel1);
    json.addProperty("m_gitDiffChooseDlgRadioSel2", m_gitDiffChooseDlgRadioSel2);
    json.addProperty("m_gitDiffChooseDlgCBoxValues1", m_gitDiffChooseDlgCBoxValues1);
    json.addProperty("m_gitDiffChooseDlgCBoxValues2", m_gitDiffChooseDlgCBoxValues2);
    json.addProperty("m_gitConsoleSashPos", m_gitConsoleSashPos);
    json.addProperty("m_gitCommitDlgHSashPos", m_gitCommitDlgHSashPos);
    json.addProperty("m_gitCommitDlgVSashPos", m_gitCommitDlgVSashPos);
    json.addProperty("m_recentCommits", m_recentCommits);
    json.addProperty("m_gitShellCommand", m_gitShellCommand);
    json.addProperty("gitBlameShowLogControls", m_gitBlameShowLogControls);
    json.addProperty("m_gitBlameShowParentCommit", m_gitBlameShowParentCommit);
    json.addProperty("m_gitBlameDlgMainSashPos", m_gitBlameDlgMainSashPos);
    json.addProperty("m_gitBlameDlgHSashPos", m_gitBlameDlgHSashPos);
    json.addProperty("m_gitBlameDlgVSashPos", m_gitBlameDlgVSashPos);

    // Add the git commands array
    JSONItem arrCommands = JSONItem::createArray("Commands");
    json.append(arrCommands);
    GitCommandsEntriesMap_t::const_iterator iter = m_commandsMap.begin();
    for(; iter != m_commandsMap.end(); ++iter) {
        iter->second.ToJSON(arrCommands);
    }
    // and the workspace info
    JSONItem arrWorkspaces = JSONItem::createArray("Workspaces");
    json.append(arrWorkspaces);
    GitWorkspaceMap_t::const_iterator it = m_workspacesMap.begin();
    for(; it != m_workspacesMap.end(); ++it) {
        it->second.ToJSON(arrWorkspaces);
    }
    return json;
}

wxString GitEntry::GetGITExecutablePath() const
{
    if(m_pathGIT.IsEmpty()) {
        return GIT_EXE;

    } else {
        return m_pathGIT;
    }
}

wxString GitEntry::GetGITKExecutablePath() const
{
    if(m_pathGITK.IsEmpty()) {
        return GITK_EXE;

    } else {
        return m_pathGITK;
    }
}

GitEntry::GitProperties GitEntry::ReadGitProperties(const wxString& localRepoPath)
{
    GitEntry::GitProperties props;
    // Read the global name/email
    // ~/.gitconfig | %USERPROFILE%\.gitconfig
    {
        wxFileName globalConfig(::wxGetHomeDir(), ".gitconfig");
        if(globalConfig.Exists()) {
            wxFFile fp(globalConfig.GetFullPath(), "rb");
            if(fp.IsOpened()) {
                wxString content;
                fp.ReadAll(&content, wxConvUTF8);
                wxStringInputStream sis(content);

                wxFileConfig conf(sis);
                conf.Read("user/email", &props.global_email);
                conf.Read("user/name", &props.global_username);

                fp.Close();
            }
        }
    }

    // Read the repo config file
    if(!localRepoPath.IsEmpty()) {
        wxFileName localConfig(localRepoPath, "config");
        localConfig.AppendDir(".git");
        wxFFile fp(localConfig.GetFullPath(), "rb");
        if(fp.IsOpened()) {
            wxString content;
            fp.ReadAll(&content, wxConvUTF8);
            wxStringInputStream sis(content);

            wxFileConfig conf(sis);
            conf.Read("user/email", &props.local_email);
            conf.Read("user/name", &props.local_username);

            fp.Close();
        }
    }
    return props;
}

GitCommandsEntries& GitEntry::GetGitCommandsEntries(const wxString& entryName)
{
    if(!m_commandsMap.count(entryName)) {
        GitCommandsEntries entries(entryName);
        m_commandsMap.insert(std::make_pair(entryName, entries));
    }

    GitCommandsEntriesMap_t::iterator iter = m_commandsMap.find(entryName);
    wxASSERT(iter != m_commandsMap.end());

    return iter->second;
}

void GitEntry::AddGitCommandsEntry(GitCommandsEntries& entries, const wxString& entryName)
{
    if(!m_commandsMap.count(entryName)) {
        m_commandsMap.insert(std::make_pair(entryName, entries));
    }
    // Possible TODO: Append any novel items to the existing vector
}

void GitEntry::WriteGitProperties(const wxString& localRepoPath, const GitEntry::GitProperties& props)
{
    // Read the global name/email
    // ~/.gitconfig | %USERPROFILE%\.gitconfig
    {
        wxFileName globalConfig(::wxGetHomeDir(), ".gitconfig");
        if(globalConfig.Exists()) {
            wxFFile fp(globalConfig.GetFullPath(), "rb");
            if(fp.IsOpened()) {
                wxString content;
                fp.ReadAll(&content, wxConvUTF8);
                fp.Close();
                wxStringInputStream sis(content);
                wxFileConfig conf(sis);
                conf.Write("user/email", props.global_email);
                conf.Write("user/name", props.global_username);

                // Write the content
                content.Clear();
                wxStringOutputStream sos(&content);
                if(conf.Save(sos, wxConvUTF8)) {
                    wxFFile fpo(globalConfig.GetFullPath(), "w+b");
                    if(fpo.IsOpened()) {
                        fpo.Write(content, wxConvUTF8);
                        fpo.Close();
                    }
                } else {
                    ::wxMessageBox("Could not save GIT global configuration. Configuration is unmodified", "git",
                                   wxICON_WARNING | wxOK | wxCENTER);
                }
            }
        }
    }

    // Read the repo config file
    if(!localRepoPath.IsEmpty()) {
        wxFileName localConfig(localRepoPath, "config");
        localConfig.AppendDir(".git");
        wxFFile fp(localConfig.GetFullPath(), "rb");
        if(fp.IsOpened()) {
            wxString content;
            fp.ReadAll(&content, wxConvUTF8);
            fp.Close();
            wxStringInputStream sis(content);
            wxFileConfig conf(sis);
            conf.Write("user/email", props.local_email);
            conf.Write("user/name", props.local_username);

            content.Clear();
            wxStringOutputStream sos(&content);
            if(conf.Save(sos, wxConvUTF8)) {
                wxFFile fpo(localConfig.GetFullPath(), "w+b");
                if(fpo.IsOpened()) {
                    fpo.Write(content, wxConvUTF8);
                    fpo.Close();
                }
            } else {
                ::wxMessageBox("Could not save GIT local configuration. Configuration is unmodified", "git",
                               wxICON_WARNING | wxOK | wxCENTER);
            }
        }
    }
}

GitEntry& GitEntry::Load()
{
    clConfig conf("git.conf");
    conf.ReadItem(this);
    return *this;
}

void GitEntry::Save()
{
    clConfig conf("git.conf");
    conf.WriteItem(this);
}

void GitEntry::AddRecentCommit(const wxString& commitMessage)
{
    wxString msg = commitMessage;
    msg.Trim().Trim(false);
    if(msg.IsEmpty())
        return;

    if(m_recentCommits.Index(msg) == wxNOT_FOUND) {
        m_recentCommits.Insert(msg, 0);
    }

    if(m_recentCommits.size() > 20) {
        m_recentCommits.RemoveAt(m_recentCommits.size() - 1); // Remove the last commit
    }
}

void GitEntry::DeleteEntry(const wxString& workspace)
{
    if(m_entries.count(workspace)) {
        m_entries.erase(workspace);
    }
}

wxString GitEntry::GetProjectUserEnteredRepoPath(const wxString& nameHash)
{
    wxString repoPath;
    wxString projectName;
    wxString workspaceName = nameHash.BeforeFirst('-', &projectName);
    
    if(workspaceName.empty() || projectName.empty())
        return "";

    GitWorkspaceMap_t::iterator iter;

    if(!m_workspacesMap.count(workspaceName)) {
        // A new workspace so add it
        GitWorkspace workspace(workspaceName);
        m_workspacesMap.insert(std::make_pair(workspaceName, workspace));
    }
    iter = m_workspacesMap.find(workspaceName);
    wxCHECK_MSG(iter != m_workspacesMap.end(), repoPath, "Failed to add a workspace to the entry");

    GitWorkspace workspace = iter->second;
    repoPath = workspace.GetProjectUserEnteredRepoPath(projectName);

    return repoPath;
}

void GitEntry::SetProjectUserEnteredRepoPath(const wxString& repoPath, const wxString& nameHash)
{
    wxString projectName;
    wxString workspaceName = nameHash.BeforeFirst('-', &projectName);
    if(workspaceName.empty() || projectName.empty()) {
        return;
    }

    GitWorkspaceMap_t::iterator iter;

    if(!m_workspacesMap.count(workspaceName)) {
        // A new workspace so add it
        GitWorkspace workspace(workspaceName);
        m_workspacesMap.insert(std::make_pair(workspaceName, workspace));
    }
    iter = m_workspacesMap.find(workspaceName);
    wxCHECK_RET(iter != m_workspacesMap.end(), "Failed to add a workspace to the entry");

    GitWorkspace& workspace = iter->second;
    workspace.SetProjectUserEnteredRepoPath(projectName, repoPath);
}

void GitCommandsEntries::FromJSON(const JSONItem& json)
{
    m_commands.clear();
    m_commandName = json.namedObject("m_commandName").toString();
    m_lastUsed = json.namedObject("m_lastUsed").toInt();

    JSONItem arrCommandChoices = json.namedObject("m_commands");
    for(int i = 0; i < arrCommandChoices.arraySize(); ++i) {
        GitLabelCommand item;
        item.label = arrCommandChoices.arrayItem(i).namedObject("label").toString();
        item.command = arrCommandChoices.arrayItem(i).namedObject("command").toString();
        m_commands.push_back(item);
    }
}

void GitCommandsEntries::ToJSON(JSONItem& arr) const
{
    JSONItem obj = JSONItem::createObject();
    obj.addProperty("m_commandName", m_commandName);
    obj.addProperty("m_lastUsed", m_lastUsed);

    JSONItem commandsArr = JSONItem::createArray("m_commands");
    obj.append(commandsArr);

    vGitLabelCommands_t::const_iterator iter = m_commands.begin();
    for(; iter != m_commands.end(); ++iter) {
        JSONItem e = JSONItem::createObject();
        e.addProperty("label", iter->label);
        e.addProperty("command", iter->command);
        commandsArr.arrayAppend(e);
    }
    arr.arrayAppend(obj);
}

const wxString GitWorkspace::GetProjectUserEnteredRepoPath(const wxString& projectName) { return m_userEnteredRepoPath[projectName]; }

void GitWorkspace::SetProjectUserEnteredRepoPath(const wxString& projectName, const wxString& userEnteredRepoPath)
{
    m_userEnteredRepoPath[projectName] = userEnteredRepoPath;
}

void GitWorkspace::FromJSON(const JSONItem& json)
{
    m_projectData.clear();
    SetWorkspaceName(json.namedObject("m_workspaceName").toString());
    m_projectData = json.namedObject("m_projectData").toStringMap();
    m_userEnteredRepoPath = json.namedObject("m_userEnteredRepoPath").toStringMap();
}

void GitWorkspace::ToJSON(JSONItem& arr) const
{
    if(!GetWorkspaceName().empty()) {
        JSONItem json = JSONItem::createObject(GetWorkspaceName());
        json.addProperty("m_workspaceName", GetWorkspaceName());
        json.addProperty("m_projectData", m_projectData);
        json.addProperty("m_userEnteredRepoPath", m_userEnteredRepoPath);
        arr.arrayAppend(json);
    }
}
