#include "EclipseCMakeThemeImporter.h"
#include "cl_standard_paths.h"
#include "globals.h"

EclipseCMakeThemeImporter::EclipseCMakeThemeImporter()
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

EclipseCMakeThemeImporter::~EclipseCMakeThemeImporter() {}

LexerConf::Ptr_t EclipseCMakeThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "cmake", 80);
    CHECK_PTR_RET_NULL(lexer);
    
    // Covnert to codelite's XML properties
    AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "1", "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, "2", "String DQ", m_string.colour, m_background.colour);
    AddProperty(lexer, "3", "String LQ", m_string.colour, m_background.colour);
    AddProperty(lexer, "4", "String RQ", m_string.colour, m_background.colour);
    AddProperty(lexer, "5", "Commands", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "6", "Parameters", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "7", "Variables", m_klass.colour, m_background.colour);
    AddProperty(lexer, "8", "UserDefined", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "9", "While Def", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "10", "Foreach Def", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "11", "If Defined Def", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "12", "Macro Def", m_keyword.colour, m_background.colour);
    AddProperty(lexer, "13", "String Var", m_klass.colour, m_background.colour);
    AddProperty(lexer, "14", "Number", m_number.colour, m_background.colour);

    FinalizeImport(lexer);
    return lexer;
}
