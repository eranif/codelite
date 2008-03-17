#include "svnoptions.h"

SvnOptions::SvnOptions()
: m_flags(SvnUseIcons | SvnKeepIconsUpdated | SvnAutoAddFiles)
, m_exePath(wxT("svn"))
, m_pattern(wxT("*.o;*.obj;*.exe;*.lib;*.so;*.dll;*.a;*.dynlib;*.exp;*.ilk;*.pdb;*.d;*.tags;*.suo;*.ncb;"))
{
}

SvnOptions::~SvnOptions()
{
}

void SvnOptions::Serialize(Archive &arch)
{
	arch.Write(wxT("m_flags"), m_flags);
	arch.Write(wxT("m_exePath"), m_exePath);
	arch.Write(wxT("m_pattern"), m_pattern);
}

void SvnOptions::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_flags"), m_flags);
	arch.Read(wxT("m_exePath"), m_exePath);
	arch.Read(wxT("m_pattern"), m_pattern);
}

