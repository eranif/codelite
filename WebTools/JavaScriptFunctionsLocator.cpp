#include "JavaScriptFunctionsLocator.h"
#include <algorithm>
#include <wx/tokenzr.h>
#include "CxxScannerTokens.h"
#include "JSLexerTokens.h"
#include "fileutils.h"

JavaScriptFunctionsLocator::JavaScriptFunctionsLocator(const wxFileName& filename, const wxString& content)
    : m_state(kNormal)
{
    const wxString jsKeywords = "abstract	arguments	boolean	break	byte "
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
                                "while	with	yield prototype undefined StringtoString NaN";
    wxArrayString keywords = ::wxStringTokenize(jsKeywords, "\t ", wxTOKEN_STRTOK);
    for(size_t i = 0; i < keywords.size(); ++i) {
        m_keywords.insert(keywords.Item(i));
    }

    wxString fileContent = content;
    if(!fileContent.IsEmpty() || FileUtils::ReadFileContent(filename, fileContent)) {
        m_scanner = ::jsLexerNew(fileContent);
    }
}

JavaScriptFunctionsLocator::~JavaScriptFunctionsLocator()
{
    if(m_scanner) {
        ::jsLexerDestroy(&m_scanner);
    }
}

void JavaScriptFunctionsLocator::OnToken(JSLexerToken& token)
{
    // We collect every word which is followed by "(" (except for keywords)

    switch(m_state) {
    //---------------------------------------------------------
    // Normal parsing state, nothing special here
    //---------------------------------------------------------
    case kNormal: {
        switch(token.type) {
        case kJS_IDENTIFIER: {
            if(m_keywords.count(token.text) == 0) {
                // keep it
                m_lastIdentifier = token.text;
            } else {
                // a keyword, skip it
                m_lastIdentifier.clear();
            }
            break;
        }
        case kJS_DOT:
            if(!m_lastIdentifier.IsEmpty()) {
                // a property
                m_properties.insert(m_lastIdentifier);
            }
            m_lastIdentifier.Clear();
            m_state = kScopeOperator;
            break;
        case '(':
            if(!m_lastIdentifier.IsEmpty()) {
                m_functions.insert(m_lastIdentifier);
            }
            m_lastIdentifier.Clear();
            break;
        default:
            m_lastIdentifier.Clear();
            break;
        }
        break;
    }
    //---------------------------------------------------------
    // Previous match was "."
    //---------------------------------------------------------
    case kScopeOperator: {
        if(token.type == kJS_IDENTIFIER) {
            wxString word = token.text;
            if(m_keywords.count(word) == 0) {
                m_functions.insert(word);
            }
            m_lastIdentifier.clear();
        }

        // Back to normal state
        m_lastIdentifier.Clear();
        m_state = kNormal;
        break;
    }
    }
}

wxString JavaScriptFunctionsLocator::GetFunctionsString() const
{
    wxString str;
    std::for_each(m_functions.begin(), m_functions.end(), [&](const wxString& func) { str << func << " "; });
    return str;
}

wxString JavaScriptFunctionsLocator::GetPropertiesString() const
{
    wxString str;
    std::for_each(m_properties.begin(), m_properties.end(), [&](const wxString& prop) { str << prop << " "; });
    return str;
}

void JavaScriptFunctionsLocator::Parse()
{
    if(!m_scanner) return;
    JSLexerToken token;
    while(::jsLexerNext(m_scanner, token)) {
        OnToken(token);
    }
}
