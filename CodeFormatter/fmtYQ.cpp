#include "fmtYQ.hpp"

#include "Platform.hpp"

#include <wx/string.h>

fmtYQ::fmtYQ()
{
    SetName("yq");

    // handle all known JSON files
    SetFileTypes({ FileExtManager::TypeYAML });

    SetDescription(_("commandline YAML processor"));

    wxString yq_exe = "yq";
    PLATFORM::Which("yq", &yq_exe);
    SetCommand({ yq_exe, ".", "$(CurrentFileFullPath)" });
}

fmtYQ::~fmtYQ() {}
