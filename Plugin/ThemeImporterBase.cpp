#include "ThemeImporterBase.hpp"

#include "ColoursAndFontsManager.h"
#include "JSON.h"
#include "cl_standard_paths.h"
#include "drawingutils.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "xmlutils.h"

#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/tokenzr.h>

namespace
{
void SetSelectionColour(bool is_dark_theme, ThemeImporterBase::Property& selection)
{
    // set the selection colour
    if(is_dark_theme) {
        selection.bg_colour = "#CC9900";
        selection.fg_colour = "#404040";
    } else {
        selection.bg_colour = "#BDD8F2";
        selection.fg_colour = "#484848";
    }
}
} // namespace

ThemeImporterBase::ThemeImporterBase() {}

ThemeImporterBase::~ThemeImporterBase() {}

void ThemeImporterBase::GetEclipseXmlProperty(const wxString& bg_prop, const wxString& fg_prop,
                                              ThemeImporterBase::Property& prop) const
{
    prop = m_editor;
    if(!bg_prop.empty() && m_xmlProperties.count(bg_prop)) {
        prop.bg_colour = m_xmlProperties.find(bg_prop)->second.color;
    }

    if(!fg_prop.empty() && m_xmlProperties.count(fg_prop)) {
        prop.fg_colour = m_xmlProperties.find(fg_prop)->second.color;
    }
}

LexerConf::Ptr_t ThemeImporterBase::InitializeImport(const wxFileName& theme_file, const wxString& langName, int langId)
{
    m_langName = langName;
    if(FileExtManager::GetType(theme_file.GetFullName()) == FileExtManager::TypeXml) {
        // Eclipse Theme XML format
        return ImportEclipseXML(theme_file, langName, langId);
    } else {
        // VSCode
        return ImportVSCodeJSON(theme_file, langName, langId);
    }
}

void ThemeImporterBase::FinalizeImport(LexerConf::Ptr_t lexer)
{
    AddCommonProperties(lexer);
    ColoursAndFontsManager::Get().UpdateLexerColours(lexer, true);
}

bool ThemeImporterBase::IsDarkTheme() const { return m_isDarkTheme; }

wxString ThemeImporterBase::GetName() const { return m_themeName; }

wxString ThemeImporterBase::GetOutputFile(const wxString& language) const
{
    wxString name = GetName();
    name.MakeLower();

    // Normalize the file name
    name.Replace(" ", "_");
    name.Replace("::", "_");
    name.Replace("(", "_");
    name.Replace(")", "_");
    name.Replace(":", "_");
    name.Replace(",", "_");
    name.Replace(".", "_");
    name.Replace(";", "_");

    wxString xmlFileName;
    xmlFileName << "lexer_" << language.Lower() << "_" << name << ".xml";
    return xmlFileName;
}

void ThemeImporterBase::AddProperty(LexerConf::Ptr_t lexer, const wxString& id, const wxString& name,
                                    const wxString& colour, const wxString& bgColour, bool bold, bool italic,
                                    bool isEOLFilled)
{
    wxASSERT(!colour.IsEmpty());
    wxASSERT(!bgColour.IsEmpty());

    long ID;
    id.ToCLong(&ID);

    StyleProperty sp(ID, colour, bgColour, 11, name, "", bold, italic, false, isEOLFilled, 50);
    lexer->GetLexerProperties().insert(std::make_pair(sp.GetId(), sp));
}

void ThemeImporterBase::AddBaseProperties(LexerConf::Ptr_t lexer, const wxString& lang, const wxString& id)
{
    lexer->SetName(lang);
    lexer->SetThemeName(GetName());
    lexer->SetIsActive(false);
    lexer->SetUseCustomTextSelectionFgColour(true);
    lexer->SetStyleWithinPreProcessor(true);
    long ID;
    id.ToCLong(&ID);
    lexer->SetLexerId(ID);
    lexer->SetKeyWords(GetKeywords0(), 0);
    lexer->SetKeyWords(GetKeywords1(), 1);
    lexer->SetKeyWords(GetKeywords2(), 2);
    lexer->SetKeyWords(GetKeywords3(), 3);
    lexer->SetKeyWords(GetKeywords4(), 4);
    lexer->SetFileSpec(GetFileExtensions());
    lexer->SetWordSetFunctionsIndex(m_functionsIndex);
    lexer->SetWordSetClassIndex(m_classesIndex);
    lexer->SetWordSetLocalsIndex(m_localsIndex);
    lexer->SetWordSetOthersIndex(m_othersIndex);
}

