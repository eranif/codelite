#include "wx/filename.h"
#include "environmentconfig.h"
EnvironmentConfig* EnvironmentConfig::ms_instance = 0;

//------------------------------------------------------------------------------

EnvironmentConfig::EnvironmentConfig()
{
}

EnvironmentConfig::~EnvironmentConfig()
{
}

EnvironmentConfig* EnvironmentConfig::Instance()
{
	if(ms_instance == 0){
		ms_instance = new EnvironmentConfig();
	}
	return ms_instance;
}

void EnvironmentConfig::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

wxString EnvironmentConfig::GetRootName()
{
	return wxT("EnvironmentVariables");
}

bool EnvironmentConfig::Load()
{
	return ConfigurationToolBase::Load( wxT("config/environment_variables.xml") );
}
