#include "fmtYQ.hpp"

#include "Platform.hpp"

#include <wx/string.h>

fmtYQ::fmtYQ()
{
    SetName("yq");

    // handle all known JSON files
    SetFileTypes({ FileExtManager::TypeYAML });

    SetDescription(_("commandline YAML processor"));
    SetShortDescription(_("Yaml formatter"));

    wxString yq_exe = "yq";
    ThePlatform->Which("yq", &yq_exe);
    SetCommand({ yq_exe, ".", R"#("$(CurrentFileRelPath)")#" });
}

fmtYQ::~fmtYQ() {}