void ThemeImporterBase::AddCommonProperties(LexerConf::Ptr_t lexer)
{
    // Set the brace match based on the background colour
    wxString whitespaceColour;
    if(IsDarkTheme()) {
        // dark theme
        // Whitespace should be a bit lighether
        whitespaceColour = wxColour(m_editor.bg_colour).ChangeLightness(150).GetAsString(wxC2S_HTML_SYNTAX);
        AddProperty(lexer, "34", "Brace match", "yellow", m_editor.bg_colour, true);
        AddProperty(lexer, "35", "Brace bad match", "red", m_editor.bg_colour, true);
        AddProperty(lexer, "37", "Indent Guide", m_editor.bg_colour, m_editor.bg_colour);

    } else {
        // light theme
        whitespaceColour = wxColour(m_editor.bg_colour).ChangeLightness(50).GetAsString(wxC2S_HTML_SYNTAX);
        AddProperty(lexer, "34", "Brace match", "black", "cyan", true);
        AddProperty(lexer, "35", "Brace bad match", "black", "red", true);
        AddProperty(lexer, "37", "Indent Guide", m_editor.bg_colour, m_editor.bg_colour);
    }

    AddProperty(lexer, "-1", "Fold Margin", m_editor);
    AddProperty(lexer, "-2", "Text Selection", m_selection);
    AddProperty(lexer, "-3", "Caret Colour", IsDarkTheme() ? "white" : "black", m_editor.bg_colour);
    AddProperty(lexer, "-4", "Whitespace", whitespaceColour, m_editor.bg_colour);
    AddProperty(lexer, "38", "Calltip", m_editor);
    AddProperty(lexer, "33", "Line Numbers", m_lineNumber);
}

void ThemeImporterBase::DoSetKeywords(wxString& wordset, const wxString& words)
{
    wordset.clear();
    wxArrayString arr = ::wxStringTokenize(words, " \t\n", wxTOKEN_STRTOK);
    arr.Sort();

    wordset = ::wxJoin(arr, ' ');
}

LexerConf::Ptr_t ThemeImporterBase::ImportEclipseXML(const wxFileName& theme_file, const wxString& langName, int langId)
{
    wxUnusedVar(langName);
    clDEBUG() << "   > Importing Eclipse XML file:" << theme_file << ". Language:" << langName << endl;
    wxXmlDocument doc;
    if(!doc.Load(theme_file.GetFullPath()))
        return NULL;

    m_themeName = doc.GetRoot()->GetAttribute("name");
    LexerConf::Ptr_t lexer(new LexerConf());

    m_xmlProperties.clear();

    // Read all the properties into table
    wxXmlNode* child = doc.GetRoot()->GetChildren();
    while(child) {
        EclipseProperty property;
        property.color = child->GetAttribute("color");
        property.isBold = child->GetAttribute("bold", "false") == "true";
        property.isItalic = child->GetAttribute("italic", "false") == "true";
        m_xmlProperties.insert({ child->GetName(), property });
        child = child->GetNext();
    }

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    // Read the basic properties ("m_editor")
    GetEclipseXmlProperty("background", "foreground", m_editor);
    m_isDarkTheme = DrawingUtils::IsDark(m_editor.bg_colour);
    // set the selection colour
    SetSelectionColour(m_isDarkTheme, m_selection);

    GetEclipseXmlProperty(wxEmptyString, "lineNumber", m_lineNumber);
    GetEclipseXmlProperty(wxEmptyString, "singleLineComment", m_singleLineComment);
    GetEclipseXmlProperty(wxEmptyString, "multiLineComment", m_multiLineComment);
    GetEclipseXmlProperty(wxEmptyString, "number", m_number);
    GetEclipseXmlProperty(wxEmptyString, "string", m_string);
    GetEclipseXmlProperty(wxEmptyString, "operator", m_oper);
    GetEclipseXmlProperty(wxEmptyString, "keyword", m_keyword);
    GetEclipseXmlProperty(wxEmptyString, "class", m_klass);
    GetEclipseXmlProperty(wxEmptyString, "localVariableDeclaration", m_variable);
    GetEclipseXmlProperty(wxEmptyString, "javadocKeyword", m_javadocKeyword);
    GetEclipseXmlProperty(wxEmptyString, "staticMethod", m_function);
    GetEclipseXmlProperty(wxEmptyString, "field", m_field);
    GetEclipseXmlProperty(wxEmptyString, "enum", m_enum);

    m_javadoc = m_multiLineComment;
    // set the caret colour
    m_caret.fg_colour = m_isDarkTheme ? "YELLOW" : "BLACK";
    m_caret.bg_colour = m_editor.bg_colour;

    // set the active line number colour - reverse of the selection colours
    m_lineNumberActive.bg_colour = m_selection.fg_colour;
    m_lineNumberActive.fg_colour = m_selection.bg_colour;
    return lexer;
}

void ThemeImporterBase::GetEditorVSCodeColour(JSONItem& colours, const wxString& bg_prop, const wxString& fg_prop,
                                              Property& colour)
{
    colour = m_editor;
    if(!fg_prop.empty() && colours.hasNamedObject(fg_prop)) {
        colour.fg_colour = colours[fg_prop].toString();
    }

    if(!bg_prop.empty() && colours.hasNamedObject(bg_prop)) {
        colour.bg_colour = colours[bg_prop].toString();
    }
}

