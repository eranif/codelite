#include "abbreviationentry.h"
AbbreviationEntry::AbbreviationEntry()
{
}

AbbreviationEntry::~AbbreviationEntry()
{
}

void AbbreviationEntry::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_entries"), m_entries);
}

void AbbreviationEntry::Serialize(Archive& arch)
{
	arch.Write(wxT("m_entries"), m_entries);
}
