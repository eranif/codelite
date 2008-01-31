#ifndef __environmentconfig__
#define __environmentconfig__

#include "configurationtoolbase.h"
#include "archive.h"

class EnvironmentConfig : public ConfigurationToolBase {

	static EnvironmentConfig* ms_instance;
	StringMap m_envSnapshot;
	
public:
	static EnvironmentConfig* Instance();
	static void Release();
	bool Load();  
	wxString ExpandVariables(const wxString &in); 
	void ApplyEnv();
	void UnApplyEnv();
	 
private:
	EnvironmentConfig();
	virtual ~EnvironmentConfig();

public:
	virtual wxString GetRootName();
};
#endif // __environmentconfig__