void ThemeImporterBase::GetVSCodeColour(const wxStringMap_t& scopes_to_colours_map, const std::vector<wxString>& scopes,
                                        Property& colour)
{
    // default use editor settings
    colour = m_editor;
    for(const wxString& scope : scopes) {
        if(scopes_to_colours_map.count(scope)) {
            colour.fg_colour = scopes_to_colours_map.find(scope)->second;
        }
    }
}

LexerConf::Ptr_t ThemeImporterBase::ImportVSCodeJSON(const wxFileName& theme_file, const wxString& langName, int langId)
{
    clDEBUG() << "   > Importing VSCode JSON file:" << theme_file << ". Language:" << langName << endl;

    JSON root(theme_file);
    if(!root.isOk()) {
        return nullptr;
    }

    auto json = root.toElement();
    m_themeName = json["name"].toString(theme_file.GetName());

    // Build the tokenColoursMap object
    auto colours = json["colors"];
    auto tokenColors = json["tokenColors"];
    int size = tokenColors.arraySize();
    wxStringMap_t tokenColoursMap;
    for(int i = 0; i < size; ++i) {
        auto token = tokenColors[i];
        auto elem_scope = token["scope"];
        wxArrayString outer_scopes;
        if(elem_scope.isArray()) {
            // if `scope` is array, collect only
            // complete entries
            int scope_count = elem_scope.arraySize();
            for(int j = 0; j < scope_count; ++j) {
                outer_scopes.Add(elem_scope[j].toString());
            }
        } else {
            // scopes is a string, split it by space|,|; and add them all
            wxString scopes_str = elem_scope.toString();
            wxArrayString tmparr = ::wxStringTokenize(scopes_str, " ;,", wxTOKEN_STRTOK);
            outer_scopes.insert(outer_scopes.end(), tmparr.begin(), tmparr.end());
        }

        wxArrayString scopes;
        for(const wxString& outer_scope : outer_scopes) {
            wxArrayString tmparr = ::wxStringTokenize(outer_scope, " ;,", wxTOKEN_STRTOK);
            if(tmparr.size() == 1) {
                // only pick entries with a single element
                scopes.push_back(tmparr[0]);
            }
        }

        for(const wxString& scope : scopes) {
            if(token.hasNamedObject("settings") && token["settings"].hasNamedObject("foreground")) {
                tokenColoursMap.insert({ scope, token["settings"]["foreground"].toString() });
            }
        }
    }

    LexerConf::Ptr_t lexer(new LexerConf());
    lexer->SetUseCustomTextSelectionFgColour(false);

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    // read the base properties
    GetEditorVSCodeColour(colours, "editor.background", "editor.foreground", m_editor);
    m_isDarkTheme = DrawingUtils::IsDark(m_editor.bg_colour);

    // set the selection colour
    SetSelectionColour(m_isDarkTheme, m_selection);

    GetEditorVSCodeColour(colours, "editor.background", "editorLineNumber.foreground", m_lineNumber);

    // read the caret colours
    GetEditorVSCodeColour(colours, "editorCursor.background", "editorCursor.foreground", m_caret);

    // active line colours
    GetEditorVSCodeColour(colours, "editor.lineHighlightBackground", "editor.foreground", m_lineNumberActive);

    // token colours
    GetVSCodeColour(tokenColoursMap, { "comment", "comments" }, m_singleLineComment);
    GetVSCodeColour(tokenColoursMap, { "comment", "comments" }, m_multiLineComment);
    GetVSCodeColour(tokenColoursMap, { "constant.numeric" }, m_number);
    GetVSCodeColour(tokenColoursMap, { "string" }, m_string);
    GetVSCodeColour(tokenColoursMap, { "punctuation" }, m_oper);
    GetVSCodeColour(tokenColoursMap,
                    { "keyword.operator.expression.delete", "keyword.operator.expression.void", "keyword",
                      "keyword.control", "storage" },
                    m_keyword);

    // search for class names
    GetVSCodeColour(tokenColoursMap,
                    { "storage.type.class", "entity.name.type.class", "entity.name.type.class.cpp",
                      "entity.name.type.class.php", "meta.block.class.cpp", "entity.name.type.namespace",
                      "entity.name.type", "entity.name.class", "entity.name.type", "class", "entity.name",
                      "entity.name.scope-resolution" },
                    m_klass);
    GetVSCodeColour(tokenColoursMap,
                    { "entity.name.function", "meta.function-call", "entity.name.function.call.cpp",
                      "entity.name.function.call.php" },
                    m_function);

    GetVSCodeColour(tokenColoursMap, { "variable", "variable.member", "meta.parameter", "variable.parameter" },
                    m_variable);

    m_field = m_variable;
    m_enum = m_klass;
    m_javadoc = m_multiLineComment;
    m_javadocKeyword = m_klass;
    return lexer;
}
