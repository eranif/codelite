#include "fmtJQ.hpp"

#include "Platform/Platform.hpp"

fmtJQ::fmtJQ()
{
    SetName("jq");

    // handle all known JSON files
    SetFileTypes({ FileExtManager::TypeJSON, FileExtManager::TypeWorkspaceFileSystem,
                   FileExtManager::TypeWorkspaceDocker, FileExtManager::TypeWxCrafter,
                   FileExtManager::TypeWorkspaceNodeJS, FileExtManager::TypeWorkspacePHP });

    SetDescription(_("commandline JSON processor"));
    SetShortDescription(_("jq - a json formatter"));

    const auto jq_exe = ThePlatform->Which("jq");
    SetCommand({ jq_exe.value_or("jq"), ".", "-S", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(jq_exe.has_value());
}

fmtJQ::~fmtJQ() {}
