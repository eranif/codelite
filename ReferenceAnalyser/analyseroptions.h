#ifndef ANALYSEROPTIONS_H
#define ANALYSEROPTIONS_H

#include "serialized_object.h"

class AnalyserOptions : public SerializedObject {
public:
	AnalyserOptions();
	virtual ~AnalyserOptions();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);
};

#endif //ANALYSEROPTIONS_H
