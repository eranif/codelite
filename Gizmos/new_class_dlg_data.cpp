#include "new_class_dlg_data.h"

NewClassDlgData::NewClassDlgData()
	: m_flags(UseUnderscores )
{
}

NewClassDlgData::~NewClassDlgData()
{
}

void NewClassDlgData::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_flags"), m_flags);
}

void NewClassDlgData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_flags"), m_flags);
}
