#include "ThemeImporterCMake.hpp"

#include "cl_standard_paths.h"
#include "globals.h"

ThemeImporterCMake::ThemeImporterCMake()
{
    SetKeywords0("add_custom_command add_custom_target add_definitions add_dependencies add_executable add_library "
                 "add_subdirectory add_test aux_source_directory build_command build_name cmake_minimum_required "
                 "configure_file create_test_sourcelist else elseif enable_language enable_testing endforeach "
                 "endfunction endif endmacro endwhile exec_program execute_process export_library_dependencies file "
                 "find_file find_library find_package find_path find_program fltk_wrap_ui foreach function "
                 "get_cmake_property get_directory_property get_filename_component get_source_file_property "
                 "get_target_property get_test_property if include include_directories include_external_msproject "
                 "include_regular_expression install install_files install_programs install_targets link_directories "
                 "link_libraries list load_cache load_command macro make_directory mark_as_advanced math message "
                 "option output_required_files project qt_wrap_cpp qt_wrap_ui remove remove_definitions "
                 "separate_arguments set set_directory_properties set_source_files_properties set_target_properties "
                 "set_tests_properties site_name source_group string subdir_depends subdirs target_link_libraries "
                 "try_compile try_run unset use_mangled_mesa utility_source variable_requires vtk_make_instantiator "
                 "vtk_wrap_java vtk_wrap_python vtk_wrap_tcl while write_file");
    SetFileExtensions("*.cmake;*.CMAKE;*CMakeLists.txt");
}

LexerConf::Ptr_t ThemeImporterCMake::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "cmake", wxSTC_LEX_CMAKE);
    CHECK_PTR_RET_NULL(lexer);

    // Convert to CodeLite's XML properties
    AddProperty(lexer, wxSTC_CMAKE_DEFAULT, "Default", m_editor);
    AddProperty(lexer, wxSTC_CMAKE_COMMENT, "Comment", m_singleLineComment);
    AddProperty(lexer, wxSTC_CMAKE_STRINGDQ, "String DQ", m_string);
    AddProperty(lexer, wxSTC_CMAKE_STRINGLQ, "String LQ", m_string);
    AddProperty(lexer, wxSTC_CMAKE_STRINGRQ, "String RQ", m_string);
    AddProperty(lexer, wxSTC_CMAKE_COMMANDS, "Commands", m_keyword);
    AddProperty(lexer, wxSTC_CMAKE_PARAMETERS, "Parameters", m_variable);
    AddProperty(lexer, wxSTC_CMAKE_VARIABLE, "Variables", m_variable);
    AddProperty(lexer, wxSTC_CMAKE_USERDEFINED, "UserDefined", m_editor);
    AddProperty(lexer, wxSTC_CMAKE_WHILEDEF, "While Def", m_keyword);
    AddProperty(lexer, wxSTC_CMAKE_FOREACHDEF, "Foreach Def", m_keyword);
    AddProperty(lexer, wxSTC_CMAKE_IFDEFINEDEF, "If Defined Def", m_keyword);
    AddProperty(lexer, wxSTC_CMAKE_MACRODEF, "Macro Def", m_function);
    AddProperty(lexer, wxSTC_CMAKE_STRINGVAR, "String Var", m_klass);
    AddProperty(lexer, wxSTC_CMAKE_NUMBER, "Number", m_number);

    FinalizeImport(lexer);
    return lexer;
}
