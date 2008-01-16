#ifndef SETTERS_GETTERS_DATA_H
#define SETTERS_GETTERS_DATA_H

#include "serialized_object.h"

enum SettersGetters {
	SG_KEEP_UNDERSCORES,
	SG_LOWER_CASE_LETTER,
	SG_REMOVE_M_PREFIX
};

class SettersGettersData : public SerializedObject
{
	size_t m_flags;
public:
	SettersGettersData();
	virtual ~SettersGettersData();

	const size_t &GetFlags() const {return m_flags;}
	void SetFlags(const size_t &flags){m_flags = flags;}

	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);
};

#endif // SETTERS_GETTERS_DATA_H


