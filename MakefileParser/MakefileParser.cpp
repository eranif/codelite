#include "MakefileParser.h"
#include "LineTypes.h"

MakefileParser::MakefileParser(wxArrayString lines) :
m_lines(lines),
m_current(0)
{ 
	Lex();
}

/**
 * Parse all the strings and find their type.
 * Could be replaced by a tool such as Lexx 
 */ 
void MakefileParser::Lex()
{		
	for (; m_current < m_lines.size(); m_current++)
	{
		wxString current = m_lines[m_current];
		
		LINETYPES::LineType currentType = Deduct();
		
		/*info(type);
		printf(" line\t: %s\n", it->c_str());*/
		
		LINETYPES::TypedString addme;
		addme.line = current;
		addme.type = currentType;
		
		m_result.push_back(addme);
		
		// Ugly solution, this way we don't support the following:
		// blabla "this is a # sign " dum
		m_continued = (current.size() > 0) && (current[current.size()-1] == '\\') && (current.find('#') == (size_t)-1);
	}
	
}

/**
 * Deduct from the current line, and wether the last line ended in a '\', what kind of line this is.
 * Could be replaced by a tool like Lexx 
 */ 
LINETYPES::LineType MakefileParser::Deduct()
{	
	wxString current = m_lines[m_current];
	
	if(current.size() == 0)
		return LINETYPES::LINE_EMPTY;
	
	int colon = (int)current.find(':');
	int tab = (int)current.find('\t');
	int pound = (int)current.find('#');
	
	if(pound == 0)
		return LINETYPES::LINE_COMMENT;
		
	bool empty = true;
	
	if(current.find_first_not_of(wxT("\n\r\t ")) != (size_t)-1)
	{
		empty = false;
	}
		
	if(empty)
		return LINETYPES::LINE_EMPTY;
	
	if(tab == 0)
	{
		if(m_continued)
			return LINETYPES::LINE_DEPEND;
		else
			return LINETYPES::LINE_ACTION;			
	}
	
	if(isalpha(current[0])) // could be a target
	{	
		if(colon != -1)
			return LINETYPES::LINE_TARGET;
			
		return LINETYPES::LINE_COMMAND;
			
	}
	
	return LINETYPES::LINE_INVALID;
}

/**
 * Get result, constructor ensures that m_result is available when we get to this point.
 */ 
TypedStrings MakefileParser::getResult()
{
	return m_result;
}
