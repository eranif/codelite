#include "clPatch.h"

#include "cl_standard_paths.h"
#include "dirsaver.h"
#include "globals.h"
#include "procutils.h"

clPatch::clPatch()
{
    // Locate the patch.exe on the system
    wxArrayString hints;
#ifdef __WXMSW__
    hints.Add(clStandardPaths::Get().GetExecutablePath());
#endif
    ::clFindExecutable("patch", m_patch, hints);
}

clPatch::~clPatch() {}

void clPatch::Patch(const wxFileName& patchFile, const wxString& workingDirectory, const wxString& args)
{
    // Sanity
    if(!m_patch.FileExists()) {
        throw clException("Could not locate patch executable");
    }

    if(!patchFile.FileExists()) {
        throw clException("Patch failed. File: '" + patchFile.GetFullPath() + "' does not exist");
    }

    // Prepare the command
    wxString command;
    command << m_patch.GetFullPath();

    ::WrapWithQuotes(command);

    if(!args.IsEmpty()) {
        command << " " << args;
    }

    // Change directory to the working directory requested by the user
    DirSaver ds;
    wxSetWorkingDirectory(workingDirectory.IsEmpty() ? ::wxGetCwd() : workingDirectory);

    wxString patch = patchFile.GetFullPath();

    command << " " << ::WrapWithQuotes(patch);
    ::WrapInShell(command);

    ProcUtils::SafeExecuteCommand(command);
}
