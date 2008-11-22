#ifndef __continousbuildconf__
#define __continousbuildconf__

#include "serialized_object.h"

class ContinousBuildConf : public SerializedObject
{
	bool m_enabled;
	size_t m_parallelProcesses;

public:
	ContinousBuildConf();
	virtual ~ContinousBuildConf();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	void SetEnabled(const bool& enabled) {
		this->m_enabled = enabled;
	}
	void SetParallelProcesses(const size_t& parallelProcesses) {
		this->m_parallelProcesses = parallelProcesses;
	}
	const bool& GetEnabled() const {
		return m_enabled;
	}
	const size_t& GetParallelProcesses() const {
		return m_parallelProcesses;
	}
};
#endif // __continousbuildconf__
