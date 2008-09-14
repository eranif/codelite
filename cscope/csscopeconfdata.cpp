#include "csscopeconfdata.h"
CSscopeConfData::CSscopeConfData()
		: m_scanScope(SCOPE_ENTIRE_WORKSPACE)
{
}

CSscopeConfData::~CSscopeConfData()
{
}

void CSscopeConfData::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_scanScope"), m_scanScope);
}

void CSscopeConfData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_scanScope"), m_scanScope);
}
