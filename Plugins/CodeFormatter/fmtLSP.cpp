#include "fmtLSP.hpp"

fmtLSP::fmtLSP()
{
    SetName("LSP");
    SetLSPFormatter(true);
    SetDescription(_("Format with the language server of the file"));
    SetShortDescription(_("Language server formatter"));
    SetWorkingDirectory(wxEmptyString);

    // all languages: the formatter is only used when the file's language server can format
    wxArrayString languages;
    for (const auto& [language, file_types] : FileExtManager::GetLanguageBundles()) {
        wxUnusedVar(file_types);
        languages.Add(language);
    }
    SetLanguages(languages);
}
