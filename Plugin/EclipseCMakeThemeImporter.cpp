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

bool EclipseCMakeThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    if(!Load(eclipseXmlFile)) return false;

    wxXmlDocument codeliteXML;
    wxXmlNode* lexer = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Lexer");
    codeliteXML.SetRoot(lexer);

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, "cmake", "80");

    wxXmlNode* properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Properties");
    codeliteXML.GetRoot()->AddChild(properties);

    Property background;
    Property foreground;
    Property lineNumber;
    Property selectionBackground;
    Property selectionForeground;
    Property singleLineComment;
    Property multiLineComment;
    Property javadoc;
    Property javadocKeyword;
    Property klass;
    Property variable;
    Property number;
    Property string;
    Property oper;
    Property keyword;

    // Read the basic properties
    if(!GetProperty("background", background)) return false;
    if(!GetProperty("foreground", foreground)) return false;
    if(!GetProperty("lineNumber", lineNumber)) return false;
    if(!GetProperty("selectionForeground", selectionForeground)) return false;
    if(!GetProperty("selectionBackground", selectionBackground)) return false;
    if(!GetProperty("singleLineComment", singleLineComment)) return false;
    if(!GetProperty("multiLineComment", multiLineComment)) return false;
    if(!GetProperty("number", number)) return false;
    if(!GetProperty("string", string)) return false;
    if(!GetProperty("operator", oper)) return false;
    if(!GetProperty("keyword", keyword)) return false;

    // Optionals
    if(!GetProperty("javadoc", javadoc)) {
        javadoc = multiLineComment;
    }

    if(!GetProperty("javadocKeyword", javadocKeyword)) {
        javadocKeyword = multiLineComment;
    }
    if(!GetProperty("class", klass)) {
        klass = foreground;
    }
    if(!GetProperty("localVariable", variable)) {
        variable = foreground;
    }

    // Covnert to codelite's XML properties
    AddProperty(properties, "0", "Default", foreground.colour, background.colour);
    AddProperty(properties, "1", "Comment", singleLineComment.colour, background.colour);
    AddProperty(properties, "2", "String DQ", string.colour, background.colour);
    AddProperty(properties, "3", "String LQ", string.colour, background.colour);
    AddProperty(properties, "4", "String RQ", string.colour, background.colour);
    AddProperty(properties, "5", "Commands", keyword.colour, background.colour);
    AddProperty(properties, "6", "Parameters", foreground.colour, background.colour);
    AddProperty(properties, "7", "Variables", klass.colour, background.colour);
    AddProperty(properties, "8", "UserDefined", foreground.colour, background.colour);
    AddProperty(properties, "9", "While Def", keyword.colour, background.colour);
    AddProperty(properties, "10", "Foreach Def", keyword.colour, background.colour);
    AddProperty(properties, "11", "If Defined Def", keyword.colour, background.colour);
    AddProperty(properties, "12", "Macro Def", keyword.colour, background.colour);
    AddProperty(properties, "13", "String Var", klass.colour, background.colour);
    AddProperty(properties, "14", "Number", number.colour, background.colour);

    AddCommonProperties(properties);
    wxString codeliteXmlFile =
        wxFileName(clStandardPaths::Get().GetUserLexersDir(), GetOutputFile("cmake")).GetFullPath();
    return ::SaveXmlToFile(&codeliteXML, codeliteXmlFile);
}
