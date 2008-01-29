#ifndef __environmentconfig__
#define __environmentconfig__

#include "configurationtoolbase.h"

class EnvironmentConfig : public ConfigurationToolBase {

	static EnvironmentConfig* ms_instance;

public:
	static EnvironmentConfig* Instance();
	static void Release();
	bool Load(); 
	
private:
	EnvironmentConfig();
	virtual ~EnvironmentConfig();

public:
	virtual wxString GetRootName();
};
#endif // __environmentconfig__
