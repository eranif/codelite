#include "JavaScriptFunctionsLocator.h"
#include <algorithm>
#include <wx/tokenzr.h>
#include "CxxScannerTokens.h"

JavaScriptFunctionsLocator::JavaScriptFunctionsLocator(CxxPreProcessor* preProcessor, const wxFileName& filename)
    : CxxScannerBase(preProcessor, filename)
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
}

JavaScriptFunctionsLocator::~JavaScriptFunctionsLocator() {}

void JavaScriptFunctionsLocator::OnToken(CxxLexerToken& token)
{
    // We collect every word which is followed by "(" (except for keywords)
    switch(token.type) {
    case T_IDENTIFIER: {
        wxString word = token.text;
        if(m_keywords.count(word) == 0) {
            // keep it
            m_lastIdentifier.swap(word);
        } else {
            // a keyword, skip it
            m_lastIdentifier.clear();
        }
        break;
    }
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
}

wxString JavaScriptFunctionsLocator::GetString() const
{
    wxString str;
    std::for_each(m_functions.begin(), m_functions.end(), [&](const wxString& func) {
        str << " ";
    });
    return str;
}
