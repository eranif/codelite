#include "EclipseThemeImporterBase.h"
#include "drawingutils.h"
#include "xmlutils.h"
#include <wx/colour.h>
#include "cl_standard_paths.h"
#include "globals.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include <wx/arrstr.h>
#include <wx/tokenzr.h>

EclipseThemeImporterBase::EclipseThemeImporterBase() {}

EclipseThemeImporterBase::~EclipseThemeImporterBase() {}

bool EclipseThemeImporterBase::GetProperty(const wxString& name, EclipseThemeImporterBase::Property& prop) const
{
    prop.colour = "";
    prop.isBold = false;
    prop.isItalic = false;
    if(!m_doc.IsOk()) return false;

    wxXmlNode* child = m_doc.GetRoot()->GetChildren();
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

LexerConf::Ptr_t
EclipseThemeImporterBase::InitializeImport(const wxFileName& eclipseXml, const wxString& langName, int langId)
{
    m_langName = langName;
    if(!m_doc.Load(eclipseXml.GetFullPath())) return NULL;

    LexerConf::Ptr_t lexer(new LexerConf());

    // Add the lexer basic properties (laguage, file extensions, keywords, name)
    AddBaseProperties(lexer, m_langName, wxString::Format("%d", langId));

    // Read the basic properties
    if(!GetProperty("background", m_background)) return NULL;
    if(!GetProperty("foreground", m_foreground)) return NULL;
    if(!GetProperty("selectionForeground", m_selectionForeground)) return NULL;
    if(!GetProperty("selectionBackground", m_selectionBackground)) return NULL;
    if(!GetProperty("lineNumber", m_lineNumber)) return NULL;
    if(!GetProperty("singleLineComment", m_singleLineComment)) return NULL;
    if(!GetProperty("multiLineComment", m_multiLineComment)) return NULL;
    if(!GetProperty("number", m_number)) return NULL;
    if(!GetProperty("string", m_string)) return NULL;
    if(!GetProperty("operator", m_oper)) return NULL;
    if(!GetProperty("keyword", m_keyword)) return NULL;
    if(!GetProperty("class", m_klass)) {
        m_klass = m_foreground;
    }
    if(!GetProperty("parameterVariable", m_variable)) {
        m_variable = m_foreground;
    }
    if(!GetProperty("javadoc", m_javadoc)) {
        m_javadoc = m_multiLineComment;
    }

    if(!GetProperty("javadocKeyword", m_javadocKeyword)) {
        m_javadocKeyword = m_multiLineComment;
    }
    
    m_oper = m_foreground;
    return lexer;
}

void EclipseThemeImporterBase::FinalizeImport(LexerConf::Ptr_t lexer)
{
    AddCommonProperties(lexer);
    ColoursAndFontsManager::Get().UpdateLexerColours(lexer, true);
}

bool EclipseThemeImporterBase::IsDarkTheme() const
{
    // load the theme background colour
    EclipseThemeImporterBase::Property p;
    if(!GetProperty("background", p)) return false;

    // test the colour
    return DrawingUtils::IsDark(p.colour);
}

wxString EclipseThemeImporterBase::GetName() const
{
    if(!IsValid()) return "";
    return m_doc.GetRoot()->GetAttribute("name");
}

wxString EclipseThemeImporterBase::GetOutputFile(const wxString& language) const
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

void EclipseThemeImporterBase::AddProperty(LexerConf::Ptr_t lexer,
                                           const wxString& id,
                                           const wxString& name,
                                           const wxString& colour,
                                           const wxString& bgColour,
                                           bool bold,
                                           bool italic,
                                           bool isEOLFilled)
{
    wxASSERT(!colour.IsEmpty());
    wxASSERT(!bgColour.IsEmpty());

    long ID;
    id.ToCLong(&ID);

    StyleProperty sp(ID, colour, bgColour, 11, name, "", bold, italic, false, isEOLFilled, 50);
    lexer->GetLexerProperties().insert(std::make_pair(sp.GetId(), sp));
}

void EclipseThemeImporterBase::AddBaseProperties(LexerConf::Ptr_t lexer, const wxString& lang, const wxString& id)
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
}

void EclipseThemeImporterBase::AddCommonProperties(LexerConf::Ptr_t lexer)
{
    // Set the brace match based on the background colour
    Property background, foreground, selectionBackground, selectionForeground, lineNumber;

    GetProperty("background", background);
    GetProperty("foreground", foreground);
    GetProperty("selectionForeground", selectionForeground);
    GetProperty("selectionBackground", selectionBackground);
    GetProperty("lineNumber", lineNumber);

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
    AddProperty(
        lexer, "33", "Line Numbers", lineNumber.colour, background.colour, lineNumber.isBold, lineNumber.isItalic);
}

void EclipseThemeImporterBase::DoSetKeywords(wxString& wordset, const wxString& words)
{
    wordset.clear();
    wxArrayString arr = ::wxStringTokenize(words, " \t\n", wxTOKEN_STRTOK);
    arr.Sort();
    
    wordset = ::wxJoin(arr, ' ');
}
