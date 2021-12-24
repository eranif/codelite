#include "ThemeImporterBase.hpp"

#include "ColoursAndFontsManager.h"
#include "JSON.h"
#include "cl_standard_paths.h"
#include "drawingutils.h"
#include "fileextmanager.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "xmlutils.h"

#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/tokenzr.h>

ThemeImporterBase::ThemeImporterBase() {}

ThemeImporterBase::~ThemeImporterBase() {}

bool ThemeImporterBase::GetEclipseXmlProperty(const wxXmlDocument& doc, const wxString& name,
                                              ThemeImporterBase::Property& prop) const
{
    prop.colour = "";
    prop.isBold = false;
    prop.isItalic = false;
    if(!doc.IsOk())
        return false;

    wxXmlNode* child = doc.GetRoot()->GetChildren();
    while(child) {
        if(child->GetName() == name) {
            prop.colour = child->GetAttribute("color");
            prop.isBold = child->GetAttribute("bold", "false") == "true";
            prop.isItalic = child->GetAttribute("italic", "false") == "true";
            return true;
        }
        child = child->GetNext();
    }
    return false;
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
    Property background = m_background;
    Property foreground = m_foreground;
    Property selectionBackground = m_selectionBackground;
    Property selectionForeground = m_selectionForeground;
    Property lineNumber = m_lineNumber;

    wxString whitespaceColour;
    if(IsDarkTheme()) {
        // dark theme
        // Whitespace should be a bit lighether
        whitespaceColour = wxColour(background.colour).ChangeLightness(150).GetAsString(wxC2S_HTML_SYNTAX);
        AddProperty(lexer, "34", "Brace match", "yellow", background.colour, true);
        AddProperty(lexer, "35", "Brace bad match", "red", background.colour, true);
        AddProperty(lexer, "37", "Indent Guide", background.colour, background.colour);

    } else {
        // light theme
        whitespaceColour = wxColour(background.colour).ChangeLightness(50).GetAsString(wxC2S_HTML_SYNTAX);
        AddProperty(lexer, "34", "Brace match", "black", "cyan", true);
        AddProperty(lexer, "35", "Brace bad match", "black", "red", true);
        AddProperty(lexer, "37", "Indent Guide", whitespaceColour, background.colour);
    }
    AddProperty(lexer, "-1", "Fold Margin", background.colour, background.colour);
    AddProperty(lexer, "-2", "Text Selection", selectionForeground.colour, selectionBackground.colour);
    AddProperty(lexer, "-3", "Caret Colour", IsDarkTheme() ? "white" : "black", background.colour);
    AddProperty(lexer, "-4", "Whitespace", whitespaceColour, background.colour);
    AddProperty(lexer, "38", "Calltip", foreground.colour, background.colour);
    AddProperty(lexer, "33", "Line Numbers", lineNumber.colour, background.colour, lineNumber.isBold,
                lineNumber.isItalic);
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
    wxXmlDocument doc;
    if(!doc.Load(theme_file.GetFullPath()))
        return NULL;

    m_themeName = doc.GetRoot()->GetAttribute("name");
    LexerConf::Ptr_t lexer(new LexerConf());

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    // Read the basic properties
    if(!GetEclipseXmlProperty(doc, "background", m_background))
        return NULL;

    if(wxColour(m_background.colour) == *wxBLACK) {
        // dont allow real black colour
        m_background.colour = wxColour(*wxBLACK).ChangeLightness(105).GetAsString(wxC2S_HTML_SYNTAX);
    }

    if(!GetEclipseXmlProperty(doc, "foreground", m_foreground))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "selectionForeground", m_selectionForeground))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "selectionBackground", m_selectionBackground))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "lineNumber", m_lineNumber))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "singleLineComment", m_singleLineComment))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "multiLineComment", m_multiLineComment))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "number", m_number))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "string", m_string))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "operator", m_oper))
        return NULL;
    if(!GetEclipseXmlProperty(doc, "keyword", m_keyword))
        return NULL;

    // Optional
    if(!GetEclipseXmlProperty(doc, "class", m_klass)) {
        m_klass = m_foreground;
    }
    if(!GetEclipseXmlProperty(doc, "localVariable", m_variable)) {
        m_variable = m_foreground;
    }
    if(!GetEclipseXmlProperty(doc, "javadoc", m_javadoc)) {
        m_javadoc = m_multiLineComment;
    }
    if(!GetEclipseXmlProperty(doc, "javadocKeyword", m_javadocKeyword)) {
        m_javadocKeyword = m_multiLineComment;
    }
    if(!GetEclipseXmlProperty(doc, "method", m_function)) {
        m_function = m_foreground;
    }
    if(!GetEclipseXmlProperty(doc, "field", m_field)) {
        m_field = m_foreground;
    }
    if(!GetEclipseXmlProperty(doc, "enum", m_enum)) {
        m_enum = m_foreground;
    }

    // load the theme background colour
    m_isDarkTheme = lexer->IsDark();
    return lexer;
}

