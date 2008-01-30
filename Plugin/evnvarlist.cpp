#include "evnvarlist.h"
EvnVarList::EvnVarList()
{
}

EvnVarList::~EvnVarList()
{
}

void EvnVarList::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_variables"), m_variables);
}

void EvnVarList::Serialize(Archive &arch)
{
	arch.Write(wxT("m_variables"), m_variables);
}
