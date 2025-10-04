#include "ThemeImporterText.hpp"

#include "cl_standard_paths.h"
#include "globals.h"

ThemeImporterText::ThemeImporterText() { SetFileExtensions("*.txt;*.log"); }

LexerConf::Ptr_t ThemeImporterText::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "text", wxSTC_LEX_NULL);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, 0, "Default", m_editor);
    AddCommonProperties(lexer);
    FinalizeImport(lexer);
    return lexer;
}
