#include "copyrightsconfigdata.h"
CopyrightsConfigData::CopyrightsConfigData()
{
}

CopyrightsConfigData::~CopyrightsConfigData()
{
}

void CopyrightsConfigData::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_templateFilename"), m_templateFilename);
	arch.Read(wxT("m_fileMasking"), m_fileMasking);
}

void CopyrightsConfigData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_templateFilename"), m_templateFilename);
	arch.Write(wxT("m_fileMasking"), m_fileMasking);
}
