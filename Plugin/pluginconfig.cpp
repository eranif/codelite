#include "pluginconfig.h"
PluginConfig* PluginConfig::ms_instance = 0;

PluginConfig::PluginConfig()
{
}

PluginConfig::~PluginConfig()
{
}

PluginConfig* PluginConfig::Instance()
{
	if(ms_instance == 0){
		ms_instance = new PluginConfig();
	}
	return ms_instance;
}

void PluginConfig::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