wxString ThemeImporterBase::GetVSCodeColour(const wxStringMap_t& scopes_to_colours_map,
                                            const std::vector<wxString>& scopes)
{
    for(const wxString& scope : scopes) {
        if(scopes_to_colours_map.count(scope)) {
            return scopes_to_colours_map.find(scope)->second;
        }
    }
    return m_foreground.colour;
}

LexerConf::Ptr_t ThemeImporterBase::ImportVSCodeJSON(const wxFileName& theme_file, const wxString& langName, int langId)
{
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
            int scope_count = elem_scope.arraySize();
            for(int j = 0; j < scope_count; ++j) {
                outer_scopes.Add(elem_scope[j].toString());
            }
        } else {
            // scopes is a string
            wxString scopes_str = elem_scope.toString();
            outer_scopes.Add(scopes_str);
        }

        wxArrayString scopes;
        for(const wxString& outer_scope : outer_scopes) {
            wxArrayString tmparr = ::wxStringTokenize(outer_scope, " ;,", wxTOKEN_STRTOK);
            scopes.insert(scopes.end(), tmparr.begin(), tmparr.end());
        }

        for(const wxString& scope : scopes) {
            if(token.hasNamedObject("settings") && token["settings"].hasNamedObject("foreground")) {
                tokenColoursMap.insert({ scope, token["settings"]["foreground"].toString() });
            }
        }
    }

    LexerConf::Ptr_t lexer(new LexerConf());

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    // read the common properties
    m_background.colour = colours["editor.background"].toString();
    m_foreground.colour = colours["editor.foreground"].toString();
    m_selectionBackground.colour = colours["editor.selectionBackground"].toString();
    m_selectionForeground.colour.clear(); // no foreground colour, jus selection bg colour
    lexer->SetUseCustomTextSelectionFgColour(false);
    m_lineNumber.colour = colours["editorLineNumber.foreground"].toString();

    // token colours
    m_singleLineComment.colour = GetVSCodeColour(tokenColoursMap, { "comment", "comments" });
    m_multiLineComment.colour = GetVSCodeColour(tokenColoursMap, { "comment", "comments" });
    m_number.colour = GetVSCodeColour(tokenColoursMap, { "constant.numeric" });
    m_string.colour = GetVSCodeColour(tokenColoursMap, { "string" });
    m_oper.colour = GetVSCodeColour(tokenColoursMap, { "punctuation" });
    m_keyword.colour =
        GetVSCodeColour(tokenColoursMap, { "keyword.operator.expression.delete", "keyword.operator.expression.void",
                                           "keyword", "keyword.control", "storage" });

    // search for class names
    m_klass.colour =
        GetVSCodeColour(tokenColoursMap,
                        { "storage.type.class", "entity.name.type.class", "entity.name.type.class.cpp",
                          "meta.block.class.cpp", "entity.name.type.namespace", "entity.name.type", "entity.name.class",
                          "entity.name.type", "class", "entity.name", "entity.name.scope-resolution" });
    m_function.colour = GetVSCodeColour(
        tokenColoursMap, { "entity.name.function", "keyword.other.special-method", "entity.name.function.call.cpp" });
    m_variable.colour =
        GetVSCodeColour(tokenColoursMap, { "variable", "variable.member", "meta.parameter", "variable.parameter" });
    m_field = m_variable;
    m_enum = m_klass;
    m_javadoc.colour = m_multiLineComment.colour;
    m_javadocKeyword = m_function;
    return lexer;
}
