#include "ThemeImporterDiff.hpp"

#include <wx/colour.h>

ThemeImporterDiff::ThemeImporterDiff() { SetFileExtensions("*.diff;*.patch;Diff;*.Diff"); }

ThemeImporterDiff::~ThemeImporterDiff() {}

LexerConf::Ptr_t ThemeImporterDiff::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "diff", wxSTC_LEX_DIFF);
    CHECK_PTR_RET_NULL(lexer);

    AddProperty(lexer, wxSTC_DIFF_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_DIFF_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_DIFF_COMMAND, "Command", m_klass);
    AddProperty(lexer, wxSTC_DIFF_HEADER, "Header", m_keyword);
    AddProperty(lexer, wxSTC_DIFF_POSITION, "Position", m_function);

    // Use pink and forest green
    if(IsDarkTheme()) {
        AddProperty(lexer, wxSTC_DIFF_DELETED, "Line Deleted", "#FF8080", m_editor.bg_colour, true);
        AddProperty(lexer, wxSTC_DIFF_ADDED, "Line Added", "#80FF80", m_editor.bg_colour, true);
    } else {
        AddProperty(lexer, wxSTC_DIFF_DELETED, "Line Deleted", "RED", m_editor.bg_colour, true);
        AddProperty(lexer, wxSTC_DIFF_ADDED, "Line Added", "rgb(0, 128, 64)", m_editor.bg_colour, true);
    }
    FinalizeImport(lexer);
    return lexer;
}
