#include "tagsmanagementconf.h"
TagsManagementConf::TagsManagementConf()
{
}

TagsManagementConf::~TagsManagementConf()
{
}

void TagsManagementConf::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_files"), m_files);
}

void TagsManagementConf::Serialize(Archive& arch)
{
	arch.Write(wxT("m_files"), m_files);
}
