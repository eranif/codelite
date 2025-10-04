#include "ThemeImporterDiff.hpp"

#include "clColours.h"

#include <wx/colour.h>

ThemeImporterDiff::ThemeImporterDiff() { SetFileExtensions("*.diff;*.patch;Diff;*.Diff"); }

LexerConf::Ptr_t ThemeImporterDiff::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "diff", wxSTC_LEX_DIFF);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, wxSTC_DIFF_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_DIFF_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_DIFF_COMMAND, "Command", m_klass);
    AddProperty(lexer, wxSTC_DIFF_HEADER, "Header", m_keyword);
    AddProperty(lexer, wxSTC_DIFF_POSITION, "Position", m_function);
    AddProperty(lexer, wxSTC_DIFF_DELETED, "Line Deleted", clColours::Red(lexer->IsDark()), m_editor.bg_colour, true);
    AddProperty(lexer, wxSTC_DIFF_ADDED, "Line Added", clColours::Green(lexer->IsDark()), m_editor.bg_colour, true);
    FinalizeImport(lexer);
    return lexer;
}
