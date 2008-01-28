#include "analyseroptions.h"

AnalyserOptions::AnalyserOptions() :
m_containerName(wxT("SmartPtr")),
m_containerOperator(wxT("->"))
{
}

AnalyserOptions::~AnalyserOptions()
{
}

void AnalyserOptions::Serialize(Archive &arch)
{
	arch.Write(wxT("m_containerName"), m_containerName);
	arch.Write(wxT("m_containerOperator"), m_containerOperator);
}

void AnalyserOptions::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_containerName"), m_containerName);
	arch.Read(wxT("m_containerOperator"), m_containerOperator);
}

