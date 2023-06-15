#include "wxTerminalHistory.hpp"

#include "Platform.hpp"
#include "StringUtils.h"
#include "cl_standard_paths.h"
#include "fileutils.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

void wxTerminalHistory::Store()
{
    wxFileName history(clStandardPaths::Get().GetUserDataDir(), "history");
    wxString data = wxJoin(m_commands, '\n');
    FileUtils::WriteFileContent(history, data);
}

void wxTerminalHistory::Load()
{
    wxFileName history(clStandardPaths::Get().GetUserDataDir(), "history");
    wxString data;
    FileUtils::ReadFileContent(history, data);

    wxString homedir;
    if(ThePlatform->FindHomeDir(&homedir)) {
        wxFileName bash_history{ homedir, ".bash_history" };
        wxString bash_content;
        if(bash_history.FileExists() && FileUtils::ReadFileContent(bash_history, bash_content)) {
            data << "\n" << bash_content;
        }
    }

    // filter non unique and empty items
    std::unordered_set<wxString> visited;
    wxArrayString tmpcommands = ::wxStringTokenize(data, "\r\n", wxTOKEN_STRTOK);
    m_commands.reserve(tmpcommands.size());
    for(auto& command : tmpcommands) {
        command.Trim().Trim(false);

        // remove excessive entries
        if(command.empty()) {
            continue;
        }

        if(visited.insert(command).second) {
            m_commands.Add(command);
        }
    }
}

void wxTerminalHistory::Add(const wxString& command)
{
    wxString trimmed_command = command;
    trimmed_command.Trim().Trim(false);
    if(trimmed_command.empty()) {
        return;
    }
    auto where = m_commands.Index(command);
    if(where != wxNOT_FOUND) {
        m_commands.RemoveAt(where);
    }
    m_commands.Insert(command, 0);
    m_current = wxNOT_FOUND;
}

void wxTerminalHistory::Up()
{
    if(m_commands.empty()) {
        return;
    }
    ++m_current;
    if(m_current >= (int)m_commands.size()) {
        m_current = (m_commands.size() - 1);
    }
}

void wxTerminalHistory::Down()
{
    if(m_commands.empty()) {
        return;
    }
    --m_current;
    if(m_current < 0) {
        m_current = 0;
    }
}

wxString wxTerminalHistory::Get() const
{
    if(m_current < 0 || m_current >= (int)m_commands.size()) {
        return "";
    }
    return m_commands.Item(m_current);
}

void wxTerminalHistory::Clear()
{
    m_current = wxNOT_FOUND;
    m_commands.clear();
}

wxString wxTerminalHistory::ForCompletion() const
{
    wxArrayString sorted = m_commands;
    sorted.Sort();
    wxString result = wxJoin(sorted, '@');
    return result;
}
