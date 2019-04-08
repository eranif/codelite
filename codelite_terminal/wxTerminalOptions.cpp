#include "wxTerminalOptions.h"
#include "fileutils.h"

wxTerminalOptions::wxTerminalOptions() {}

wxTerminalOptions::~wxTerminalOptions() {}

void wxTerminalOptions::SetCommandFromFile(const wxString& command)
{
    wxString content;
    if(FileUtils::ReadFileContent(command, content)) {
        content.Trim().Trim(false);
        SetCommand(content);
    }
}
