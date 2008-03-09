#include "Node.h"
#include "Target.h"

MakefileNode::MakefileNode(MakefileNode* parent, Target* target) :
m_parent(parent),
m_target(target)
{
	m_extentions.Add(wxT(".h"));
	m_extentions.Add(wxT(".hpp"));
	m_extentions.Add(wxT(".c"));
	m_extentions.Add(wxT(".cc"));
	m_extentions.Add(wxT(".cpp"));
	m_extentions.Add(wxT(".cxx"));
	//m_extentions.Add(wxT(".o"));
	
	wxArrayString deps = target->getDeps();
	for(wxArrayString::iterator it = deps.begin(); it != deps.end(); it++)
		m_dependees.Add(*it);
}

void MakefileNode::addNodes(std::map<wxString, Target*>& targets)
{
	for(wxArrayString::iterator it = m_dependees.begin(); it != m_dependees.end(); it++)
	{
		Target* target = targets[*it];
		if(target == NULL)
		{
			wxString name = *it;
			wxCharBuffer buf = name.ToAscii();
			// printf("Can't find '%s'\n", buf.data());
			continue;
		}
			
		MakefileNode* child = new MakefileNode(this, target);
		m_children.push_back(child);
	}
	
	for(MakefileNodes::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		MakefileNode* child = *it;
		child->addNodes(targets);
	}

	return;
}

wxString MakefileNode::toString(wxString prefix)
{
	wxString result;
	
	result += prefix;
	result += wxT("Node ");
	result += m_target->getName();
	result += wxT(" {\n");

	result += prefix;	
	result += wxT("Source files:\n");
	
	result += prefix;
	
	wxArrayString actions = m_target->getDeps();	
	for(wxArrayString::iterator it = actions.begin(); it != actions.end(); it++)
	{
		wxString action = *it;
		bool good = false;
		
		for(wxArrayString::iterator it = m_extentions.begin(); it != m_extentions.end(); it++)
		{
			wxString extension = *it;
			if(action.EndsWith(extension))
				good = true;
		}
		
		if(good)
		{
			result += wxT("\t");
			result += action + wxT("\n");
		}
	}
	
	result += wxT("\n");
	result += prefix;
	result += wxT("Child nodes:\n");
	
	for(MakefileNodes::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		MakefileNode* child = *it;
		result += child->toString(prefix + wxT("\t"));
		result += wxT("\n");
	}
	result += wxT("\n");
	result += prefix;
	result += wxT("}\n");
	return result;
}

Target* MakefileNode::getTarget()
{
	return m_target;
}
