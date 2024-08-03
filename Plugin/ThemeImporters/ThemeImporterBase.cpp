#include "ThemeImporterBase.hpp"

#include "ColoursAndFontsManager.h"
#include "JSON.h"
#include "StringUtils.h"
#include "clINIParser.hpp"
#include "drawingutils.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "lexer_configuration.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/parser.h"
#include "yaml-cpp/yaml.h"

#include <string>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/tokenzr.h>

namespace
{
void SetSelectionColour(bool is_dark_theme, ThemeImporterBase::Property& selection)
{
    // set the selection colour
    if (is_dark_theme) {
        selection.bg_colour = "#CC9900";
        selection.fg_colour = "#404040";
    } else {
        selection.bg_colour = "#BDD8F2";
        selection.fg_colour = "#484848";
    }
}

bool alacritty_read_colour(const YAML::Node& node, const std::string& prop_name, wxString* value)
{
    if (!node[prop_name]) {
        return false;
    }
    try {
        wxString str = node[prop_name].as<std::string>();
        str.Replace("0x", "#");

        wxColour c{ str };
        if (c.IsOk()) {
            *value = str;
            return true;
        }
    } catch (YAML::Exception& e) {
        clDEBUG() << "exception thrown while searching for node[" << prop_name << "]." << e.msg << endl;
    } catch (...) {
        clDEBUG() << "exception thrown while searching for node[" << prop_name << "]" << endl;
    }
    return false;
}

wxString adjust_colour(const wxString& col, bool is_dark)
{
    bool col_is_dark = DrawingUtils::IsDark(col);
    if (is_dark && col_is_dark) {
        // dark on dark
        return wxColour(col).ChangeLightness(115).GetAsString(wxC2S_HTML_SYNTAX);
    } else if (!is_dark && !col_is_dark) {
        // bright on bright
        return wxColour(col).ChangeLightness(85).GetAsString(wxC2S_HTML_SYNTAX);
    } else {
        // don't modify it
        return col;
    }
}
} // namespace

ThemeImporterBase::ThemeImporterBase() {}

ThemeImporterBase::~ThemeImporterBase() {}

void ThemeImporterBase::GetEclipseXmlProperty(const wxString& bg_prop, const wxString& fg_prop,
                                              ThemeImporterBase::Property& prop) const
{
    prop = m_editor;
    if (!bg_prop.empty() && m_xmlProperties.count(bg_prop)) {
        prop.bg_colour = m_xmlProperties.find(bg_prop)->second.color;
    }

    if (!fg_prop.empty() && m_xmlProperties.count(fg_prop)) {
        prop.fg_colour = m_xmlProperties.find(fg_prop)->second.color;
    }
}

