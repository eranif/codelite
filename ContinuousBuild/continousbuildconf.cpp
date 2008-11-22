#include "continousbuildconf.h"
ContinousBuildConf::ContinousBuildConf()
		: m_enabled(false)
		, m_parallelProcesses(1)
{
}

ContinousBuildConf::~ContinousBuildConf()
{
}

void ContinousBuildConf::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_enabled"), m_enabled);
	arch.Read(wxT("m_parallelProcesses"), m_parallelProcesses);
}

void ContinousBuildConf::Serialize(Archive& arch)
{
	arch.Write(wxT("m_enabled"), m_enabled);
	arch.Write(wxT("m_parallelProcesses"), m_parallelProcesses);
}
