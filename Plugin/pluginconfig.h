#ifndef __pluginconfig__
#define __pluginconfig__
#include "configtool.h"

class PluginConfig : public ConfigTool {

	static PluginConfig* ms_instance;

public:
	static PluginConfig* Instance();
	static void Release();

private:
	PluginConfig();
	virtual ~PluginConfig();

};
#endif // __pluginconfig__
