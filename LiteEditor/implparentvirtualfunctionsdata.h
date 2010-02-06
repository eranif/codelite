#ifndef IMPLPARENTVIRTUALFUNCTIONSDATA_H
#define IMPLPARENTVIRTUALFUNCTIONSDATA_H

#include "serialized_object.h" // Base class

class ImplParentVirtualFunctionsData : public SerializedObject
{
	size_t m_flags;
public:
	enum {
		InsertDoxygenComment   = 0x00000001,
		FormatText             = 0x00000002,
		PrependVirtual         = 0x00000004,
		ImplParentVFuncDefault = InsertDoxygenComment | FormatText
	};

public:
	ImplParentVirtualFunctionsData()
			: m_flags(ImplParentVFuncDefault) {
	}

	virtual ~ImplParentVirtualFunctionsData() {
	}

public:
	virtual void DeSerialize(Archive &arch) {
		arch.Read(wxT("m_flags"), m_flags);
	}

	virtual void Serialize(Archive &arch) {
		arch.Write(wxT("m_flags"), m_flags);
	}

	void SetFlags(const size_t& flags) {
		this->m_flags = flags;
	}

	const size_t& GetFlags() const {
		return m_flags;
	}
};

#endif // IMPLPARENTVIRTUALFUNCTIONSDATA_H
