#ifndef __qmakeplugindata__
#define __qmakeplugindata__

#include <wx/string.h>
#include <map>

class QmakePluginData
{
public:
	class BuildConfPluginData
	{
	public:
		bool          m_enabled;
		wxString      m_buildConfName;
		wxString      m_qmakeConfig;
		wxString      m_qmakeExecutionLine;
		wxString      m_freeText;

	public:
		BuildConfPluginData() : m_enabled (false), m_qmakeExecutionLine(wxT("$(QMAKE)")) {}

	};

private:
	std::map<wxString, BuildConfPluginData> m_pluginsData;

public:
	QmakePluginData(const wxString &data);
	~QmakePluginData();

	wxString   ToString();
	bool       GetDataForBuildConf(const wxString &configName, BuildConfPluginData &bcpd    );
	void       SetDataForBuildConf(const wxString &configName, const BuildConfPluginData &cd);
};
#endif // __qmakeplugindata__
