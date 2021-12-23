#include "clBacktickCache.hpp"

#include "fileutils.h"
#include "wx/filename.h"

#include <wx/tokenzr.h>

clBacktickCache::clBacktickCache(const wxString& directory)
{
    wxString content;
    wxFileName fn(directory, "BacktickCache.txt");
    fn.AppendDir(".codelite");
    m_file = fn.GetFullPath();
    if(fn.FileExists()) {
        FileUtils::ReadFileContent(fn, content);
        wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
        for(auto& line : lines) {
            TrimString(line);
            wxString command = line.BeforeFirst('=');
            wxString output = line.AfterFirst('=');
            TrimString(command);
            TrimString(output);

            if(command.empty()) {
                continue;
            }
            m_cache.insert({ command, output });
        }
    }
}

clBacktickCache::~clBacktickCache() {}

void clBacktickCache::Save()
{
    wxString content;
    for(const auto& vt : m_cache) {
        content << vt.first << "=" << vt.second << "\n";
    }
    FileUtils::WriteFileContent(m_file, content);
}

bool clBacktickCache::HasCommand(const wxString& command) const { return m_cache.count(command) != 0; }

void clBacktickCache::SetCommand(const wxString& command, const wxString& expanded)
{
    m_cache.erase(command);
    m_cache.insert({ command, expanded });
}

wxString clBacktickCache::GetExpanded(const wxString& command) const
{
    auto iter = m_cache.find(command);
    if(iter == m_cache.end()) {
        return "";
    }
    return iter->second;
}
