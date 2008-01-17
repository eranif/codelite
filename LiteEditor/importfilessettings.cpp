#include "importfilessettings.h"

ImportFilesSettings::ImportFilesSettings()
{
}

ImportFilesSettings::~ImportFilesSettings()
{
}

void ImportFilesSettings::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_fileMask"), m_fileMask);
	arch.Read(wxT("m_flags"), m_flags);
}

void ImportFilesSettings::Serialize(Archive &arch)
{
	arch.Write(wxT("m_fileMask"), m_fileMask);
	arch.Write(wxT("m_flags"), m_flags);
}
