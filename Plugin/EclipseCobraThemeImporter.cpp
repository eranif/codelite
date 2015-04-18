#include "EclipseCobraThemeImporter.h"

EclipseCobraThemeImporter::EclipseCobraThemeImporter()
{
    SetKeywords0("abstract adds all and any as assert base body bool branch break callable catch char class const "
                 "continue cue decimal def do dynamic each else end ensure enum event every except expect extend "
                 "extern fake false finally float for from get has if ignore implements implies import in inherits "
                 "inlined inout int interface internal invariant is listen lock mixin must namespace new nil "
                 "nonvirtual not number objc of old or out override partial pass passthrough post print private pro "
                 "protected public raise ref require return same set shared sig stop struct success test this throw to "
                 "to? trace true try uint use using var vari virtual where while yield");
    SetFileExtensions("*.cobra;cob");
}

EclipseCobraThemeImporter::~EclipseCobraThemeImporter() {}

LexerConf::Ptr_t EclipseCobraThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "cobra", wxSTC_LEX_PYTHON);
    AddProperty(lexer, wxSTC_P_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Comment", m_singleLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_COMMENTLINE, "Number", m_number.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_STRING, "String", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_CHARACTER, "Character", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_WORD, "Word", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_TRIPLE, "Triple", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_TRIPLEDOUBLE, "Double Triple", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_CLASSNAME, "Class Name", m_klass.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_DEFNAME, "Def Name", m_variable.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_OPERATOR, "Operator", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_COMMENTBLOCK, "Comment block", m_multiLineComment.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_STRINGEOL, "String EOL", m_string.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_WORD2, "Word 2", m_keyword.colour, m_background.colour);
    AddProperty(lexer, wxSTC_P_DECORATOR, "Decorator", m_klass.colour, m_background.colour);
    FinalizeImport(lexer);
    return lexer;
}
