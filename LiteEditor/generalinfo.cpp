#include "generalinfo.h"

GeneralInfo::GeneralInfo() 
: m_frameSize(800, 600)
, m_framePos(0, 0)
, m_flags(CL_SHOW_WELCOME_PAGE | CL_LOAD_LAST_SESSION | CL_USE_EOL_LF)
{
}

GeneralInfo::~GeneralInfo()
{
}

void GeneralInfo::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_frameSize"), m_frameSize);
	arch.Read(wxT("m_framePos"), m_framePos);
	arch.Read(wxT("m_flags"), m_flags);
}

void GeneralInfo::Serialize(Archive &arch)
{
	arch.Write(wxT("m_frameSize"), m_frameSize);
	arch.Write(wxT("m_framePos"), m_framePos);
	arch.Write(wxT("m_flags"), m_flags);
}
