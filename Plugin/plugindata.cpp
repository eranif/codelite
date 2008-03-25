#include "plugindata.h"
PluginInfo::PluginInfo()
{
}

PluginInfo::~PluginInfo()
{
}

void PluginInfo::DeSerialize(Archive &arch)
{
	arch.Read(wxT("enabled"), enabled);
	arch.Read(wxT("name"), name);
}
void PluginInfo::Serialize(Archive &arch)
{
	arch.Write(wxT("enabled"), enabled);
	arch.Write(wxT("name"), name);
}
