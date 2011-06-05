#include "db_explorer_settings.h"

DbExplorerSettings::DbExplorerSettings()
{
}

DbExplorerSettings::~DbExplorerSettings()
{
}

void DbExplorerSettings::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_recentFiles"), m_recentFiles);
}

void DbExplorerSettings::Serialize(Archive& arch)
{
	arch.Write(wxT("m_recentFiles"), m_recentFiles);
}

void DbExplorerSettings::SetRecentFiles(const wxArrayString& recentFiles)
{
	m_recentFiles.Clear();
	for(size_t i=0; i<recentFiles.Count(); i++) {
		if(m_recentFiles.Index(recentFiles.Item(i)) == wxNOT_FOUND) {
			m_recentFiles.Add(recentFiles.Item(i));
		}
	}
}
