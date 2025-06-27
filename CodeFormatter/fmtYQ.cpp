#include "fmtYQ.hpp"

#include "Platform/Platform.hpp"

#include <wx/string.h>

fmtYQ::fmtYQ()
{
    SetName("yq");

    // handle all known JSON files
    SetFileTypes({ FileExtManager::TypeYAML });

    SetDescription(_("commandline YAML processor"));
    SetShortDescription(_("Yaml formatter"));

    const auto yq_exe = ThePlatform->Which("yq");
    SetCommand({ yq_exe.value_or("yq"), ".", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(yq_exe.has_value());
}

fmtYQ::~fmtYQ() {}
