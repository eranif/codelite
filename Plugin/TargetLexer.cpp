#include "TargetLexer.h"
#include "Target.h"

TargetLexer::TargetLexer(TypedStrings lines) :
m_lines(lines),
m_current(0) 
{
	Lex();
}

void TargetLexer::Lex()
{
	for(int i = 0; FindTarget(); i++)
	{
		wxString nameanddep = m_lines[m_current].line; // target: test.o
		size_t to = nameanddep.find(':');
		
		if(to == (size_t)-1) 
		{
			printf("Found a target but no colon in it.");
			exit(-1);
		}
		
		wxString name = nameanddep.substr(0, to);
		wxString firstdep = nameanddep.substr(to+1);
		wxArrayString deps;		
		
		if(firstdep.size() > 0)
		{
			deps.push_back(firstdep);
		}
		
		m_current++;
		
		while(FindDeps())
		{
			wxString dep = m_lines[m_current].line;
			deps.push_back(dep);
			m_current++;
		}
		
		wxArrayString actions;
		while(FindActions())
		{
			wxString action = m_lines[m_current].line;
			actions.push_back(action);
			m_current++;
		}
		
		Target targ(name, deps, actions);
		m_result.push_back(targ);
	}
	
}

bool TargetLexer::FindTarget()
{
	for(; m_current < m_lines.size(); m_current++)
	{
		TypedString current = m_lines[m_current];
		
		// this is the first one of this batch, untill we get a target, skip the rest
		// Note: it SHOULD be just comments and empty lines
		if(current.type != LINETYPES::LINE_TARGET)	
			continue;
			
		return true;
	}
	return false;
}

bool TargetLexer::FindDeps()
{
	for(; m_current < m_lines.size(); m_current++)
	{
		TypedString current = m_lines[m_current];
		
		if(current.type == LINETYPES::LINE_INVALID)
		{
			printf("Invalid line found?!");
			exit(-1);
		}			

		if(current.type == LINETYPES::LINE_DEPEND)	
			return true;

		if(current.type == LINETYPES::LINE_COMMAND)
			continue;

		if(current.type == LINETYPES::LINE_COMMENT)
			continue;

		if(current.type == LINETYPES::LINE_EMPTY)
			continue;
		
		return false;
	}
	return false;
}

bool TargetLexer::FindActions()
{
	for(; m_current < m_lines.size(); m_current++)
	{
		TypedString current = m_lines[m_current];
		
		if(current.type == LINETYPES::LINE_INVALID)
		{
			printf("Invalid line found?!");
			exit(-1);
		}			

		if(current.type == LINETYPES::LINE_ACTION)	
			return true;

		if(current.type == LINETYPES::LINE_COMMAND)
			continue;

		if(current.type == LINETYPES::LINE_COMMENT)
			continue;
			
		if(current.type == LINETYPES::LINE_EMPTY)
			continue;
		
		return false;
	}
	return false;
}

Targets TargetLexer::getResult()
{
	return m_result;
}
