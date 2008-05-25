#include "copyrightsconfigdata.h"
CopyrightsConfigData::CopyrightsConfigData()
: m_templateFilename(wxEmptyString)
, m_fileMasking(wxT("*.h;*.hpp;*.h++"))
, m_backupFiles(true)
{
}

CopyrightsConfigData::~CopyrightsConfigData()
{
}

void CopyrightsConfigData::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_templateFilename"), m_templateFilename);
	arch.Read(wxT("m_fileMasking"), m_fileMasking);
	arch.Read(wxT("m_backupFiles"), m_backupFiles);
}

void CopyrightsConfigData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_templateFilename"), m_templateFilename);
	arch.Write(wxT("m_fileMasking"), m_fileMasking);
	arch.Write(wxT("m_backupFiles"), m_backupFiles);
}
