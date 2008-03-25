#ifndef __plugindata__
#define __plugindata__
#include "serialized_object.h"

class PluginInfo : public SerializedObject {
	
	bool enabled;
	wxString name;
	
public:
	PluginInfo();
	virtual ~PluginInfo();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	
	//Setters
	void SetEnabled(const bool& enabled) {this->enabled = enabled;}
	void SetName(const wxString& name) {this->name = name;}
	//Getters
	const bool& GetEnabled() const {return enabled;}
	const wxString& GetName() const {return name;}
	
};
#endif // __plugindata__
