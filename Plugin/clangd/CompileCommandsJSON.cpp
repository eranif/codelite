#include "CompileCommandsJSON.h"

#include "JSON.h"
#include "StringUtils.h"
#include "compiler_command_line_parser.h"

CompileCommandsJSON::CompileCommandsJSON(const wxString& filename)
    : m_filename(filename)
{
    std::vector<wxString> all_includes;
    std::vector<wxString> all_macros;
    std::vector<wxString> all_others;

    if(m_filename.FileExists()) {
        JSON json(m_filename);
        JSONItem arr = json.toElement();
        const int count = arr.arraySize();
        for(int i = 0; i < count; ++i) {
            wxString command = arr.arrayItem(i).namedObject("command").toString();
            wxString workingDirectory = arr.arrayItem(i).namedObject("directory").toString();

            // Use the workingDirectory to convert all paths to full path
            CompilerCommandLineParser cclp(command, workingDirectory);
            all_includes.insert(all_includes.end(), cclp.GetIncludes().begin(), cclp.GetIncludes().end());
            all_macros.insert(all_macros.end(), cclp.GetMacros().begin(), cclp.GetMacros().end());
            all_others.insert(all_others.end(), cclp.GetOtherOptions().begin(), cclp.GetOtherOptions().end());
        }
    }

    // ensure no duplicate exists in the array
    m_includes = StringUtils::MakeUniqueArray(all_includes);
    m_macros = StringUtils::MakeUniqueArray(all_macros);
    m_others = StringUtils::MakeUniqueArray(all_others);
}

CompileCommandsJSON::~CompileCommandsJSON() {}
