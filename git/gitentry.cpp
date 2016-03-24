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

#include "gitentry.h"
#include <wx/sstream.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/ffile.h>
#include "GitLocator.h"

const wxEventType wxEVT_GIT_CONFIG_CHANGED = ::wxNewEventType();

#ifdef __WXMSW__
#define GIT_EXE "git"
#define GITK_EXE "gitk"
#elif defined(__WXMAC__)
#define GIT_EXE "git"
#define GITK_EXE "gitk"
#else
#define GIT_EXE "/usr/bin/git"
#define GITK_EXE "/usr/bin/gitk"
#endif

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
{
    GitLocator locator;
    wxFileName gitpath;
    if(locator.GetExecutable(gitpath)) {
        m_pathGIT = gitpath.GetFullPath();
    }
    locator.MSWGetGitShellCommand(m_gitShellCommand);
}

GitEntry::~GitEntry() {}

void GitEntry::FromJSON(const JSONElement& json)
{
    GitLocator locator;
    m_entries = json.namedObject("m_entries").toStringMap();
    wxString track, diff;
    track = json.namedObject("m_colourTrackedFile").toString();
    diff = json.namedObject("m_colourDiffFile").toString();
    m_pathGIT = json.namedObject("m_pathGIT").toString(m_pathGIT);
    m_pathGITK = json.namedObject("m_pathGITK").toString(m_pathGITK);
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_gitDiffDlgSashPos = json.namedObject("m_gitDiffDlgSashPos").toInt(m_gitDiffDlgSashPos);
    m_gitConsoleSashPos = json.namedObject("m_gitConsoleSashPos").toInt(m_gitConsoleSashPos);
    m_gitCommitDlgHSashPos = json.namedObject("m_gitCommitDlgHSashPos").toInt(m_gitCommitDlgHSashPos);
    m_gitCommitDlgVSashPos = json.namedObject("m_gitCommitDlgVSashPos").toInt(m_gitCommitDlgVSashPos);
    
    wxString defaultGitShell;
    locator.MSWGetGitShellCommand(defaultGitShell);
    m_gitShellCommand = json.namedObject("m_gitShellCommand").toString(defaultGitShell);
    
    // override the colour only if it is a valid colour
    if(!track.IsEmpty()) {
        m_colourTrackedFile = track;
    }
    if(!diff.IsEmpty()) {
        m_colourDiffFile = diff;
    }

    m_recentCommits = json.namedObject("m_recentCommits").toArrayString();

    // read the git commands
    JSONElement arrCommands = json.namedObject("Commands");
    for(int i = 0; i < arrCommands.arraySize(); ++i) {
        GitCommandsEntries entry;
        entry.FromJSON(arrCommands.arrayItem(i));
        m_commandsMap.insert(std::make_pair(entry.GetCommandname(), entry));
    }
}

JSONElement GitEntry::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
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
    json.addProperty("m_gitConsoleSashPos", m_gitConsoleSashPos);
    json.addProperty("m_gitCommitDlgHSashPos", m_gitCommitDlgHSashPos);
    json.addProperty("m_gitCommitDlgVSashPos", m_gitCommitDlgVSashPos);
    json.addProperty("m_recentCommits", m_recentCommits);
    json.addProperty("m_gitShellCommand", m_gitShellCommand);
    
    // Add the git commands array
    JSONElement arrCommands = JSONElement::createArray("Commands");
    json.append(arrCommands);
    GitCommandsEntriesMap_t::const_iterator iter = m_commandsMap.begin();
    for(; iter != m_commandsMap.end(); ++iter) {
        iter->second.ToJSON(arrCommands);
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
    if(msg.IsEmpty()) return;

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

void GitCommandsEntries::FromJSON(const JSONElement& json)
{
    m_commands.clear();
    m_commandName = json.namedObject("m_commandName").toString();
    m_lastUsed = json.namedObject("m_lastUsed").toInt();

    JSONElement arrCommandChoices = json.namedObject("m_commands");
    for(int i = 0; i < arrCommandChoices.arraySize(); ++i) {
        GitLabelCommand item;
        item.label = arrCommandChoices.arrayItem(i).namedObject("label").toString();
        item.command = arrCommandChoices.arrayItem(i).namedObject("command").toString();
        m_commands.push_back(item);
    }
}

void GitCommandsEntries::ToJSON(JSONElement& arr) const
{
    JSONElement obj = JSONElement::createObject();
    obj.addProperty("m_commandName", m_commandName);
    obj.addProperty("m_lastUsed", m_lastUsed);

    JSONElement commandsArr = JSONElement::createArray("m_commands");
    obj.append(commandsArr);

    vGitLabelCommands_t::const_iterator iter = m_commands.begin();
    for(; iter != m_commands.end(); ++iter) {
        JSONElement e = JSONElement::createObject();
        e.addProperty("label", iter->label);
        e.addProperty("command", iter->command);
        commandsArr.arrayAppend(e);
    }
    arr.arrayAppend(obj);
}
