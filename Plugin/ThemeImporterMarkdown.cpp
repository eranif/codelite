#include "ThemeImporterMarkdown.hpp"

#include "ColoursAndFontsManager.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "globals.h"
#include "xmlutils.h"

#include <wx/dir.h>
#include <wx/stc/stc.h>

ThemeImporterMarkdown::ThemeImporterMarkdown()
{
    // Preprocessor definitions (we are going to use it for functions)
    SetFileExtensions("*.md;README");
}

ThemeImporterMarkdown::~ThemeImporterMarkdown() {}

LexerConf::Ptr_t ThemeImporterMarkdown::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "markdown", wxSTC_LEX_MARKDOWN);
    CHECK_PTR_RET_NULL(lexer);

    // Covnert to codelite's XML properties
    AddProperty(lexer, wxSTC_MARKDOWN_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_LINE_BEGIN, "Start of line", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_STRONG1, "Strong 1", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_STRONG2, "Strong 2", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_EM1, "Emphasis 1", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_EM2, "Emphasis 2", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_HEADER1, "Header 1", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_HEADER2, "Header 2", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_HEADER3, "Header 3", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_HEADER4, "Header 4", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_HEADER5, "Header 5", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_HEADER6, "Header 6", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_PRECHAR, "Prechar", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_ULIST_ITEM, "Unordered List Item", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_OLIST_ITEM, "Ordered List Item", m_klass);
    AddProperty(lexer, wxSTC_MARKDOWN_BLOCKQUOTE, "Block Quote", m_multiLineComment);
    AddProperty(lexer, wxSTC_MARKDOWN_STRIKEOUT, "Strike out", m_singleLineComment);
    AddProperty(lexer, wxSTC_MARKDOWN_HRULE, "Hrule", m_number);
    AddProperty(lexer, wxSTC_MARKDOWN_LINK, "Link", m_variable);
    AddProperty(lexer, wxSTC_MARKDOWN_CODE, "Code", m_function);
    AddProperty(lexer, wxSTC_MARKDOWN_CODE2, "Code2", m_editor);
    AddProperty(lexer, wxSTC_MARKDOWN_CODEBK, "Code Block", m_editor);

    // bold font
    const std::vector<int> emphasis = { wxSTC_MARKDOWN_STRONG1, wxSTC_MARKDOWN_STRONG2, wxSTC_MARKDOWN_EM1,
                                        wxSTC_MARKDOWN_EM2 };
    for(auto state : emphasis) {
        auto& prop = lexer->GetProperty(state);
        prop.SetBold(true);
    }

    auto& prop_link = lexer->GetProperty(wxSTC_MARKDOWN_LINK);
    prop_link.SetUnderlined(true);
    prop_link.SetItalic(true);

    // headings
    const std::vector<int> headings = { wxSTC_MARKDOWN_HEADER1, wxSTC_MARKDOWN_HEADER2, wxSTC_MARKDOWN_HEADER3,
                                        wxSTC_MARKDOWN_HEADER4, wxSTC_MARKDOWN_HEADER5, wxSTC_MARKDOWN_HEADER6 };

    for(auto state : headings) {
        auto& prop_heading = lexer->GetProperty(state);
        prop_heading.SetBold(true);
        prop_heading.SetEolFilled(true);
    }

    // code style
    const std::vector<int> codes = { wxSTC_MARKDOWN_CODE2, wxSTC_MARKDOWN_CODE, wxSTC_MARKDOWN_CODEBK };
    bool is_dark = lexer->IsDark();
    for(auto state : codes) {
        auto& prop_code = lexer->GetProperty(state);
        // use different background colour for code sections
        wxColour code_bg_colour = wxColour(prop_code.GetBgColour()).ChangeLightness(is_dark ? 110 : 90);
        prop_code.SetBgColour(code_bg_colour.GetAsString(wxC2S_HTML_SYNTAX));
        prop_code.SetEolFilled(true);
    }

    FinalizeImport(lexer);
    return lexer;
}
