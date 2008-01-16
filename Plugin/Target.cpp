#include "Target.h"

#include "wx/string.h"
#include "wx/arrstr.h"
#include "tokenizer.h"

Target::Target(wxString name, wxArrayString deps, wxArrayString actions) : 
m_name(name),
m_deps(deps),
m_actions(actions)
{
	stripWhitespace();
	splitDepencencies();
}

void Target::stripWhitespace()
{
        for(size_t i = 0; i < m_deps.size(); i++)
        {
                wxString* dep = &m_deps[i];

                size_t to = dep->find_first_not_of(wxT("\n\r\t "));
                if(to != (size_t)-1)
                        dep->erase(0, to);

                size_t from = dep->find_last_not_of(wxT("\\\t\n\r "))+1;
                if(from != (size_t)-1)
                        dep->erase(from);
        }

        for(size_t i = 0; i < m_actions.size(); i++)
        {
                wxString* action = &m_actions[i];

                size_t to = action->find_first_not_of(wxT("\n\r\t "));
                if(to != (size_t)-1)
                        action->erase(0, to);

                size_t from = action->find_last_not_of(wxT("\\\t\n\r "))+1;
                if(from != (size_t)-1)
                        action->erase(from);
        }
}

void Target::splitDepencencies()
{
	wxArrayString deps;
	for(size_t i = 0; i < m_deps.size(); i++)
	{
		m_deps[i].Replace(wxT("\t"),wxT(" ")); // just replace all tabs by spaces
		StringTokenizer tokenizer(m_deps[i]); // tokenize at spaces
		for(int j = 0; j < tokenizer.Count(); j++)
		{
			deps.push_back(tokenizer[j]);
		}
		m_deps[i].Clear();
	}
	m_deps.Clear();
	
	m_deps = deps;
}

wxString Target::getName()
{
	return m_name;
}

wxArrayString Target::getDeps()
{
	return m_deps;
}

wxArrayString Target::getActions()
{
	return m_actions;
}
