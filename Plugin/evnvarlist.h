#ifndef __evnvarlist__
#define __evnvarlist__
#include "serialized_object.h"

class EvnVarList : public SerializedObject {
	StringMap m_variables;

public:
	EvnVarList();
	virtual ~EvnVarList();
	
	void SetVariables(const StringMap& variables) {this->m_variables = variables;}
	const StringMap& GetVariables() const {return m_variables;}
	
public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
};
#endif // __evnvarlist__
