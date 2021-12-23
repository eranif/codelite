#include "CompileCommandsJSON.h"

#include "JSON.h"
#include "compiler_command_line_parser.h"

CompileCommandsJSON::CompileCommandsJSON(const wxString& filename)
    : m_filename(filename)
{
    if(m_filename.FileExists()) {
        JSON json(m_filename);
        JSONItem arr = json.toElement();
        const int count = arr.arraySize();
        for(int i = 0; i < count; ++i) {
            wxString command = arr.arrayItem(i).namedObject("command").toString();
            wxString workingDirectory = arr.arrayItem(i).namedObject("directory").toString();

            // Use the workingDirectory to convert all paths to full path
            CompilerCommandLineParser cclp(command, workingDirectory);
            m_includes = cclp.GetIncludes();
            m_macros = cclp.GetMacros();
            m_others = cclp.GetOtherOptions();
        }
    }
}

CompileCommandsJSON::~CompileCommandsJSON() {}
