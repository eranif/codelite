#include "setters_getters_data.h"

SettersGettersData::SettersGettersData()
{
}


SettersGettersData::~SettersGettersData()
{
}

void SettersGettersData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_flags"), m_flags);
}

void SettersGettersData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_flags"), m_flags);
}


