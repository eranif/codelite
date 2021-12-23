#include "clShellHelper.hpp"

#include "asyncprocess.h"
#include "clTempFile.hpp"
#include "fileutils.h"

#include <wx/tokenzr.h>

clShellHelper::clShellHelper() {}

clShellHelper::~clShellHelper() {}

bool clShellHelper::ProcessCommand(const wxString& cmd)
{
    // Check if multiline command
    auto lines = ::wxStringTokenize(cmd, "\r\n", wxTOKEN_STRTOK);
    if(lines.size() == 1) {
        return false;
    }

    // a multiple line command was found, on linux we simply glue them using the
    // && operator, on Windows, we generate a temporary script file that should be deleted later
    m_out_command.clear();

#ifdef __WXMSW__
    wxString script_content;
    script_content << "@echo off\r\n";
    for(auto& line : lines) {
        line.Trim().Trim(false);
        script_content << line << "\r\n";
    }
    clTempFile script_file("bat");
    script_file.Persist();
    script_file.Write(script_content);
    m_out_command = script_file.GetFullPath();
    m_cleanup_scripts.Add(script_file.GetFullPath());
#else
    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(!m_out_command.empty()) {
            m_out_command << " && ";
        }
        m_out_command << line;
    }
#endif
    m_processCreateFlags = IProcessWrapInShell;
    return true;
}

void clShellHelper::Cleanup()
{
    for(const auto& file : m_cleanup_scripts) {
        FileUtils::RemoveFile(file);
    }
    m_cleanup_scripts.clear();
}
