#include "clConsoleCodeLiteTerminal.h"
#include "cl_standard_paths.h"

clConsoleCodeLiteTerminal::clConsoleCodeLiteTerminal()
{
    wxFileName codeliteTerminal(clStandardPaths::Get().GetBinFolder(), "codelite-terminal");
#ifdef __WXMSW__
    codeliteTerminal.SetExt("exe");
#endif
    wxString cmd = codeliteTerminal.GetFullPath();
    WrapWithQuotesIfNeeded(cmd);
    SetTerminalCommand(wxString() << cmd << " --working-directory=\"%WD%\" --command=\"%COMMAND%\"");
    SetEmptyTerminalCommand(wxString() << cmd << " --working-directory=\"%WD%\"");
}

clConsoleCodeLiteTerminal::~clConsoleCodeLiteTerminal() {}
