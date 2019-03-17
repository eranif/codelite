#include "CompileFlagsTxt.h"
#include "fileutils.h"
#include <wx/tokenzr.h>

CompileFlagsTxt::CompileFlagsTxt(const wxFileName& filename)
    : m_filename(filename)
{
    wxString data;
    wxStringSet_t uniqueIncludePaths;
    wxStringSet_t uniqueMacros;
    wxStringSet_t uniqueOthers;

    if(FileUtils::ReadFileContent(m_filename, data)) {
        wxArrayString lines = ::wxStringTokenize(data, "\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            wxString& line = lines.Item(i);
            line.Trim().Trim(false);
            if(line.StartsWith("-I")) {
                line.Remove(0, 2); // remove the "-I"
                if(uniqueIncludePaths.count(line) == 0) {
                    uniqueIncludePaths.insert(line);
                    m_includes.Add(line);
                }
            } else if(line.StartsWith("-D")) {
                line.Remove(0, 2); // remove the "-D"
                if(uniqueMacros.count(line) == 0) {
                    uniqueMacros.insert(line);
                    m_macros.Add(line);
                }
            } else {
                // others
                if(uniqueOthers.count(line) == 0) {
                    uniqueOthers.insert(line);
                    m_others.Add(line);
                }
            }
        }
    }
}

CompileFlagsTxt::~CompileFlagsTxt() {}
