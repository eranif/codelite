#include "EclipseJavaScriptThemeImporter.h"
#include "globals.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "xmlutils.h"
#include "wx/dir.h"
#include <wx/stc/stc.h>

EclipseJavaScriptThemeImporter::EclipseJavaScriptThemeImporter()
{
    // Reserved words
    SetKeywords0("abstract	arguments	boolean	break	byte "
                 "case	catch	char	class*	const "
                 "continue	debugger	default	delete	do "
                 "double	else	enum*	eval	export* "
                 "extends*	false	final	finally	float "
                 "for	function	goto	if	implements "
                 "import*	in	instanceof	int	interface "
                 "let	long	native	new	null "
                 "package	private	protected	public	return "
                 "short	static	super*	switch	synchronized "
                 "this	throw	throws	transient	true "
                 "try	typeof	var	void	volatile "
                 "while	with	yield prototype undefined StringtoString NaN ");

    // Used for wxSTC_C_WORD2 ("functions")
    SetKeywords1(
        "activeElement addEventListener adoptNode	anchors applets "
        "baseURI body close cookie createAttribute createComment createDocumentFragment createElement createTextNode "
        "doctype documentElement documentMode documentURI domain "
        "domConfig embeds forms getElementById getElementsByClassName getElementsByName getElementsByTagName hasFocus "
        "head images implementation importNode inputEncoding lastModified links normalize normalizeDocument open "
        "querySelector"
        "querySelectorAll readyState referrer removeEventListener renameNode scripts strictErrorChecking title URL "
        "write writeln Math NaN name NumberObject valueOf");

    // Used for wxSTC_C_GLOBALCLASS ("classes")
    SetKeywords3("Math Array Date document window");

    SetFileExtensions("*.js;*.javascript;*.qml;*.json");
}

EclipseJavaScriptThemeImporter::~EclipseJavaScriptThemeImporter() {}

LexerConf::Ptr_t EclipseJavaScriptThemeImporter::Import(const wxFileName& eclipseXmlFile)
{
    LexerConf::Ptr_t lexer = InitializeImport(eclipseXmlFile, "javascript", 3);
    CHECK_PTR_RET_NULL(lexer);

    // Covnert to codelite's XML properties
    AddProperty(lexer, wxSTC_C_DEFAULT, "Default", m_foreground.colour, m_background.colour);
    AddProperty(lexer,
                wxSTC_C_COMMENT,
                "Block comment",
                m_multiLineComment.colour,
                m_background.colour,
                m_multiLineComment.isBold,
                m_multiLineComment.isItalic);
    AddProperty(lexer,
                wxSTC_C_COMMENTLINE,
                "Single line comment",
                m_singleLineComment.colour,
                m_background.colour,
                m_singleLineComment.isBold,
                m_singleLineComment.isItalic);
    AddProperty(lexer,
                wxSTC_C_COMMENTDOC,
                "Doxygen block comment",
                m_javadoc.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(
        lexer, wxSTC_C_NUMBER, "Number", m_number.colour, m_background.colour, m_number.isBold, m_number.isItalic);
    AddProperty(lexer,
                wxSTC_C_WORD,
                "JavaScript keyword",
                m_keyword.colour,
                m_background.colour,
                m_keyword.isBold,
                m_keyword.isItalic);
    AddProperty(
        lexer, wxSTC_C_STRING, "String", m_string.colour, m_background.colour, m_string.isBold, m_string.isItalic);
    AddProperty(lexer,
                wxSTC_C_CHARACTER,
                "Character",
                m_string.colour,
                m_background.colour,
                m_string.isBold,
                m_string.isItalic);
    AddProperty(lexer, wxSTC_C_OPERATOR, "Operator", m_foreground.colour, m_background.colour);
    AddProperty(lexer, wxSTC_C_IDENTIFIER, "Identifier", m_foreground.colour, m_background.colour);
    AddProperty(lexer,
                wxSTC_C_STRINGEOL,
                "Open String",
                m_string.colour,
                m_background.colour,
                m_string.isBold,
                m_string.isItalic);
    AddProperty(lexer,
                wxSTC_C_COMMENTLINEDOC,
                "Doxygen single line comment",
                m_javadoc.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer,
                wxSTC_C_WORD2,
                "JavaScript functions",
                m_variable.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer,
                wxSTC_C_GLOBALCLASS,
                "JavaScript global classes",
                m_klass.colour,
                m_background.colour,
                m_javadoc.isBold,
                m_javadoc.isItalic);
    AddProperty(lexer,
                wxSTC_C_COMMENTDOCKEYWORD,
                "Doxygen keyword",
                m_javadocKeyword.colour,
                m_background.colour,
                m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    AddProperty(lexer,
                wxSTC_C_COMMENTDOCKEYWORDERROR,
                "Doxygen keyword error",
                m_javadocKeyword.colour,
                m_background.colour,
                m_javadocKeyword.isBold,
                m_javadocKeyword.isItalic);
    FinalizeImport(lexer);
    return lexer;
}
