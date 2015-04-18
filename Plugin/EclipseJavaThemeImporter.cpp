#include "EclipseJavaThemeImporter.h"

EclipseJavaThemeImporter::EclipseJavaThemeImporter()
{
    SetKeywords0("abstract assert boolean break byte case catch char class const continue default do double else "
                 "extends final  finally float for future generic goto if implements import inner "
                 "instanceof int interface long native new null outer  package private protected public rest "
                 "return short static super switch synchronized this throw throws transient try  var "
                 "void volatile while");
    SetFileExtensions("*.java");
    m_langName = "java";
}

EclipseJavaThemeImporter::~EclipseJavaThemeImporter() {}
LexerConf::Ptr_t EclipseJavaThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, GetLangName(), wxSTC_LEX_CPP);
    CHECK_PTR_RET_NULL(lexer);

    // Covnert to codelite's XML properties
    AddProperty(lexer, "0", "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer,
                "1",
                "Block comment",
                m_multiLineComment.colour,
                m_background.colour,
                m_multiLineComment.isBold,
                m_multiLineComment.isItalic);
    AddProperty(lexer,
                "2",
                "Single line comment",
                m_singleLineComment.colour,
                m_background.colour,
                m_singleLineComment.isBold,
                m_singleLineComment.isItalic);
    AddProperty(lexer,
                "3",
                "Javadoc block comment",
                m_javadoc.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer, "4", "Number", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
    AddProperty(
        lexer, "5", "Java keyword", m_keyword.colour, m_background.colour, m_keyword.isBold, m_keyword.isItalic);
    AddProperty(lexer, "6", "String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer, "7", "Character", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer, "8", "Uuid", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
    AddProperty(lexer, "9", "Preprocessor", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "10", "Operator", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "11", "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, "12", "Open String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer,
                "15",
                "Javadoc single line comment",
                m_javadoc.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer,
                "17",
                "Javadoc keyword",
                m_javadocKeyword.colour,
                m_background.colour,
                m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    AddProperty(lexer,
                "18",
                "Javadoc keyword error",
                m_javadocKeyword.colour,
                m_background.colour,
                m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    AddProperty(lexer, "16", "Workspace tags", m_klass.colour, m_background.colour, m_klass.isBold, m_klass.isItalic);
    AddProperty(
        lexer, "19", "Local variables", m_variable.colour, m_background.colour, m_variable.isBold, m_variable.isItalic);

    FinalizeImport(lexer);
    return lexer;
}