LexerConf::Ptr_t ThemeImporterBase::InitializeImport(const wxFileName& theme_file, const wxString& langName, int langId)
{
    m_langName = langName;
    if (FileExtManager::GetType(theme_file.GetFullName()) == FileExtManager::TypeXml) {
        // Eclipse Theme XML format
        return ImportEclipseXML(theme_file, langName, langId);

    } else if (FileExtManager::GetType(theme_file.GetFullName()) == FileExtManager::TypeYAML) {
        // Alacritty theme (YAML)
        return ImportAlacrittyThemeYAML(theme_file, langName, langId);

    } else if (theme_file.GetExt().Lower() == "toml") {
        // Alacritty theme (TOML)
        return ImportAlacrittyThemeToml(theme_file, langName, langId);

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

    StyleProperty sp(ID, name, colour, bgColour, wxNOT_FOUND, bold, italic, false, isEOLFilled);
    lexer->GetLexerProperties().push_back(sp);
}

void ThemeImporterBase::AddPropertySubstyle(LexerConf::Ptr_t lexer, int id, const wxString& name, const Property& prop)
{
    wxASSERT(!prop.fg_colour.IsEmpty());
    wxASSERT(!prop.bg_colour.IsEmpty());

    StyleProperty sp(id, name, prop.fg_colour, prop.bg_colour, wxNOT_FOUND, prop.isBold, prop.isItalic, false, false);
    sp.SetSubstyle();
    lexer->GetLexerProperties().push_back(sp);
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
    lexer->SetWordSet(LexerConf::WS_FUNCTIONS, GetFunctionWordSetIndex());
    lexer->SetWordSet(LexerConf::WS_CLASS, GetClassWordSetIndex());
    lexer->SetWordSet(LexerConf::WS_VARIABLES, GetLocalsSetIndex());
    lexer->SetWordSet(LexerConf::WS_OTHERS, GetOthersWordSetIndex());
}

void ThemeImporterBase::AddCommonProperties(LexerConf::Ptr_t lexer)
{
    // Set the brace match based on the background colour
    wxString whitespaceColour;
    if (IsDarkTheme()) {
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
    AddProperty(lexer, "-3", "Caret Colour", m_caret);
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
    if (!doc.Load(theme_file.GetFullPath()))
        return NULL;

    m_themeName = doc.GetRoot()->GetAttribute("name");
    LexerConf::Ptr_t lexer(new LexerConf());

    m_xmlProperties.clear();

    // Read all the properties into table
    wxXmlNode* child = doc.GetRoot()->GetChildren();
    while (child) {
        EclipseProperty property;
        property.color = child->GetAttribute("color");
        property.isBold = child->GetAttribute("bold", "false") == "true";
        property.isItalic = child->GetAttribute("italic", "false") == "true";
        m_xmlProperties.insert({ child->GetName(), property });
        child = child->GetNext();
    }

    // Add the lexer basic properties (language, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    // Read the basic properties ("m_editor")
    GetEclipseXmlProperty("background", "foreground", m_editor);

    m_isDarkTheme = DrawingUtils::IsDark(m_editor.bg_colour);
    // set the selection colour
    SetSelectionColour(m_isDarkTheme, m_selection);

    m_oper = m_editor;

    GetEclipseXmlProperty(wxEmptyString, "lineNumber", m_lineNumber);
    GetEclipseXmlProperty(wxEmptyString, "singleLineComment", m_singleLineComment);
    GetEclipseXmlProperty(wxEmptyString, "multiLineComment", m_multiLineComment);
    GetEclipseXmlProperty(wxEmptyString, "number", m_number);
    GetEclipseXmlProperty(wxEmptyString, "string", m_string);
    GetEclipseXmlProperty(wxEmptyString, "keyword", m_keyword);
    GetEclipseXmlProperty(wxEmptyString, "class", m_klass);
    GetEclipseXmlProperty(wxEmptyString, "localVariable", m_variable);
    GetEclipseXmlProperty(wxEmptyString, "javadocTag", m_javadocKeyword);
    GetEclipseXmlProperty(wxEmptyString, "methodDeclaration", m_function);
    GetEclipseXmlProperty(wxEmptyString, "field", m_field);
    GetEclipseXmlProperty(wxEmptyString, "enum", m_enum);
    GetEclipseXmlProperty(wxEmptyString, "commentTaskTag", m_task);

    m_javadoc = m_multiLineComment;
    // set the caret colour
    m_caret.fg_colour = m_isDarkTheme ? "YELLOW" : "BLACK";
    m_caret.bg_colour = m_editor.bg_colour;

    // set the active line number colour - reverse of the selection colours
    m_lineNumberActive.bg_colour = m_selection.fg_colour;
    m_lineNumberActive.fg_colour = m_selection.bg_colour;
    return lexer;
}

void ThemeImporterBase::GetEditorVSCodeColour(const std::unordered_map<std::string_view, JSONItem>& colours,
                                              const wxString& bg_prop, const wxString& fg_prop, Property& colour)
{
    colour = m_editor;

    std::string str_fg = fg_prop.ToStdString();
    std::string str_bg = bg_prop.ToStdString();
    std::string_view sv_fg(str_fg.c_str());
    std::string_view sv_bg(str_bg.c_str());

    if (!fg_prop.empty() && colours.count(sv_fg)) {
        colour.fg_colour = colours.find(sv_fg)->second.toString();
    }

    if (!bg_prop.empty() && colours.count(sv_bg)) {
        colour.bg_colour = colours.find(sv_bg)->second.toString();
    }
}

void ThemeImporterBase::GetVSCodeColour(const std::unordered_map<wxString, VSCodeScope>& lookup,
                                        const std::vector<wxString>& scopes, Property& colour)
{
    // default use editor settings
    colour = m_editor;
    for (const wxString& scope : scopes) {
        if (lookup.count(scope.Lower())) {
            colour.fg_colour = lookup.find(scope.Lower())->second.fg_colour;
            break;
        }
    }
}

LexerConf::Ptr_t ThemeImporterBase::ImportAlacrittyThemeBase(AlacrittyColours& colours, const wxString& langName,
                                                             int langId)
{
    m_editor.bg_colour = colours.bg;
    m_editor.fg_colour = colours.fg;
    m_caret.bg_colour = m_editor.bg_colour;
    m_caret.fg_colour = colours.caret;
    colours.black = adjust_colour(colours.black, m_isDarkTheme);
    colours.red = adjust_colour(colours.red, m_isDarkTheme);
    colours.green = adjust_colour(colours.green, m_isDarkTheme);
    colours.yellow = adjust_colour(colours.yellow, m_isDarkTheme);
    colours.blue = adjust_colour(colours.blue, m_isDarkTheme);
    colours.magenta = adjust_colour(colours.magenta, m_isDarkTheme);
    colours.cyan = adjust_colour(colours.cyan, m_isDarkTheme);
    colours.white = adjust_colour(colours.white, m_isDarkTheme);

    LexerConf::Ptr_t lexer(new LexerConf());

    // reset everything to the m_editor
    m_enum = m_lineNumber = m_lineNumberActive = m_selection = m_caret = m_singleLineComment = m_multiLineComment =
        m_number = m_string = m_oper = m_keyword = m_klass = m_variable = m_javadoc = m_javadocKeyword = m_function =
            m_field = m_task = m_editor;

    // no cursor colour found? define one
    if (m_caret.fg_colour.empty()) {
        m_caret.fg_colour = m_isDarkTheme ? "ORANGE" : "DARK GREY";
    }

    m_lineNumber = m_editor;
    m_lineNumberActive = m_lineNumber;
    m_lineNumberActive.fg_colour = colours.yellow;
    SetSelectionColour(m_isDarkTheme, m_selection);

    m_singleLineComment.fg_colour = "GREY";
    m_multiLineComment.fg_colour = "GREY";
    m_number.fg_colour = colours.cyan;

    m_string.fg_colour = colours.green;
    m_oper.fg_colour = m_editor.fg_colour;
    m_keyword.fg_colour = colours.magenta;
    m_klass.fg_colour = colours.yellow;
    m_variable.fg_colour = colours.red;
    m_function.fg_colour = colours.blue;
    m_javadoc = m_multiLineComment;
    m_javadocKeyword.fg_colour = colours.yellow;
    m_field = m_variable;
    m_enum.fg_colour = colours.cyan;
    m_task.fg_colour = colours.cyan;
    lexer->SetUseCustomTextSelectionFgColour(false);
    m_themeName = "Alacritty: " + colours.theme_name;

    // Add the lexer basic properties (language, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));
    clDEBUG() << "theme imported:" << m_themeName << endl;
    return lexer;
}

LexerConf::Ptr_t ThemeImporterBase::ImportAlacrittyThemeToml(const wxFileName& theme_file, const wxString& langName,
                                                             int langId)
{
    clDEBUG() << "   > Importing Alacritty Theme (TOML) file:" << theme_file << ". Language:" << langName << endl;
    std::string filename = StringUtils::ToStdString(theme_file.GetFullPath());

    clINIParser ini_parser;
    ini_parser.ParseFile(theme_file.GetFullPath());

    AlacrittyColours colours;
    colours.theme_name = theme_file.GetName();
    colours.bg = ini_parser["colors.primary"]["background"].GetValue();
    colours.fg = ini_parser["colors.primary"]["foreground"].GetValue();

    m_isDarkTheme = DrawingUtils::IsDark(colours.bg);

    wxString default_caret_colour = m_isDarkTheme ? "ORANGE" : "DARK GREY";
    colours.caret = ini_parser["colors.cursor"]["foreground"].GetValue();
    if (colours.caret.empty()) {
        colours.caret = default_caret_colour;
    }

    if (colours.bg.empty() || colours.fg.empty()) {
        clERROR() << "missing basic colours. bg=" << colours.bg << "fg=" << colours.fg << endl;
        return nullptr;
    }

    // choose the colour variant. we assume that "normal" is always there
    // and "bright" is optional. if "bright" is required but missing
    // fallback to use "normal"
    wxString section_name = "colors.normal"; // the default
    if (m_isDarkTheme && ini_parser.HasSection("colors.bright")) {
        section_name = "colors.bright";
    }

    colours.black = ini_parser[section_name]["black"].GetValue();
    colours.red = ini_parser[section_name]["red"].GetValue();
    colours.green = ini_parser[section_name]["green"].GetValue();
    colours.yellow = ini_parser[section_name]["yellow"].GetValue();
    colours.blue = ini_parser[section_name]["blue"].GetValue();
    colours.magenta = ini_parser[section_name]["magenta"].GetValue();
    colours.cyan = ini_parser[section_name]["cyan"].GetValue();
    colours.white = ini_parser[section_name]["white"].GetValue();

    if (colours.black.empty() || colours.red.empty() || colours.green.empty() || colours.yellow.empty() ||
        colours.blue.empty() || colours.magenta.empty() || colours.cyan.empty() || colours.white.empty()) {
        clERROR() << "failed to read basic colour" << endl;
        return nullptr;
    }
    return ImportAlacrittyThemeBase(colours, langName, langId);
}

LexerConf::Ptr_t ThemeImporterBase::ImportAlacrittyThemeYAML(const wxFileName& theme_file, const wxString& langName,
                                                             int langId)
{
    clDEBUG() << "   > Importing Alacritty Theme (YAML) file:" << theme_file << ". Language:" << langName << endl;

    std::string filename = StringUtils::ToStdString(theme_file.GetFullPath());

    // load the file (this might throw)
    YAML::Node config;
    try {
        config = YAML::LoadFile(filename);
    } catch (YAML::Exception& e) {
        clERROR() << "failed loading file:" << filename << "." << e.msg << endl;
        return nullptr;
    } catch (...) {
        clERROR() << "failed loading file:" << filename << endl;
        return nullptr;
    }

    // sanity
    if (config.IsNull()) {
        return nullptr;
    }

    bool has_colors = config["colors"].IsDefined();
    bool has_primary = config["colors"]["primary"].IsDefined();
    bool has_bg = config["colors"]["primary"]["background"].IsDefined();
    bool has_fg = config["colors"]["primary"]["foreground"].IsDefined();

    if (!has_colors || !has_primary || !has_bg || !has_fg) {
        // not a valid alacritty file
        return nullptr;
    }

    // base colours
    auto colors_node = config["colors"];
    AlacrittyColours colours;
    if (!alacritty_read_colour(colors_node["primary"], "background", &colours.bg) ||
        !alacritty_read_colour(colors_node["primary"], "foreground", &colours.fg))
        return nullptr;

    m_isDarkTheme = DrawingUtils::IsDark(colours.bg);

    // choose the colour variant. we assume that "normal" is always there
    // and "bright" is optional. if "bright" is required but missing
    // fallback to use "normal"
    std::string colour_variant_name = m_isDarkTheme ? "bright" : "normal";
    if (colour_variant_name == "bright" && !colors_node["bright"].IsDefined()) {
        colour_variant_name = "normal";
    }

    // ensure that the variant selected exists
    if (!colors_node[colour_variant_name].IsDefined()) {
        return nullptr;
    }

    auto colours_variant_node = colors_node[colour_variant_name];

    if (!alacritty_read_colour(colours_variant_node, "black", &colours.black) ||
        !alacritty_read_colour(colours_variant_node, "red", &colours.red) ||
        !alacritty_read_colour(colours_variant_node, "green", &colours.green) ||
        !alacritty_read_colour(colours_variant_node, "yellow", &colours.yellow) ||
        !alacritty_read_colour(colours_variant_node, "blue", &colours.blue) ||
        !alacritty_read_colour(colours_variant_node, "magenta", &colours.magenta) ||
        !alacritty_read_colour(colours_variant_node, "cyan", &colours.cyan) ||
        !alacritty_read_colour(colours_variant_node, "white", &colours.white)) {
        clDEBUG() << "failed to read basic colour" << endl;
        return nullptr;
    }

    // OPTIONAL: read the caret colour from the configuration file
    if (colors_node["cursor"].IsDefined()) {
        // use this colour
        wxString cursor_colour;
        if (alacritty_read_colour(colors_node["cursor"], "cursor", &cursor_colour)) {
            colours.caret = cursor_colour;
        }
    }

    colours.theme_name = theme_file.GetName();
    return ImportAlacrittyThemeBase(colours, langName, langId);
}

LexerConf::Ptr_t ThemeImporterBase::ImportVSCodeJSON(const wxFileName& theme_file, const wxString& langName, int langId)
{
    clDEBUG() << "   > Importing VSCode JSON file:" << theme_file << ". Language:" << langName << endl;

    JSON root(theme_file);
    if (!root.isOk()) {
        return nullptr;
    }

    auto json = root.toElement();
    m_themeName = json["name"].toString(theme_file.GetName());

    // Build the tokenColoursMap object
    auto colours = json["colors"];
    auto colours_map = colours.GetAsMap();

    // read the base properties
    m_editor = {};
    GetEditorVSCodeColour(colours_map, "editor.background", "editor.foreground", m_editor);

    // in case no fg colour provided, guess it
    if (m_editor.fg_colour.empty()) {
        if (DrawingUtils::IsDark(m_editor.bg_colour)) {
            // if its dark colour, use light text
            wxColour fg_colour = wxColour("WHITE").ChangeLightness(90);
            m_editor.fg_colour = fg_colour.GetAsString(wxC2S_HTML_SYNTAX);
        } else {
            m_editor.fg_colour = "#000000";
        }
    }

    auto tokenColors = json["tokenColors"];
    int size = tokenColors.arraySize();
    std::unordered_map<wxString, VSCodeScope> lookup;
    for (int i = 0; i < size; ++i) {
        auto token = tokenColors[i];
        if (!token.hasNamedObject("scope")) {
            continue;
        }

        // Read the "scope" property. Notice that it can be either a string or an array
        // we cover both cases here
        auto scope = token["scope"];
        wxArrayString outer_scopes;
        if (scope.isArray()) {
            // if `scope` is array, collect only
            // complete entries
            int scope_count = scope.arraySize();
            for (int j = 0; j < scope_count; ++j) {
                outer_scopes.Add(scope[j].toString());
            }
        } else {
            // scopes is a string, split it by space|,|; and add them all
            wxString scopes_str = scope.toString();
            wxArrayString tmparr = ::wxStringTokenize(scopes_str, " ;,", wxTOKEN_STRTOK);
            outer_scopes.insert(outer_scopes.end(), tmparr.begin(), tmparr.end());
        }

        wxArrayString scopes;
        for (const wxString& outer_scope : outer_scopes) {
            wxArrayString tmparr = ::wxStringTokenize(outer_scope, " ;,", wxTOKEN_STRTOK);
            if (tmparr.size() == 1) {
                // only pick entries with a single element
                scopes.push_back(tmparr[0]);
            }
        }

        for (const wxString& scope : scopes) {
            wxString fg_colour = token["settings"]["foreground"].toString();
            if (fg_colour.empty()) {
                fg_colour = m_editor.fg_colour;
            }
            lookup.insert({ scope.Lower(), VSCodeScope(m_editor, fg_colour) });
        }
    }

    LexerConf::Ptr_t lexer(new LexerConf());
    lexer->SetUseCustomTextSelectionFgColour(false);

    // Add the lexer basic properties (language, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    m_isDarkTheme = DrawingUtils::IsDark(m_editor.bg_colour);

    // set the selection colour
    SetSelectionColour(m_isDarkTheme, m_selection);
    GetEditorVSCodeColour(colours_map, "editor.background", "editorLineNumber.foreground", m_lineNumber);

    // read the caret colours
    GetEditorVSCodeColour(colours_map, "editorCursor.background", "editorCursor.foreground", m_caret);

    // active line colours
    GetEditorVSCodeColour(colours_map, "editor.lineHighlightBackground", "editor.foreground", m_lineNumberActive);

    // token colours
    GetVSCodeColour(lookup, { "comment", "comments" }, m_singleLineComment);
    GetVSCodeColour(lookup, { "comments", "comment" }, m_multiLineComment);
    GetVSCodeColour(lookup, { "constant.numeric" }, m_number);
    GetVSCodeColour(lookup, { "string" }, m_string);
    GetVSCodeColour(lookup, { "punctuation" }, m_oper);
    GetVSCodeColour(
        lookup,
        { "keyword.operator.expression.delete", "keyword.operator.expression.void", "keyword", "keyword.control" },
        m_keyword);

    // search for class names
    GetVSCodeColour(lookup,
                    { "storage.type", "storage", "storage.type.class", "entity.name.type.class",
                      "entity.name.type.class.cpp", "entity.name.type.class.php", "meta.block.class.cpp",
                      "entity.name.type.namespace", "entity.name.type", "entity.name.class", "entity.name.type",
                      "class", "entity.name", "entity.name.scope-resolution" },
                    m_klass);
    GetVSCodeColour(lookup,
                    { "entity.name.function", "meta.function-call", "entity.name.function.call.cpp",
                      "entity.name.function.call.php" },
                    m_function);

    GetVSCodeColour(lookup, { "variable", "variable.member", "meta.parameter", "variable.parameter" }, m_variable);

    m_field = m_variable;
    m_enum = m_klass;
    m_javadoc = m_multiLineComment;
    m_javadocKeyword = m_klass;
    return lexer;
}
