#include "svnoptions.h"

SvnOptions::SvnOptions()
: m_flags(SVN_DEFAULT_FLAGS)
, m_refreshInterval(500)
, m_exePath(wxT("svn"))
{
}

SvnOptions::~SvnOptions()
{
}

void SvnOptions::Serialize(Archive &arch)
{
	arch.Write(wxT("m_flags"), m_flags);
	arch.Write(wxT("m_refreshInterval"), m_refreshInterval);
	arch.Write(wxT("m_exePath"), m_exePath);
}

void SvnOptions::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_flags"), m_flags);
	arch.Read(wxT("m_refreshInterval"), m_refreshInterval);
	arch.Read(wxT("m_exePath"), m_exePath);
}

