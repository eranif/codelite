#include "externaltoolsdata.h"
ToolInfo::ToolInfo()
{
}

ToolInfo::~ToolInfo()
{
}

void ToolInfo::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_id"), m_id);
	arch.Read(wxT("m_path"), m_path);
	arch.Read(wxT("m_arguments"), m_arguments);
	arch.Read(wxT("m_wd"), m_wd);
	arch.Read(wxT("m_name"), m_name);
	arch.Read(wxT("m_icon16"), m_icon16);
	arch.Read(wxT("m_icon24"), m_icon24);
	
	m_captureOutput = false;
	arch.Read(wxT("m_captureOutput"), m_captureOutput);
}

void ToolInfo::Serialize(Archive& arch)
{
	
	arch.Write(wxT("m_id"), m_id);
	arch.Write(wxT("m_path"), m_path);
	arch.Write(wxT("m_arguments"), m_arguments);
	arch.Write(wxT("m_wd"), m_wd);
	arch.Write(wxT("m_name"), m_name);
	arch.Write(wxT("m_icon16"), m_icon16);
	arch.Write(wxT("m_icon24"), m_icon24);
	arch.Write(wxT("m_captureOutput"), m_captureOutput);
}

//----------------------------------------------------------------
// ExternalToolsData
//----------------------------------------------------------------
ExternalToolsData::ExternalToolsData()
{
}

ExternalToolsData::~ExternalToolsData()
{
}

const std::vector<ToolInfo>& ExternalToolsData::GetTools() const
{
	return m_tools;
}

void ExternalToolsData::SetTools(const std::vector<ToolInfo>& tools)
{
	m_tools = tools;
}

void ExternalToolsData::DeSerialize(Archive& arch)
{
	// first we serialize the list of IDs
	size_t count(0);
	arch.Read(wxT("toolsCount"), count);

	// now loop over the toolsinfo and serialize them
	m_tools.clear();
	for (size_t i=0; i<count; i++) {
		ToolInfo info;
		arch.Read(wxString::Format(wxT("Tool_%d"), i), &info);
		m_tools.push_back(info);
	}
}

void ExternalToolsData::Serialize(Archive& arch)
{
	size_t count = m_tools.size();
	arch.Write(wxT("toolsCount"), count);

	// now loop over the toolsinfo and serialize them
	for (size_t i=0; i<m_tools.size(); i++) {
		arch.Write(wxString::Format(wxT("Tool_%d"), i), &m_tools.at(i));
	}
}
