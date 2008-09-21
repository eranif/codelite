#include "confformbuilder.h"
ConfFormBuilder::ConfFormBuilder()
: m_command(wxT("$(wxfb) $(wxfb_project"))
{
}

ConfFormBuilder::~ConfFormBuilder()
{
}

void ConfFormBuilder::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_command"), m_command);
	arch.Read(wxT("m_fbPath"), m_fbPath);
}

void ConfFormBuilder::Serialize(Archive& arch)
{
	arch.Write(wxT("m_command"), m_command);
	arch.Write(wxT("m_fbPath"), m_fbPath);
}
