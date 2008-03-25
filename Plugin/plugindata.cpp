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
	arch.Read(wxT("author"), author);
	arch.Read(wxT("description"), description);
	arch.Read(wxT("version"), version);
}
void PluginInfo::Serialize(Archive &arch)
{
	arch.Write(wxT("enabled"), enabled);
	arch.Write(wxT("name"), name);
	arch.Write(wxT("author"), author);
	arch.Write(wxT("description"), description);
	arch.Write(wxT("version"), version);
}

