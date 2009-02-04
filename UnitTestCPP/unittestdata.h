#ifndef __unittestdata__
#define __unittestdata__

#include "serialized_object.h"

class UnitTestData : public SerializedObject {

public:
	UnitTestData();
	~UnitTestData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
};
#endif // __unittestdata__
