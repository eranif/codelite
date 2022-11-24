#include "fmtJQ.hpp"

#include "Platform.hpp"

fmtJQ::fmtJQ()
{
    SetName("jq");

    // handle all known JSON files
    SetFileTypes({ FileExtManager::TypeJSON, FileExtManager::TypeWorkspaceFileSystem,
                   FileExtManager::TypeWorkspaceDocker, FileExtManager::TypeWxCrafter,
                   FileExtManager::TypeWorkspaceNodeJS, FileExtManager::TypeWorkspacePHP });

    SetDescription(_("commandline JSON processor"));
    SetShortDescription(_("jq - a json formatter"));

    wxString jq_exe = "jq";
    ThePlatform->Which("jq", &jq_exe);
    SetCommand({ jq_exe, ".", "-S", R"#("$(CurrentFileRelPath)")#" });
}

fmtJQ::~fmtJQ() {}
