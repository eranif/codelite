#ifndef __pluginsdata__
#define __pluginsdata__

#include "plugindata.h"
#include "map"

class PluginsData : public SerializedObject
{
	std::map< wxString, PluginInfo > m_info;

public:
	PluginsData();
	virtual ~PluginsData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetInfo(const std::map<wxString, PluginInfo>& info) {
		this->m_info = info;
	}
	
	const std::map<wxString, PluginInfo>& GetInfo() const {
		return m_info;
	}
};

#endif // __pluginsdata__
