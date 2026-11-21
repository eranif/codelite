#include "ThemeImporterMeson.hpp"

// Meson build files. Scintilla has no Meson lexer: the Python lexer is the closest match (same comments, strings and
// operators), with the Meson keywords instead of the Python ones.
ThemeImporterMeson::ThemeImporterMeson()
{
    SetKeywords0("if elif else endif foreach endforeach and or not in break continue true false");
    // The built-in functions and objects, coloured as "Functions"
    SetKeywords1("add_global_arguments add_global_link_arguments add_languages add_project_arguments "
                 "add_project_dependencies add_project_link_arguments add_test_setup alias_target assert benchmark "
                 "both_libraries build_machine build_target configuration_data configure_file custom_target "
                 "declare_dependency dependency disabler environment error executable files find_program generator "
                 "get_option get_variable host_machine import include_directories install_data install_emptydir "
                 "install_headers install_man install_subdir install_symlink is_disabler is_variable jar join_paths "
                 "library meson message project range run_command run_target set_variable shared_library "
                 "shared_module static_library structured_sources subdir subdir_done subproject summary "
                 "target_machine test unset_variable vcs_tag warning");
    SetFileExtensions("meson.build;meson.options;meson_options.txt");
    SetLangName("meson");
}

LexerConf::Ptr_t ThemeImporterMeson::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, GetLangName(), wxSTC_LEX_PYTHON);
    AddProperty(lexer, wxSTC_P_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_P_NUMBER, "Number", m_number);
    AddProperty(lexer, wxSTC_P_STRING, "String", m_string);
    AddProperty(lexer, wxSTC_P_CHARACTER, "Character", m_string);
    AddProperty(lexer, wxSTC_P_WORD, "Keyword", m_keyword);
    AddProperty(lexer, wxSTC_P_TRIPLE, "Triple", m_string);
    AddProperty(lexer, wxSTC_P_TRIPLEDOUBLE, "Double Triple", m_string);
    AddProperty(lexer, wxSTC_P_CLASSNAME, "Class Name", m_klass);
    AddProperty(lexer, wxSTC_P_DEFNAME, "Def Name", m_function);
    AddProperty(lexer, wxSTC_P_OPERATOR, "Operator", m_oper);
    AddProperty(lexer, wxSTC_P_IDENTIFIER, "Identifier", m_editor);
    AddProperty(lexer, wxSTC_P_COMMENTBLOCK, "Comment block", m_singleLineComment);
    AddProperty(lexer, wxSTC_P_STRINGEOL, "String EOL", m_string);
    AddProperty(lexer, wxSTC_P_WORD2, "Functions", m_function);
    AddProperty(lexer, wxSTC_P_DECORATOR, "Decorator", m_klass);

#if wxCHECK_VERSION(3, 3, 0)
    AddProperty(lexer, wxSTC_P_FSTRING, "F-string", m_string);
    AddProperty(lexer, wxSTC_P_FCHARACTER, "F-character", m_string);
    AddProperty(lexer, wxSTC_P_FTRIPLE, "F-triple", m_string);
    AddProperty(lexer, wxSTC_P_FTRIPLEDOUBLE, "F-triple Double", m_string);
#endif

    FinalizeImport(lexer);
    return lexer;
}
